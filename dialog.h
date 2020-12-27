#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected slots:
    void checkSpelling();

private:
    void replaceAll(int nPos, QString sOld, QString sNew);
    Ui::Dialog *ui;
};

#endif // DIALOG_H
