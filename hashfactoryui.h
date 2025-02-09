#ifndef HASHFACTORYUI_H
#define HASHFACTORYUI_H

#include <QMainWindow>
#include <QMap>
#include <QCryptographicHash>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui {
class HashFactoryUI;
}
QT_END_NAMESPACE

class HashFactoryUI : public QMainWindow
{
    Q_OBJECT

public:
    HashFactoryUI(QWidget *parent = nullptr);
    ~HashFactoryUI();

private:
    Ui::HashFactoryUI *ui;

    QMap<QString,QCryptographicHash::Algorithm> encryptedAlgorithm;

    void loadData();
    void showOnTable(QSqlQuery &query);
private slots:
    void onImportClicked();
    void onExportClicked();
    void onCalculateClicked();
    void onBrowseClicked();
    void onLoadData();
    void onSearchClicked();
    void onAddClicked();
    void onActionExit();
    void onActionAbout();
    void onActionHelp();
};
#endif // HASHFACTORYUI_H
