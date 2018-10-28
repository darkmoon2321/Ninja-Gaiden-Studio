#ifndef IMPORTSCENESELECTDIALOG_H
#define IMPORTSCENESELECTDIALOG_H

#include <QDialog>
#include "data_types.h"

namespace Ui {
class ImportSceneSelectDialog;
}

class ImportSceneSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportSceneSelectDialog(uint8_t, QWidget *parent = 0);
    ~ImportSceneSelectDialog();
    uint8_t getSelection();
private:
    Ui::ImportSceneSelectDialog *ui;
};

#endif // IMPORTSCENESELECTDIALOG_H
