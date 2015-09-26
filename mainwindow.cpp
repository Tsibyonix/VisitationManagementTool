#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QStringList pathList;
    pathList = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    envAppData = pathList.at(0);
    patchDir = envAppData+"/patch";

    ui->setupUi(this);
    init_LoadDatabase();
    setWindowTitle("Visitation Management Tool");

    qDebug() << "Loading saved settings";
    loadSettings();
    init_ConnectActions();
    //main
    //getCells();
//    ui->fromDateEdit->setMaximumWidth(62);
//    ui->fromDateEdit->setInputMask("0000-00-00");
//    ui->toDateEdit->setMaximumWidth(62);
//    ui->toDateEdit->setInputMask("0000-00-00");
    QStringList month;
    month << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    QStringList fortnite;
    fortnite << "This Month" << "Fortnite 1/ Fortnite 2";
    ui->fortnite->addItems(fortnite);
    ui->searchEdit->setMaximumWidth(80);
    //ui->cellComboBox->addItems(cells);

    ui->tabWidget->setTabsClosable(true);
    updates = false;
    patchDownload = false;
    checkForUpdate();
    //readXML();
    //variables
}

MainWindow::~MainWindow()
{
    LoadDatabase loadDatabase(envAppData);
    saveSettings();
    qDebug() << "Saving settings from current session";
    loadDatabase.close();
    //file->close();
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
    loadDatabse = new LoadDatabase(envAppData, this);
    loadDatabse->moveToThread(loadDB);
    this->connect(loadDatabse, SIGNAL(error(QString)), this, SLOT(errorToStatusbar(QString)));
    this->connect(loadDB, SIGNAL(started()), loadDatabse, SLOT(loadDatabase()));
    this->connect(loadDatabse, SIGNAL(finished()), loadDB, SLOT(quit()));
    this->connect(loadDatabse, SIGNAL(finished()), this, SLOT(slot_SetCellComboBox()));
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
    this->connect(ui->actionCheck_for_Updates, SIGNAL(triggered(bool)), this, SLOT(doUpdate()));
    this->connect(ui->actionSetup_Views, SIGNAL(triggered(bool)), this, SLOT(setupViews()));
    //
    this->connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    this->connect(ui->familyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_FamilyComboBoxSelectionChanged(int)));
    this->connect(ui->cellComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_CellComboBoxSelectionChanged(int)));
    this->connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_TabIndexChanged(int)));
    this->connect(this, SIGNAL(signal_PatchReady()), this, SLOT(slot_DoPatch()));
}

void MainWindow::showMessage(QString msg, int time)
{
    if(time == NULL)
        ui->statusBar->showMessage(msg);
    else
        ui->statusBar->showMessage(msg, time);
}

void MainWindow::Unzip(QString zipfilename , QString filename)
{
    QFile infile(zipfilename);
    QFile outfile(filename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qUncompress(uncompressedData);
    outfile.write(compressedData);
    patchPath = QFileInfo(outfile.fileName()).absoluteFilePath();
    patchReady = outfile.fileName();
    infile.close();
    outfile.close();
}

void MainWindow::slot_DoPatch()
{
    if(patchReady.isEmpty())
        return;
    else {
        int patch = QMessageBox::warning(this,
                                         "Continue patching",
                                         "Pressing yes will close the application, unsubmitted data will be lost. Do you wish to continue?",
                                         QMessageBox::Yes, QMessageBox::No);
        if(patch == QMessageBox::Yes) {
            showMessage(patchPath, 6000);
            Unzip(patchPath, envAppData+"/patches/patch.exe");
            if(QDesktopServices::openUrl(QUrl("file:///"+patchPath, QUrl::TolerantMode)))
                MainWindow::close();
            else{
                showMessage("Cannot open file", 6000);
            }
        }
        else {
            showMessage("You can alway came back and update later", 6000);
        }
    }
}

void MainWindow::checkForUpdate()
{
    downloadFile("http://tsibyonixo.scrapmyemail.tntrg.com/visitationmanagementtool/version.xml", false);
}

void MainWindow::downloadFile(QString link, bool progressDialog)
{
    if(progressDialog)
        patchDownload = true;
    url.setUrl(link);

    QString fileName = QFileInfo(url.path()).fileName();

    if (fileName.isEmpty())
        fileName = "version.xml";

    if (QFile::exists(fileName)) {
//        if(progressDialog) {
//            patchReady = QFileInfo(url.path()).fileName();
//            emit signal_PatchReady();
//            return;
//        } else {
            QFile::remove(fileName);
//        }
    }

    //fileName = QStandardPaths::displayName(QStandardPaths::AppDataLocation);
    file = new QFile(envAppData+"/patches/"+fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        file->close();
        delete file;
    }

    QFileInfo fileInfo(file->fileName());

    if(progressDialog)
        patchPath = fileInfo.absoluteFilePath();

    startRequest(url);
}

void MainWindow::startRequest(QUrl url)
{
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(slot_Finished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(slot_ReadRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slot_Progress(qint64,qint64)));
}

void MainWindow::slot_ReadRead()
{
    if (file)
        file->write(reply->readAll());
}

void MainWindow::slot_Finished()
{
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        showMessage("Server returned 404, file not found", 6000);
        file->remove();
        file->close();
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        file->close();
    } else {
        if(patchDownload) {
            reply->deleteLater();
            file->close();
            patchReady = QFileInfo(url.path()).fileName();
            emit signal_PatchReady();
        }
        else {
            reply->deleteLater();
            file->close();
            qDebug() << "Reading XML file";
            readXML();
        }
    }
}

void MainWindow::slot_Progress(qint64 bytesRead, qint64 totalBytes)
{
    float time;
    QString format = "minutes";
    float kbRamaining;
    qint64 kbRead = bytesRead/(1024*1024);
    speed = (bytesRead/1024) - last;
    last = bytesRead/1024;
    kbRamaining = (totalBytes - bytesRead) / 1024;
    time = (kbRamaining/speed)/60;
    if(time > 60) {
        format = "hours";
        time = time/60;
    }
    //speed
    qint64 kbtotal = totalBytes/(1024*1024);
    if(kbRead != kbtotal)
        showMessage(tr("Downloading file %1, current progress: %2 MiB/ %3 MiB, @%4 KiB/iteration, Etimated time remaining: %5 %6")
                    .arg(QFileInfo(url.path()).fileName())
                    .arg(QString::number(kbRead))
                    .arg(QString::number(kbtotal))
                    .arg(QString::number(speed))
                    .arg(QString::number(time, 'f', 1))
                    .arg(format), NULL);
    else {
       showMessage("File Downloaded", 6000);
    }
}

void MainWindow::readXML()
{
    qDebug() << "Parsing document";
    QDomDocument document;
    QFile xml(envAppData+"/patches/version.xml");
    if(!xml.open(QIODevice::ReadOnly))
        qDebug() << "Cannot open file";
    if(!document.setContent(&xml)) {
        qDebug() << "Unable to read document file";
        xml.close();
        return;
    }
    xml.close();

    QDomElement root;
    root = document.firstChildElement();

    stable = root.firstChildElement("stable");
    linkstable = root.firstChildElement("stablelink");
    beta = root.firstChildElement("beta");
    linkbeta = root.firstChildElement("betalink");

    float current;
    current = version.toFloat();

    if(status.contains("stable")) {
        if(stable.text().toFloat() > current) {
        showMessage("New updates available", 6000);
        updates = true;
        } else {
        showMessage("No new updates available", 6000);
        }
    } else {
        if(beta.text().toFloat() > current) {
        showMessage("New updates available", 6000);
        updates = true;
        } else {
        showMessage("No new updates available", 6000);
        }
    }
}

void MainWindow::doUpdate()
{
    if(updates) {
        if(status.contains("stable"))
            downloadFile(linkstable.text(), true);
        else
            downloadFile(linkbeta.text(), true);
    } else {
        QMessageBox::warning(this, "Updates", "No updates were found", QMessageBox::Ok);
    }
}

void MainWindow::aboutAction(bool val)
{
    AboutDialog aboutDialog(version, status, this);
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
    ui->cellComboBox->clear();
    cells << "ALL";
    QSqlQuery qry("select cell from cell_id");
    if(!qry.exec())
        showMessage("Cannot get cell list, "+qry.lastError().text(), 6000);
    else
    {
        while(qry.next())
        {
            cells.append(qry.value("cell").toString());
        }
    }
}

void MainWindow::slot_SetCellComboBox()
{
    getCells();
    ui->cellComboBox->addItems(cells);
    setupCellView();
}

void MainWindow::setupCellView()
{
    int counter =1;
    for(counter =1; counter <= cells.length() -1; counter++)
    {
        QString cell = cells.at(counter);
        cell.replace(" ", "_");
        cell.append("_visit");
        showMessage(cell, 6000);
        QSqlQuery qry;
        qry.prepare("create view '"+cell+"' as select visit.visit_to, visit.visit_by, visit.date, visit.comments from family_id left outer join visit on family_id.family = visit.visit_to where family_id.id in (select id from cell_id where cell = '"+cells.at(counter)+"')");
        qry.exec();
    }
}

void MainWindow::setupViews()
{
    setupCellView();
}

void MainWindow::closeTab(int index)
{
    if(index == 0)
        showMessage("Cannot close main tab.", 6000);
    else
        ui->tabWidget->removeTab(index);
}

void MainWindow::slot_CellComboBoxSelectionChanged(int index)
{
    QString cell = ui->cellComboBox->currentText();
    setMainTable(ui->familyComboBox->currentText(), cell);
}

void MainWindow::slot_FamilyComboBoxSelectionChanged(int index)
{
    QString family = ui->familyComboBox->currentText();
    setMainTable(family, ui->cellComboBox->currentText());
}

void MainWindow::setMainTable(QString family, QString cell)
{
    mainQuery.clear();
    if(QString::compare(cell, "ALL") == 0) {
        mainQuery = "select family_id.family, visit.visit_by, visit.date, visit.comments from family_id ";
        if(family.compare("All Families")) {
            mainQuery.append(", visit on family_id.family = visit.visit_to");
        }
        else
            mainQuery.append("left outer join visit on family_id.family = visit.visit_to");
    }
    else {
        mainQuery = "select family_id.family, visit.visit_by, visit.date, visit.comments from family_id ";
        if(family.compare("All Families")) {
            mainQuery.append(", visit on family_id.family = visit.visit_to where family_id.id in ( select id from cell_id where cell = '"+ cell +"');");
        }
        else
            mainQuery.append("left outer join visit on family_id.family = visit.visit_to where family_id.id in ( select id from cell_id where cell = '"+ cell +"');");
    }

    QSqlQueryModel *model;
    model = new QSqlQueryModel;
    QSqlQuery query;
    //query.prepare(mainQuery);
    query.prepare("select"
                  " visit_to,"
                  " replace(rtrim(group_concat((case when date(date(date, '-15 days'), 'start of month') <> date(date, 'start of month') then visit_by else '' end), (case when date(date(date, '-15 days'), 'start of month') <> date(date, 'start of month') then ' ' else '' end))), ' ', ',') fortnite1,"
                  " replace(rtrim(group_concat((case when date(date(date, '-15 days'), 'start of month') = date(date, 'start of month') then visit_by else '' end), (case when date(date(date, '-15 days'), 'start of month') = date(date, 'start of month') then ' ' else '' end))), ' ', ',') fortnite2"
                  " from cell_2_visit"
                  " group by visit_to, date(date, 'start of month')");

    if(!query.exec())
        showMessage(query.lastError().text(), 6000);

    else
    {
        model->setQuery(query);
        ui->mainTableView->setModel(model);
    }
}

void MainWindow::slot_TabIndexChanged(int index)
{
    ui->show->show();
    ui->familyComboBox->show();
    ui->inThis->show();
    ui->cellComboBox->show();
    ui->in->show();
    ui->fortnite->show();

    if(index != 0) {
        ui->show->hide();
        ui->familyComboBox->hide();
        ui->inThis->hide();
        ui->cellComboBox->hide();
        ui->in->hide();
        ui->fortnite->hide();
    }
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
        slot_SetCellComboBox();
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
