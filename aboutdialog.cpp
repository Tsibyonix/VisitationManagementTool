#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
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
