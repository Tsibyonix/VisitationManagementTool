#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define APP_VERSION 0.1
//#define APP_STATUS beta

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

#include <QtNetwork>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QFileInfo>
#include <QFile>
#include <QtXml>
#include <QDesktopServices>

//Windows
#include <QProcessEnvironment>
#include <QStandardPaths>

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

    QString status = "beta";
    QString version = "0.2";

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

    //main table
    void setMainTable();
    QString mainQuery;

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
    QPushButton *manageFamily_SubmitButton;
    QPushButton *manageFamily_RevertButton;

    //management ManageVisitation visit
    QSqlRelationalTableModel *manageVisit_Model;
    void manageVisit_RefreshTable();
    QTableView *visitTable;
    QPushButton *manageVisit_AddButton;
    QPushButton *manageVisit_DelButton;
    QPushButton *manageVisit_SubmitButton;
    QPushButton *manageVisit_RevertButton;

    void downloadFile(QString link, bool progressDialog);
    void startRequest(QUrl url);
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    bool patchDownload;
    bool updates;
    QString patchReady;
    QString patchPath;

    QDomElement stable;
    QDomElement linkstable;
    QDomElement beta;
    QDomElement linkbeta;

    void checkForUpdate();
    void readXML();
    void Unzip(QString zipfilename , QString filename);

    qint64 speed = 0;
    qint64 last = 0;

    QString envAppData;
    QString patchDir;

private slots:
    //mangement
    void slot_ManageCells(bool val);
    void slot_ManageCells_AddButtonPress(bool val);
    void slot_ManageCells_DelButtonPress(bool val);
    void slot_ManageCell_SubmitButton(bool val);

    void slot_ManageFamily(bool val);
    void slot_ManageFamily_AddButton(bool val);
    void slot_ManageFamily_DelButton(bool val);
    void slot_ManageFamily_SubmitButton(bool val);

    void slot_ManageVisit(bool val);
    void slot_ManageVisit_AddButton(bool val);
    void slot_ManageVisit_DelButton(bool val);
    void slot_ManageVisit_SubmitButton(bool val);

    void slot_runQueryAction(bool val);

    void slot_ReadRead();
    void slot_Finished();
    void slot_Progress(qint64 bytesRead, qint64 totalBytes);

    void slot_DoPatch();

    void slot_SetComboBox();

public slots:
    void errorToStatusbar(QString err);

    //actions
    void aboutAction(bool val);
    void doUpdate();

    //
    void closeTab(int index);

protected:
    //00bool QSqlTableModel::insertRowIntoTable(const QSqlRecord &values);

signals:
    void linkReceived(QString link);
    void signal_PatchReady();
};

#endif // MAINWINDOW_H
