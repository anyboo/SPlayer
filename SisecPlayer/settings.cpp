#include "settings.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>


Settings::Settings()
{
}


void Settings::createTable()
{
    QSqlQuery query;
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS  "SETTIN_TABLE_NAME" (key VARCHAR PRIMARY KEY,"
                         "value VARCHAR)");
    if (!ok)
    {
        qDebug() << "create table "SETTIN_TABLE_NAME" failed:" << query.lastQuery() << ":" << query.lastError().text();;
    }
}

bool Settings::setItem(const QString &key, const QString &value)
{
    qDebug()<<__FUNCTION__<<" key:"<<key<<"  "<<"value:"<<value;
    QSqlQuery query;
    query.prepare("SELECT value FROM "SETTIN_TABLE_NAME" WHERE key=:key");
    query.bindValue(":key", key);
    if (query.exec())
    {
        if (query.next())
        {
            query.prepare("UPDATE "SETTIN_TABLE_NAME" SET value=:value WHERE key=:key");
        }
        else
        {
            query.prepare("INSERT INTO "SETTIN_TABLE_NAME" (key, value) VALUES (:key, :value)");
        }
        query.bindValue(":key", key);
        query.bindValue(":value", value);
        if (!query.exec())
        {
            qDebug() << "Settings::setItem failed:" << ":" << query.lastQuery() << ":" << query.lastError().text();
            return false;

        }
        else
        {
            return true;
        }
    }
    else
    {
        qDebug() << "Settings::setItem failed select:" << ":" << query.lastQuery() << ":" << query.lastError().text();
        return false;

    }

}

QString Settings::getItem(const QString &key)
{
    QSqlQuery query;
    query.prepare("SELECT value FROM "SETTIN_TABLE_NAME" WHERE key=:key");
    query.bindValue(":key", key);
    query.exec();
    if (query.next())
    {
        return query.value("value").toString();
    }

    return "";
}

