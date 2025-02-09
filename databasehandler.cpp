#include "databasehandler.hpp"

void connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("../../db/hashfactory");
    db.setDatabaseName("db/hashfactory");

    if (!db.open()) {
        qDebug() << "Error: Could not open database" << db.lastError().text();;
        return;
    }
    qDebug() << "Database connected successfully!";
}


QString fetchType(int typeID){

    QSqlQuery query;
    query.prepare("SELECT type_name FROM file_type WHERE type_id = :type_id");
    query.bindValue(":type_id", typeID);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
        return "";
    }

    if (query.next()) {
        return query.value("type_name").toString();
    } else {
        return "";
    }
}

int fetchTypeID(QString typeName){

    QSqlQuery query;
    query.prepare("SELECT type_id FROM file_type WHERE type_name = :type_name");
    query.bindValue(":type_name", typeName);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value("type_id").toInt();
    } else {
        return 0;
    }
}

QString fetchAlgorithm(int id){

    QSqlQuery query;
    query.prepare("SELECT name FROM algorithm WHERE algorithm_id = :algorithm_id");
    query.bindValue(":algorithm_id", id);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
        return "";
    }

    if (query.next()) {
        return query.value("name").toString();
    } else {
        return "";
    }
}

int fetchAlgorithmId(QString algorithm){

    QSqlQuery query;
    query.prepare("SELECT algorithm_id FROM algorithm WHERE name = :name");
    query.bindValue(":name", algorithm);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value("algorithm_id").toInt();
    } else {
        return 0;
    }
}

int findHash(QString hash){

    QSqlQuery query;
    query.prepare("SELECT id FROM hash_list WHERE hash = :hash");
    query.bindValue(":hash", hash);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value("id").toInt();
    } else {
        return 0;
    }
}

QSqlQuery fetchRecordById(int id){

    QSqlQuery query;
    query.prepare("SELECT filename,type,algorithm,hash,creation_date FROM hash_list WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
    }

    return query;
}

QSqlQuery fetchRecordByHash(QString hash)
{
    QSqlQuery query;
    query.prepare("SELECT filename,type,algorithm,hash,creation_date FROM hash_list WHERE hash LIKE :hash");
    query.bindValue(":hash", "%" + hash + "%");

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
    }

    return query;
}

QSqlQuery fetchRecordByFileName(QString fileName)
{
    QSqlQuery query;
    query.prepare("SELECT filename,type,algorithm,hash,creation_date FROM hash_list WHERE fileName LIKE :fileName");
    query.bindValue(":fileName", "%" + fileName + "%");

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
    }

    return query;
}


QSqlQuery fetchRecordByAlgorithm(QString algorithm)
{
    QSqlQuery query;
    int algorithmId = fetchAlgorithmId(algorithm);
    query.prepare("SELECT filename,type,algorithm,hash,creation_date FROM hash_list WHERE algorithm LIKE :algorithmId");
    query.bindValue(":algorithmId", "%" + QString(algorithmId) + "%");

    if (!query.exec()) {
        qDebug() << "Error: Could not execute query.";
        qDebug() << query.lastError().text();
    }

    return query;
}

void addToTable(QString filename,QString hash,int algorithmId,int typeId,int time)
{
    QSqlQuery query;
    query.prepare("INSERT INTO hash_list (filename,type, algorithm,hash,creation_date) VALUES (:filename,:type, :algorithm,:hash,:creation_date)");

    query.bindValue(":filename", filename);
    query.bindValue(":type", typeId);
    query.bindValue(":algorithm", algorithmId);
    query.bindValue(":hash", hash);
    query.bindValue(":creation_date", time);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error: Could not insert record.";
        qDebug() << query.lastError().text();
    } else {
        qDebug() << "Record inserted successfully.";
    }
}
