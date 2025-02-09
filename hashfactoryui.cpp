#include "hashfactoryui.h"
#include "ui_hashfactoryui.h"
#include <QFileDialog>
#include <QPushButton>
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include "databasehandler.hpp"



HashFactoryUI::HashFactoryUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HashFactoryUI)
{
    ui->setupUi(this);

    connect(ui->pathFileLineEdit, &QLineEdit::selectionChanged, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(nullptr, "Select File", "", "All Files (*.*)");
        if (!filePath.isEmpty()) {
            ui->pathFileLineEdit->setText(filePath);
        }
    });

    connect(ui->calculateButton,&QPushButton::clicked,this,&HashFactoryUI::onCalculateClicked);
    connect(ui->loadTable,&QPushButton::clicked,this,&HashFactoryUI::onLoadData);
    connect(ui->filterButton,&QPushButton::clicked,this,&HashFactoryUI::onSearchClicked);
    connect(ui->addToDB,&QPushButton::clicked,this,&HashFactoryUI::onAddClicked);


    encryptedAlgorithm.insert("MD5",QCryptographicHash::Md5);
    encryptedAlgorithm.insert("SHA-256",QCryptographicHash::Sha256);
    encryptedAlgorithm.insert("SHA-1",QCryptographicHash::Sha1);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); // Path column expands

    connectToDatabase();
    loadData();

}


void HashFactoryUI::loadData() {
    // Clear existing data
    //ui->tableWidget->clear();
    ui->tableWidget->clearContents();
    // Fetch data from the database
    QSqlQuery query;
    if (!query.exec("SELECT filename,type,algorithm,hash,creation_date FROM hash_list ORDER BY id DESC LIMIT 50")) {
        qDebug() << "Error: Could not fetch records.";
        qDebug() << query.lastError().text();
        return;
    }

    showOnTable(query);
}

void HashFactoryUI::onLoadData()
{
    loadData();
}

void HashFactoryUI::onSearchClicked()
{
    QString searchType = ui->columnFilterBox->currentText();
    QString searchValue = ui->filterSearch->text();
    QSqlQuery query;
    if (searchType == "FileName"){
        query = fetchRecordByFileName(searchValue);
    }
    else if (searchType == "Checker"){
        query = fetchRecordByAlgorithm(searchValue);
    }
    else if (searchType == "Hash"){
       query =  fetchRecordByHash(searchValue);
    }

    showOnTable(query);
}

void HashFactoryUI::onAddClicked()
{
    QString fileName = ui->pathFileLineEdit->text().section('/', -1);
    int typeId = fetchTypeID(ui->typeCombo->currentText());
    int algorithmId = fetchAlgorithmId(ui->checkerComboBox->currentText());
    QString hash = ui->calculatedHashFile->text();
    int timeNow = QDateTime::currentSecsSinceEpoch();

    addToTable(fileName,hash,algorithmId,typeId,timeNow);

}
void HashFactoryUI::onCalculateClicked()
{
    QFile file(ui->pathFileLineEdit->text());
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file.");
        //return QByteArray();
    }
    QCryptographicHash hash(encryptedAlgorithm[ui->checkerComboBox->currentText()]);
    if (hash.addData(&file)) {
        ui->calculatedHashFile->setText(hash.result().toHex());
    }

    int id = findHash(ui->calculatedHashFile->text());
    QSqlQuery query = fetchRecordById(id);

    showOnTable(query);


}

void HashFactoryUI::showOnTable(QSqlQuery &query)
{
    int row = 0;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    while (query.next()) {
        ui->tableWidget->insertRow(row);
        QTableWidgetItem *fileName = new QTableWidgetItem(query.value(0).toString());
        ui->tableWidget->setItem(row, 1, fileName);

        QTableWidgetItem *type = new QTableWidgetItem(fetchType(query.value(1).toInt()));
        ui->tableWidget->setItem(row, 2, type);

        QTableWidgetItem *algo = new QTableWidgetItem(fetchAlgorithm(query.value(2).toInt()));
        ui->tableWidget->setItem(row, 3, algo);

        QTableWidgetItem *hash = new QTableWidgetItem(query.value(3).toString());
        ui->tableWidget->setItem(row, 4, hash);

        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(query.value(4).toInt());
        QTableWidgetItem *creationDate = new QTableWidgetItem(dateTime.toString());
        ui->tableWidget->setItem(row, 5, creationDate);
        ++row;
    }
}
HashFactoryUI::~HashFactoryUI()
{
    delete ui;
}
