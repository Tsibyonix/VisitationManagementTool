#include "runquerydialog.h"
#include "ui_runquerydialog.h"

RunQueryDialog::RunQueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunQueryDialog)
{
    ui->setupUi(this);
    this->connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(okButtonClicked()));
    this->connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelButtonClicked()));
    //create slots
}

RunQueryDialog::~RunQueryDialog()
{
    delete ui;
}

void RunQueryDialog::okButtonClicked()
{
    accept();
}

void RunQueryDialog::cancelButtonClicked()
{
    reject();
}

QString RunQueryDialog::returnQuery()
{
    Query = ui->queryEdit->text();
    return Query;
}
