#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setTabsClosable(true);

    qDebug() << "Loading saved settings";
    loadSettings();
    init_ConnectActions();
    init_LoadDatabase();
    //getCells();

    //variables
}

MainWindow::~MainWindow()
{
    LoadDatabase loadDatabase;
    saveSettings();
    qDebug() << "Saving settings from current session";
    loadDatabase.close();
    delete ui;
}

void MainWindow::saveSettings()
{
    QSettings settings("tSibyonix", "FollowUpTool");
    settings.beginGroup("MainWindow");
    settings.setValue("size", this->geometry());
    settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings("tSibyonix", "FollowUpTool");
    settings.beginGroup("MainWindow");
    QRect rectS = settings.value("size", "800*600").toRect();
    this->setGeometry(rectS);
    settings.endGroup();
}

void MainWindow::errorToStatusbar(QString err)
{
    showMessage(err, 6000);
}

void MainWindow::init_LoadDatabase()
{
    qDebug() << "Loading Database";
    LoadDatabase *loadDatabse;
    loadDB = new QThread;
    loadDatabse = new LoadDatabase;
    loadDatabse->moveToThread(loadDB);
    this->connect(loadDatabse, SIGNAL(error(QString)), this, SLOT(errorToStatusbar(QString)));
    this->connect(loadDB, SIGNAL(started()), loadDatabse, SLOT(loadDatabase()));
    this->connect(loadDatabse, SIGNAL(finished()), loadDB, SLOT(quit()));
    this->connect(loadDatabse, SIGNAL(finished()), loadDatabse, SLOT(deleteLater()));
    this->connect(loadDB, SIGNAL(finished()), loadDB, SLOT(deleteLater()));
    loadDB->start();

    if(loadDatabse->returnDatabaseState() == 0)
        showMessage("Database loaded", 6000);
    else
        showMessage("Database not loaded", 6000);
}

void MainWindow::init_ConnectActions()
{
    qDebug() << "Connecting Actions";
    this->connect(ui->action_About, SIGNAL(triggered(bool)), this, SLOT(aboutAction(bool)));
    this->connect(ui->actionRun_Query, SIGNAL(triggered(bool)), this, SLOT(slot_runQueryAction(bool)));
    this->connect(ui->actionMange_Cells, SIGNAL(triggered(bool)), this, SLOT(slot_ManageCells(bool)));
    this->connect(ui->actionManage_Families, SIGNAL(triggered(bool)), this, SLOT(slot_ManageFamily(bool)));
    this->connect(ui->actionManage_Visits, SIGNAL(triggered(bool)), this, SLOT(slot_ManageVisit(bool)));
    //
    this->connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void MainWindow::showMessage(QString msg, int time)
{
    if(time == NULL)
        ui->statusBar->showMessage(msg);
    else
        ui->statusBar->showMessage(msg, time);
}

void MainWindow::aboutAction(bool val)
{
    AboutDialog aboutDialog(this);
    aboutDialog.setModal(val);
    aboutDialog.exec();
}

void MainWindow::slot_runQueryAction(bool val)
{
    RunQueryDialog runQueryDialog(this);        //var
    runQueryDialog.setModal(val);
    QSqlQueryModel *model;                      //var
    QString Query;                              //var
    if(runQueryDialog.exec() == QDialog::Accepted)
    {
        Query = runQueryDialog.returnQuery();
        QSqlQuery qry;                          //var
        if(!qry.exec(Query))
           QMessageBox::warning(this, "Error", qry.lastError().text());
        else
        {
           showMessage("Query Execution Successfull", NULL);
           model = new QSqlQueryModel();
           model->setQuery(qry);

           //Query output
           QWidget *tabWidget;                  //var
           tabWidget = new QWidget();
           QTableView *tableView;               //var
           tableView = new QTableView(tabWidget);
           QHBoxLayout *hBoxLayout;             //var
           hBoxLayout = new QHBoxLayout(tabWidget);
           hBoxLayout->addWidget(tableView);
           tabWidget->setLayout(hBoxLayout);
           int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex() + 1, tabWidget, "Query output");
           ui->tabWidget->setCurrentIndex(index);
           tableView->setModel(model);
        }
    }
}

void MainWindow::getCells()
{
    cells.clear();
    QSqlQuery qry("select cell from cell_id");
    if(!qry.exec())
        showMessage("Cannot get cell list", 6000);
    else
    {
        while(qry.next())
        {
            cells.append(qry.value("cell").toString());
        }
    }
}

void MainWindow::closeTab(int index)
{
    if(index == 0)
        showMessage("Cannot close main tab.", 6000);
    else
        ui->tabWidget->removeTab(index);
}

void MainWindow::manageCell_RefreshTable()
{
    doSubmit = false;
    manageCell_Model = new QSqlTableModel(this);
    manageCell_Model->setTable("cell_id");
    manageCell_Model->setSort(1, Qt::AscendingOrder);
    manageCell_Model->select();
    manageCell_Model->setHeaderData(0, Qt::Horizontal, "Cells");
    manageCell_Model->setHeaderData(1, Qt::Horizontal, "ID");
    manageCell_Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    cellTable->setModel(manageCell_Model);
    cellTable->sortByColumn(1, Qt::AscendingOrder);
    cellTable->setSortingEnabled(true);
}

void MainWindow::slot_ManageCells(bool val)
{
    QWidget *tabWidget;                     //var
    tabWidget = new QWidget();
    QHBoxLayout *hBoxLayout;                //var
    hBoxLayout = new QHBoxLayout(this);
    manageCell_SubmitButton = new QPushButton("Submit", this);
    hBoxLayout->addWidget(manageCell_SubmitButton);
    manageCell_RevertButton = new QPushButton("Revert", this);
    hBoxLayout->addWidget(manageCell_RevertButton);
    hBoxLayout->addSpacerItem(new QSpacerItem(200, 15, QSizePolicy::Expanding));
    addButton = new QPushButton("add", this);
    addButton->setMaximumWidth(30);
    deleteButton = new QPushButton("del", this);
    deleteButton->setMaximumWidth(30);
    hBoxLayout->addWidget(addButton);
    hBoxLayout->addWidget(deleteButton);
    QVBoxLayout *vBoxLayout;                //var
    vBoxLayout = new QVBoxLayout(this);
    cellTable = new QTableView(this);
    vBoxLayout->addWidget(cellTable);
    vBoxLayout->addLayout(hBoxLayout);
    tabWidget->setLayout(vBoxLayout);
    int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex() + 1, tabWidget, "Cell Management");      //var
    ui->tabWidget->setCurrentIndex(index);

    manageCell_RefreshTable();
    cellTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //button signal and slots
    this->connect(addButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageCells_AddButtonPress(bool)));
    this->connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageCells_DelButtonPress(bool)));
    this->connect(manageCell_SubmitButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageCell_SubmitButton(bool)));
    this->connect(manageCell_RevertButton, SIGNAL(clicked(bool)), manageCell_Model, SLOT(revertAll()));
}

void MainWindow::slot_ManageCells_AddButtonPress(bool val)
{
    showMessage("Add pressed", 4000);
    QSqlRecord sqlRecord;
    QSqlField cell("cell", QVariant::String);
    QSqlField id("id", QVariant::Int);
    cell.setValue(QVariant(""));
    id.setValue(QVariant(""));
    sqlRecord.append(cell);
    sqlRecord.append(id);
    manageCell_Model->insertRecord(-1, sqlRecord);
}

void MainWindow::slot_ManageCells_DelButtonPress(bool val)
{
    QModelIndex index;
    int row = -1;
    index = cellTable->currentIndex();
    row = index.row();
    if(index.isValid())
    {
        showMessage("Deleting row number "+QString::number(row + 1)+"", 6000);
        QString temp = index.sibling(row, 1).data().toString();
        manageCell_Model->removeRow(index.row());
        manageCell_Model->submit();
    }
    else
        showMessage("Select the row you want to delete", 6000);
}

void MainWindow::slot_ManageCell_SubmitButton(bool val)
{
    showMessage("Submit Clicked", 6000);
    manageCell_Model->database().transaction();
    if(manageCell_Model->submitAll())
    {
        manageCell_Model->database().commit();
        qDebug() << "Changes are commited";
    }
    else
    {
        qDebug() << "Rollback";
        manageCell_Model->database().rollback();
        QMessageBox::warning(this, "Edit Error",
                             manageCell_Model->lastError().text());
    }
}

void MainWindow::manageFamily_RefreshTable()
{
    manageFamily_Model = new QSqlRelationalTableModel;
    manageFamily_Model->setTable("family_id");
    manageFamily_Model->setSort(1, Qt::AscendingOrder);
    manageFamily_Model->select();
    manageFamily_Model->setHeaderData(0, Qt::Horizontal, "Family Name");
    manageFamily_Model->setHeaderData(1, Qt::Horizontal, "Cell Number");
    manageFamily_Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    familyTable->setModel(manageFamily_Model);

    familyTable->sortByColumn(1, Qt::AscendingOrder);
    familyTable->setSortingEnabled(true);
    manageFamily_Model->setRelation(1, QSqlRelation("cell_id", "id", "id"));
    familyTable->setItemDelegate(new QSqlRelationalDelegate(familyTable));
}

void MainWindow::slot_ManageFamily(bool val)
{
    QWidget *tabWidget;                     //var
    tabWidget = new QWidget();
    QHBoxLayout *hBoxLayout;                //var
    hBoxLayout = new QHBoxLayout(this);
    manageFamily_SubmitButton = new QPushButton("Submit", this);
    hBoxLayout->addWidget(manageFamily_SubmitButton);
    manageFamily_RevertButton = new QPushButton("Revert", this);
    hBoxLayout->addWidget(manageFamily_RevertButton);
    hBoxLayout->addSpacerItem(new QSpacerItem(200, 15, QSizePolicy::Expanding));
    manageFamily_AddButton = new QPushButton("add", this);
    manageFamily_AddButton->setMaximumWidth(30);
    manageFamily_DelButton = new QPushButton("del", this);
    manageFamily_DelButton->setMaximumWidth(30);
    hBoxLayout->addWidget(manageFamily_AddButton);
    hBoxLayout->addWidget(manageFamily_DelButton);
    QVBoxLayout *vBoxLayout;                //var
    vBoxLayout = new QVBoxLayout(this);
    familyTable = new QTableView(this);
    vBoxLayout->addWidget(familyTable);
    vBoxLayout->addLayout(hBoxLayout);
    tabWidget->setLayout(vBoxLayout);
    int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex() + 1, tabWidget, "Family Management");      //var
    ui->tabWidget->setCurrentIndex(index);

    manageFamily_RefreshTable();
    familyTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    //signal and slots
    this->connect(manageFamily_AddButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageFamily_AddButton(bool)));
    this->connect(manageFamily_DelButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageFamily_DelButton(bool)));
    this->connect(manageFamily_SubmitButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageFamily_SubmitButton(bool)));
    this->connect(manageFamily_RevertButton, SIGNAL(clicked(bool)), manageFamily_Model, SLOT(revertAll()));
}

void MainWindow::slot_ManageFamily_AddButton(bool val)
{
    QSqlRecord sqlRecord;
    QSqlField family("family", QVariant::String);
    QSqlField id("id", QVariant::Int);
    family.setValue(QVariant(""));
    id.setValue(QVariant(""));
    sqlRecord.append(family);
    sqlRecord.append(id);
    manageFamily_Model->insertRecord(-1, sqlRecord);
}

void MainWindow::slot_ManageFamily_DelButton(bool val)
{
    QModelIndex index;
    int row = -1;
    index = familyTable->currentIndex();
    row = index.row();
    if(index.isValid())
    {
        showMessage("Deleting row number "+QString::number(row + 1)+"", 6000);
        QString temp = index.sibling(row, 1).data().toString();
        manageFamily_Model->removeRow(index.row());
        manageFamily_Model->submit();
    }
    else
        showMessage("Select the row you want to delete", 6000);
}

void MainWindow::slot_ManageFamily_SubmitButton(bool val)
{
    showMessage("Submit Clicked", 6000);
    manageFamily_Model->database().transaction();
    if(manageFamily_Model->submitAll())
    {
        manageFamily_Model->database().commit();
        qDebug() << "Changes are commited";
    }
    else
    {
        qDebug() << "Rollback";
        manageFamily_Model->database().rollback();
        QMessageBox::warning(this, "Edit Error",
                             manageFamily_Model->lastError().text());
    }
}

void MainWindow::manageVisit_RefreshTable()
{
    manageVisit_Model = new QSqlRelationalTableModel;
    manageVisit_Model->setTable("visit");
    manageVisit_Model->setSort(1, Qt::AscendingOrder);
    manageVisit_Model->select();
    manageVisit_Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    visitTable->setModel(manageVisit_Model);

    visitTable->sortByColumn(1, Qt::AscendingOrder);
    visitTable->setSortingEnabled(true);
    manageVisit_Model->setRelation(0, QSqlRelation("family_id", "family", "family"));
    visitTable->setItemDelegate(new QSqlRelationalDelegate(visitTable));
}

void MainWindow::slot_ManageVisit(bool val)
{
    QWidget *tabWidget;                     //var
    tabWidget = new QWidget();
    QHBoxLayout *hBoxLayout;                //var
    hBoxLayout = new QHBoxLayout(this);
    manageVisit_SubmitButton = new QPushButton("Submit", this);
    hBoxLayout->addWidget(manageVisit_SubmitButton);
    manageVisit_RevertButton = new QPushButton("Revert", this);
    hBoxLayout->addWidget(manageVisit_RevertButton);
    hBoxLayout->addSpacerItem(new QSpacerItem(200, 15, QSizePolicy::Expanding));
    manageVisit_AddButton = new QPushButton("add", this);
    manageVisit_AddButton->setMaximumWidth(30);
    manageVisit_DelButton = new QPushButton("del", this);
    manageVisit_DelButton->setMaximumWidth(30);
    hBoxLayout->addWidget(manageVisit_AddButton);
    hBoxLayout->addWidget(manageVisit_DelButton);
    QVBoxLayout *vBoxLayout;                //var
    vBoxLayout = new QVBoxLayout(this);
    visitTable = new QTableView(this);
    vBoxLayout->addWidget(visitTable);
    vBoxLayout->addLayout(hBoxLayout);
    tabWidget->setLayout(vBoxLayout);
    int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex() + 1, tabWidget, "Visit Management");      //var
    ui->tabWidget->setCurrentIndex(index);

    manageVisit_RefreshTable();
    visitTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    //signal and slots
    this->connect(manageVisit_AddButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageVisit_AddButton(bool)));
    this->connect(manageVisit_DelButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageVisit_DelButton(bool)));
    this->connect(manageVisit_SubmitButton, SIGNAL(clicked(bool)), this, SLOT(slot_ManageVisit_SubmitButton(bool)));
    this->connect(manageVisit_RevertButton, SIGNAL(clicked(bool)), manageVisit_Model, SLOT(revertAll()));
}

void MainWindow::slot_ManageVisit_AddButton(bool val)
{
    QSqlRecord sqlRecord;
    QSqlField visitTo("visit_to", QVariant::String);
    QSqlField visitBy("visit_by", QVariant::Int);
    QSqlField date("date", QVariant::String);
    QSqlField comments("comments", QVariant::String);
    visitTo.setValue(QVariant(""));
    visitBy.setValue(QVariant(""));
    date.setValue(QVariant(""));
    comments.setValue(QVariant(""));
    sqlRecord.append(visitTo);
    sqlRecord.append(visitBy);
    sqlRecord.append(date);
    sqlRecord.append(comments);
    manageVisit_Model->insertRecord(-1, sqlRecord);
}

void MainWindow::slot_ManageVisit_DelButton(bool val)
{
    QModelIndex index;
    int row = -1;
    index = visitTable->currentIndex();
    row = index.row();
    if(index.isValid())
    {
        showMessage("Deleting row number "+QString::number(row + 1)+"", 6000);
        QString temp = index.sibling(row, 1).data().toString();
        manageVisit_Model->removeRow(index.row());
        manageVisit_Model->submit();
    }
    else
        showMessage("Select the row you want to delete", 6000);
}

void MainWindow::slot_ManageVisit_SubmitButton(bool val)
{
    showMessage("Submit Clicked", 6000);
    manageVisit_Model->database().transaction();
    if(manageVisit_Model->submitAll())
    {
        manageVisit_Model->database().commit();
        qDebug() << "Changes are commited";
    }
    else
    {
        qDebug() << "Rollback";
        manageVisit_Model->database().rollback();
        QMessageBox::warning(this, "Edit Error",
                             manageVisit_Model->lastError().text());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
        case Qt::Key_C:
        {
            slot_ManageCells(true);
        }
    }
}
