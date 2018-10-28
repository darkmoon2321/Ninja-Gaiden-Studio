#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include "importsceneselectdialog.h"
#include "importbgdialog.h"
#include "editspritedialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Ninja Gaiden Studio");
    unsigned int i;
    for(i=0;i<0x100;i++){
        ppu_strings[i] = NULL;
        ppu_bases[i] = NULL;
        sprite_linears[i] = NULL;
        sprite_accels[i] = NULL;
        earthquakes[i] = NULL;
        sprite_shakes[i] = NULL;
        palettes[i] = NULL;
        bgs[i] = NULL;
        all_dialog[i] = NULL;
    }
    num_ppu_strings = 0;
    num_earthquakes = 0;
    num_ppu_bases = 0;
    num_sprite_linears = 0;
    num_sprite_accels = 0;
    num_sprite_shakes = 0;
    num_palettes = 0;
    num_bgs = 0;
    num_dialogs = 0;
    for(i=0;i<0x80;i++){
        sprite_animations[i] = NULL;
        sprites[i] = NULL;
    }
    num_sprite_animations = 0;
    num_sprites = 0;
    for(i=0;i<0x40;i++){
        scroll_params_list[i] = NULL;
    }
    num_params = 0;
    num_current_bgs = 0;
    num_current_sprites = 0;
    num_current_strings = 0;
    CHR_pages = NULL;
    num_chr_pages = 0;
    status_message.setText(QString("Ready"));
    ui->statusBar->addPermanentWidget(&status_message);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_save_button_clicked()
{
    ui->statusBar->showMessage(QString("Working..."));
    data = d_handler.exportAll();
    if(!data) return;
    QString filename = QFileDialog::getSaveFileName(this,QString(""),QString(""),tr("ROM file (*.nes)"));
    if(!filename.isNull()){
        QFile outfile(filename);
        if(!outfile.open(QIODevice::WriteOnly)){
            ui->statusBar->showMessage(QString("Failed to Save ROM Data"));
            QMessageBox::critical(this,tr("Error!"),tr("Could not create file."));
            return;
        }
        QDataStream out(&outfile);
        for(unsigned int i=0;i<file_size;i++){
            out<<data[i];
        }
    }
    ui->statusBar->showMessage(QString("ROM Saved!"),2000);
}

void MainWindow::on_importROM_button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString(""),QString(""),tr("ROM file (*.nes)"));
    if(!fileName.isEmpty()){
        ui->statusBar->showMessage(QString("Working..."));
        QFile infile(fileName);
        if(!infile.open(QIODevice::ReadOnly)){
            ui->statusBar->showMessage(QString("Could not open ROM!"),2000);
            return;
        }
        file_size = infile.size();
        QDataStream in(&infile);
        data = new uint8_t[file_size];
        in.readRawData((char *)data,file_size);
        d_handler.importAll(data);
        file_location = fileName.toStdString();
        unsigned int i,j;
        for(i=file_location.length() - 1; i!=0;i--){
            if(file_location[i] == '\\' || file_location[i] == '/') break;
        }
        if(file_location[i] == '\\' || file_location[i] == '/'){
            std::string temp_string = "";
            for(j=0;j<i;j++){
                temp_string += file_location[j];
            }
            file_location = temp_string;
        }
        status_message.setText(QString("ROM Ready"));
        ui->statusBar->showMessage("Imported Rom Successfully");
    }
}

void MainWindow::on_importScene_button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString(""),QString(""),tr("Text files (*.txt)"));
    if(!fileName.isEmpty()){
        ui->statusBar->showMessage(QString("Working"));
        QFile infile(fileName);
        if(!infile.open(QIODevice::ReadOnly)){
            ui->statusBar->showMessage(QString("Scene Import Failed"));
            QMessageBox::critical(this,tr("Error!"),tr("Could not open file"));
            return;
        }
        std::string scene_data = "";
        QTextStream in(&infile);
        scene_data = in.readAll().toStdString();
        int scene_selection;
        ImportSceneSelectDialog select_dialog(d_handler.getNumScenes(),this);
        scene_selection = select_dialog.exec();
        if(scene_selection){
            scene_selection = select_dialog.getSelection();
            d_handler.importScene(scene_selection,scene_data);
        }
        ui->statusBar->showMessage(QString("Scene Import Successful"),2000);
    }
}

void MainWindow::on_importBG_button_clicked()
{
    unsigned int i;
    ui->statusBar->showMessage(QString("Working..."));
    importBGdialog bg_dialog;
    bool result = bg_dialog.exec();
    if(result){
        for(i=0;i<num_bgs;i++) bgs[i]->generateImage();
        for(i=0;i<num_sprites;i++) sprites[i]->generateImage();
        for(i=0;i<num_ppu_strings;i++) ppu_strings[i]->generateImage();
    }
    ui->statusBar->clearMessage();
}

void MainWindow::on_importSprite_button_clicked()
{
    unsigned int i;
    ui->statusBar->showMessage(QString("Working..."));
    editSpriteDialog sprite_dialog;
    bool result = sprite_dialog.exec();
    if(result){
        for(i=0;i<num_bgs;i++) bgs[i]->generateImage();
        for(i=0;i<num_sprites;i++) sprites[i]->generateImage();
        for(i=0;i<num_ppu_strings;i++) ppu_strings[i]->generateImage();
    }
    ui->statusBar->clearMessage();

}

void MainWindow::on_export_button_clicked()
{
    ui->statusBar->showMessage(QString("Working..."));
    d_handler.writeToTXT(file_location);
    ui->statusBar->showMessage(QString("All Data Exported!"),2000);
}

void MainWindow::on_centralWidget_destroyed()
{
    delete [] data;
}
