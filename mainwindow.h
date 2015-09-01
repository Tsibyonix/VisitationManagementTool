#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QDebug>
#include <QThread>

#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QProgressDialog>
#include <QMessageBox>

#include "loaddatabase.h"
#include "aboutdialog.h"
#include "runquerydialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent* keyEvent);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    //init
    void init_ConnectActions();
    void init_LoadDatabase();

    //dialogs
    //QProgressDialog loadingDB;

    //threads
    QThread *loadDB;

    //settings
    void saveSettings();
    void loadSettings();

    //debug and status bar outputs
    void showMessage(QString msg, int time);

    //data function
    void getCells();
    QStringList cells;

    //management ManageCell cell_id
    //QSqlTableModel manageCell_SetupTable();
    QSqlTableModel *manageCell_Model;
    QSqlRecord manageCell_record;
    void manageCell_RefreshTable();
    bool doSubmit;
    QTableView *cellTable;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *manageCell_SubmitButton;
    QPushButton *manageCell_RevertButton;

    //management ManageFamilies family_id
    QSqlRelationalTableModel *manageFamily_Model;
    void manageFamily_RefreshTable();
    QTableView *familyTable;
    QPushButton *manageFamily_AddButton;
    QPushButton *manageFamily_DelButton;

    //management ManageVisitation visit
    QSqlRelationalTableModel *manageVisit_Model;
    void manageVisit_RefreshTable();
    QTableView *visitTable;
    QPushButton *manageVisit_AddButton;
    QPushButton *manageVisit_DelButton;

private slots:
    //mangement
    void slot_ManageCells(bool val);
    void slot_ManageCells_AddButtonPress(bool val);
    void slot_ManageCells_DelButtonPress(bool val);
    void slot_ManageCells_UpdateTable(QModelIndex,QModelIndex,QVector<int>);
    void slot_ManageCell_SubmitButton(bool val);

    void slot_ManageFamily(bool val);
    void slot_ManageFamily_AddButton(bool val);
    void slot_ManageFamily_DelButton(bool val);

    void slot_ManageVisits(bool val);
    void slot_ManageVisits_AddButton(bool val);
    void slot_ManageVisit_DelButton(bool val);

    void slot_runQueryAction(bool val);

public slots:
    void errorToStatusbar(QString err);

    //actions
    void aboutAction(bool val);

    //
    void closeTab(int index);

protected:
    //00bool QSqlTableModel::insertRowIntoTable(const QSqlRecord &values);
};

#endif // MAINWINDOW_H
