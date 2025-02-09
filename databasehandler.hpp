#ifndef DATABASEHANDLER_HPP
#define DATABASEHANDLER_HPP

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

void connectToDatabase();

QString fetchType(int typeID);
int fetchTypeID(QString typeName);

QString fetchAlgorithm(int id);
int fetchAlgorithmId(QString algorithm);

int findHash(QString hash);

QSqlQuery fetchRecordById(int id);
QSqlQuery fetchRecordByHash(QString hash);
QSqlQuery fetchRecordByFileName(QString fileName);
QSqlQuery fetchRecordByAlgorithm(QString algorithm);

void addToTable(QString filename,QString hash,int algorithmId,int typeId,int time);
bool insertIntoDatabase(const QStringList &data);

#endif // DATABASEHANDLER_HPP
