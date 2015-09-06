#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QString version, QString type, QWidget *parent = 0);
    ~AboutDialog();

public slots:
    void okButton();
private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
