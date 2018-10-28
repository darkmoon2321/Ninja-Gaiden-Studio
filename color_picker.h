#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include "data_types.h"

class color_picker : public QDialog
{
    Q_OBJECT

public:
    explicit color_picker(QWidget *parent = 0);
    void setDefaultColor(uint8_t);
protected:
    void closeEvent(QCloseEvent * event);
private slots:
    void buttonPushed();
   // void close();
private:
    QPushButton buttons[0x40];
    uint8_t selection;
};

#endif // COLOR_PICKER_H
