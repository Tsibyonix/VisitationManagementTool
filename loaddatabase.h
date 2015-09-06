#ifndef LOADDATABASE_H
#define LOADDATABASE_H

#include <QWidget>

#include "mainwindow.h"

class LoadDatabase : public QWidget
{
    Q_OBJECT
public:
    explicit LoadDatabase(QString dbLocation, QWidget *parent = 0);
    void closeDatabase();
    bool returnDatabaseState();
    
signals:
    void finished();
    void error(QString err);

private:
    QSqlDatabase followupmain;
    bool DatabaseSate;
    QString dbLocationPath;
    
public slots:
    void loadDatabase();
    
};

#endif // LOADDATABASE_H
