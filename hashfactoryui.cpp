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
#include <QDesktopServices>


HashFactoryUI::HashFactoryUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HashFactoryUI)
{
    ui->setupUi(this);

    connect(ui->calculateButton,&QPushButton::clicked,this,&HashFactoryUI::onCalculateClicked);
    connect(ui->browseButton,&QPushButton::clicked,this,&HashFactoryUI::onBrowseClicked);
    connect(ui->loadTable,&QPushButton::clicked,this,&HashFactoryUI::onLoadData);
    connect(ui->filterButton,&QPushButton::clicked,this,&HashFactoryUI::onSearchClicked);
    connect(ui->addToDB,&QPushButton::clicked,this,&HashFactoryUI::onAddClicked);
    connect(ui->exportButton,&QPushButton::clicked,this,&HashFactoryUI::onExportClicked);
    connect(ui->importButton,&QPushButton::clicked,this,&HashFactoryUI::onImportClicked);
    connect(ui->actionExit, &QAction::triggered, this, &HashFactoryUI::onActionExit);
    connect(ui->actionAbout, &QAction::triggered, this, &HashFactoryUI::onActionAbout);
    connect(ui->actionHelp, &QAction::triggered, this, &HashFactoryUI::onActionHelp);


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

void HashFactoryUI::onActionExit()
{
    this->close();
}

void HashFactoryUI::onActionAbout()
{
    QUrl url("https://github.com/MiladSabri74/MiladSabri74"); // Replace with your desired URL
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox::critical(this, "Error", "Could not open URL.");
    }
}

void HashFactoryUI::onActionHelp()
{
    QUrl url("https://github.com/MiladSabri74/HashFactory/wiki"); // Replace with your desired URL
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox::critical(this, "Error", "Could not open URL.");
    }
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

void HashFactoryUI::onBrowseClicked()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Select File", "", "All Files (*.*)");
    if (!filePath.isEmpty()) {
        ui->pathFileLineEdit->setText(filePath);
    }
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

void HashFactoryUI::onImportClicked()
{
    // Open a file dialog to choose the CSV file
    QString fileName = QFileDialog::getOpenFileName(this, "Open CSV File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        return; // User canceled the dialog
    }


    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for reading.");
        return;
    }

    QTextStream in(&file);

    // Read the CSV file line by line
    //read headerline
    QString line = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(","); // Split by comma (adjust delimiter if needed)

        qDebug() << fields.size();
        if (fields.size() != 4) { // Example: Expecting 3 columns
            continue;
        }

        // Insert the data into the database
        if (!insertIntoDatabase(fields)) {
            qDebug("while");
            QMessageBox::critical(this, "Error", "Failed to insert data into the database.");
            break;
        }
    }
    qDebug("close");
    file.close();
    QMessageBox::information(this, "Success", "Data imported from CSV file successfully.");
    qDebug("load");
    loadData();
}


void HashFactoryUI::onExportClicked()
{
    // Open a file dialog to choose the save location
    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        return; // User canceled the dialog
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing.");
        return;
    }

    QTextStream out(&file);

    // Get the table widget
    QTableWidget *table = ui->tableWidget; // Replace with your table widget's object name

    // Write the headers
    for (int col = 1; col < table->columnCount(); ++col) {
        out << table->horizontalHeaderItem(col)->text();
        if (col < table->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n";

    // Write the data
    for (int row = 0; row < table->rowCount(); ++row) {
        for (int col = 1; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item) {
                out << item->text();
            }
            if (col < table->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Success", "Data exported to CSV file successfully.");

}

HashFactoryUI::~HashFactoryUI()
{
    delete ui;
}
