#include "loaddatabase.h"

LoadDatabase::LoadDatabase(QString dbLocation, QWidget *parent) :
    QWidget(parent)
{
    dbLocationPath = dbLocation;
}

void LoadDatabase::loadDatabase()
{
    followupmain = QSqlDatabase::addDatabase("QSQLITE");
    followupmain.setDatabaseName(dbLocationPath+"/followupmain.db3");
    followupmain.open();
    if(followupmain.isOpen()) {
        DatabaseSate = 0;
        emit finished();
    }
    else {
        DatabaseSate = 1;
        emit error("Unable to load Database!");
    }
}

void LoadDatabase::closeDatabase()
{
    followupmain.close();
}

bool LoadDatabase::returnDatabaseState()
{
    return DatabaseSate;
}
