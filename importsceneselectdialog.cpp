#include "importsceneselectdialog.h"
#include "ui_importsceneselectdialog.h"

ImportSceneSelectDialog::ImportSceneSelectDialog(uint8_t num_scenes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportSceneSelectDialog)
{
    ui->setupUi(this);
    for(unsigned int i=0;i<num_scenes;i++){
        ui->scene_selector->addItem(QString(convertByteToHexString(i).c_str()));
    }
    ui->scene_selector->addItem(QString("New"));
}

ImportSceneSelectDialog::~ImportSceneSelectDialog()
{
    delete ui;
}

uint8_t ImportSceneSelectDialog::getSelection(){
    return ui->scene_selector->currentIndex();
}
