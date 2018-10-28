#ifndef RESIZEBGDIALOG_H
#define RESIZEBGDIALOG_H

#include <QDialog>
#include <string>
#include "data_types.h"

namespace Ui {
class resizeBGdialog;
}

class resizeBGdialog : public QDialog
{
    Q_OBJECT

public:
    explicit resizeBGdialog(unsigned int,unsigned int,QWidget *parent = 0);
    ~resizeBGdialog();
    unsigned int getWidth();
    unsigned int getHeight();
private slots:

private:
    Ui::resizeBGdialog *ui;
};

#endif // RESIZEBGDIALOG_H
