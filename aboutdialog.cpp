#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QString version, QString type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->version->setText("Version "+version+" "+type);
    this->connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(okButton()));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::okButton()
{
    accept();
}
