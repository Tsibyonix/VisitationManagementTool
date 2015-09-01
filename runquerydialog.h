#ifndef RUNQUERYDIALOG_H
#define RUNQUERYDIALOG_H

#include <QDialog>

namespace Ui {
class RunQueryDialog;
}

class RunQueryDialog : public QDialog
{
    Q_OBJECT

public:
    QString returnQuery();
    QString Query;
    explicit RunQueryDialog(QWidget *parent = 0);
    ~RunQueryDialog();

public slots:
    void okButtonClicked();
    void cancelButtonClicked();

private:
    Ui::RunQueryDialog *ui;
};

#endif // RUNQUERYDIALOG_H
