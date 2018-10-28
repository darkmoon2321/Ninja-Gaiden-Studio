#include "resizebgdialog.h"
#include "ui_resizebgdialog.h"

resizeBGdialog::resizeBGdialog(unsigned int metatile_width, unsigned int metatile_height, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resizeBGdialog)
{
    ui->setupUi(this);
    unsigned int i;
    for(i=4;i<=0x20;i+=4){
        ui->comboBox->addItem(QString(convertByteToHexString(i).c_str()));
    }
    for(i=2;i<=0x1E;i+=2){
        ui->comboBox_2->addItem(QString(convertByteToHexString(i).c_str()));
    }
    ui->comboBox->setCurrentIndex(metatile_width-1);
    ui->comboBox_2->setCurrentIndex(metatile_height-1);
}

resizeBGdialog::~resizeBGdialog()
{
    delete ui;
}

unsigned int resizeBGdialog::getWidth(){
    return ui->comboBox->currentIndex() + 1;
}

unsigned int resizeBGdialog::getHeight(){
    return ui->comboBox_2->currentIndex() + 1;
}
