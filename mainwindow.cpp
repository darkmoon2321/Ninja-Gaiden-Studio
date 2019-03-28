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
    updateTextTable(":/Resources/default.tbl");
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
    command_names.clear();
    scene_model = new QStringListModel;
    scene_model->setStringList(command_names);
    ui->listView->setModel(scene_model);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->listView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(selectionHandler(QItemSelection,QItemSelection)));
    status_message.setText(QString("Ready"));
    ui->statusBar->addPermanentWidget(&status_message);
    ui->comboBox_2->addItem("BAR_OR_RESET");
    ui->comboBox_2->addItem("BGCOLOR_MASK");
    ui->comboBox_2->addItem("BG_GFXPAGE");
    ui->comboBox_2->addItem("BGSET/CLEAR");
    ui->comboBox_2->addItem("BLACKOUT");
    ui->comboBox_2->addItem("DIALOG_DELAY");
    ui->comboBox_2->addItem("EARTHQUAKE");
    ui->comboBox_2->addItem("END_SCENE_DURING_POPCORN_VIEWER");
    ui->comboBox_2->addItem("FADE");
    ui->comboBox_2->addItem("FLASH");
    ui->comboBox_2->addItem("SPRITE1_SPEED");
    ui->comboBox_2->addItem("MIRRORING");
    ui->comboBox_2->addItem("MOVESPRITE");
    ui->comboBox_2->addItem("MUSIC");
    ui->comboBox_2->addItem("PALETTE_SET");
    ui->comboBox_2->addItem("PPU_TRANSMIT");
    ui->comboBox_2->addItem("SCROLL_SETUP");
    ui->comboBox_2->addItem("SCROLL_SPEED");
    ui->comboBox_2->addItem("SETUP_BOSS_RUSH_TEXT");
    ui->comboBox_2->addItem("SETUP_HARD_MODE");
    ui->comboBox_2->addItem("SETUP_RANKING_TEXT");
    ui->comboBox_2->addItem("SPRITESET/CLEAR");
    ui->comboBox_2->addItem("SYNC");
    ui->comboBox_2->addItem("TEXT");
    ui->comboBox_2->addItem("UNKNOWN");
    ui->comboBox_2->addItem("WAIT/END_SCENE");
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
        ui->comboBox->clear();
        for(i=0;i<d_handler.num_scenes;i++) ui->comboBox->addItem(QString(convertByteToHexString(i).c_str()));
        ui->comboBox->addItem(QString("New"));
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

void MainWindow::updateTextTable(QString file_path){
    QFile infile(file_path);
    QTextStream in(&infile);
    if(!infile.open(QIODevice::ReadOnly)) {
        return;
    }
    uint8_t NG_byte;
    QChar conversion;
    std::string macro;
    uint32_t offset;
    unsigned int i;
    for(i=0;i<0x100;i++) text_table[i] = 0;
    while(!in.atEnd()){
        QString line = in.readLine();
        std::string stdline = line.toStdString();
        offset = 0;
        NG_byte = getIntFromTXT(stdline,offset);
        if(offset >= line.length()) continue;
        if(line[offset] == ','){
            macro = "";
            macro += NG_byte;
            while(line[offset] == ','){
                NG_byte = getIntFromTXT(stdline,offset);
                if(NG_byte != 0xff) macro += NG_byte;
            }
            if(line[offset] == '='){
                offset++;
                if(offset>=line.length()) continue;
                unicode_conversions.push_back(macro);
                unicode_list.push_back(line.at(offset));
            }
        }
        else if(line[offset] == '='){
            offset++;
            if(offset>=line.length()) continue;
            conversion = line.at(offset);
            if(conversion == ' '){
                text_table[NG_byte] = conversion;
            }
            text_table[NG_byte] = conversion;
        }
    }
    infile.close();
}

void MainWindow::on_actionImport_ROM_triggered()
{
    on_importROM_button_clicked();
}


void MainWindow::on_actionImport_Scene_triggered()
{
    on_importScene_button_clicked();
}

void MainWindow::on_actionBG_Editor_triggered()
{
    on_importBG_button_clicked();
}


void MainWindow::on_actionSprite_Editor_triggered()
{
    on_importSprite_button_clicked();
}


void MainWindow::on_actionExport_All_Data_triggered()
{
    on_export_button_clicked();
}


void MainWindow::on_actionSave_Changes_triggered()
{
    on_save_button_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(ui->comboBox->count() <= 1) return;
    command_names.clear();
    int i;
    for(i=0;i<d_handler.scenes[index].size();i++){
        command_names.push_back(d_handler.scenes[index].at(i)->getName());
    }
    scene_model->setStringList(command_names);
}

void MainWindow::selectionHandler(QItemSelection current_selection, QItemSelection previous_selection){
    QModelIndex index = current_selection.indexes().at(0);
    uint8_t command = d_handler.scenes[ui->comboBox->currentIndex()].at(index.row())->getCommand();
    //ui->comboBox_2->setCurrentIndex();

}
