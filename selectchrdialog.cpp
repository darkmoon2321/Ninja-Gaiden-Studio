#include "selectchrdialog.h"
#include "ui_selectchrdialog.h"

selectCHRDialog::selectCHRDialog(QWidget *parent,uint8_t page_num) :
    QDialog(parent),
    ui(new Ui::selectCHRDialog)
{
    ui->setupUi(this);
    unsigned int i,j;

    overwriteable_pixmap = new QGraphicsPixmapItem;
    old_pages = new SHARED_FLAGS[num_chr_pages];
    for(i=0;i<0x100;i++){
        chr_array[i] = new chrItem;
        page_scene.addItem(chr_array[i]);
        chr_array[i]->setOffset((i&0xf)*9,(i>>4)*9);
        chr_array[i]->setFlag(QGraphicsPixmapItem::ItemIsSelectable);
        chr_array[i]->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
        chr_array[i]->setID(i);
        connect(chr_array[i],SIGNAL(chr_pressed()),this,SLOT(tile_changed()));
    }
    selected_tile = 0;
    page_scene.addItem(overwriteable_pixmap);
    ui->graphicsView->setScene(&page_scene);
    for(i=0;i<num_chr_pages;i++){
        ui->comboBox->addItem(QString(convertByteToHexString(i).c_str()));
        for(j=0;j<0x100;j++) old_pages[i].shared[j] = CHR_pages[i].t[j].shared;
    }
    ui->comboBox->setCurrentIndex(page_num);
    //on_comboBox_currentIndexChanged(page_num);
}

selectCHRDialog::~selectCHRDialog()
{
    delete ui;
}

void selectCHRDialog::drawCHR(){
    unsigned int i,j,k;
    uint8_t color_index;
    QColor pal[4];
    pal[0] = QColor(0,0,0);
    pal[1] = QColor(80,80,80);
    pal[2] = QColor(160,160,160);
    pal[3] = QColor(240,240,240);
    QRgb * edit_line;
    for(i=0;i<0x100;i++){
        tiles[i] = QImage(8,8,QImage::Format_ARGB32);
        for(j=0;j<8;j++){
            edit_line = (QRgb*) tiles[i].scanLine(j);
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= ((page->t[i].t[j]) >> (7-k))&0x01;
                color_index |= (((page->t[i].t[j+8]) >> (7-k))&0x01)<<1;
                edit_line[k] = pal[color_index].rgb();
            }
        }
        chr_array[i]->setPixmap(QPixmap::fromImage(tiles[i]));
    }
}

void selectCHRDialog::tile_changed(){
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    ui->label_4->setPixmap(QPixmap::fromImage(tiles[selected_tile]));
    ui->checkBox_2->setChecked(!page->t[selected_tile].shared);
}

void selectCHRDialog::on_checkBox_2_clicked()
{
    page->t[selected_tile].shared = !ui->checkBox_2->isChecked();
    updateCHRMask();
}


void selectCHRDialog::on_comboBox_currentIndexChanged(int index)
{
    int i;

    page = &CHR_pages[index];
    drawCHR();
    updateCHRMask();
    ui->label_4->setPixmap(QPixmap::fromImage(tiles[selected_tile]));
    ui->label_4->setScaledContents(true);
    ui->checkBox_2->setChecked(!page->t[selected_tile].shared);
    bg_list.clear();
    sprite_list.clear();
    dynamic_list.clear();
    ui->comboBox_2->clear();
    for(i=0;i<num_bgs;i++){
        if(bgs[i]->gfx_page == index){
            bg_list.push_back(bgs[i]);
            overwrite_list.push_back(false);
            ui->comboBox_2->addItem(QString((std::string("BG") + convertByteToHexString(bgs[i]->id)).c_str()));
        }
    }
    for(i=0;i<num_sprites;i++){
        if(sprites[i]->gfx_page == index || sprites[i]->bg_gfx_page == index){
            overwrite_list.push_back(false);
            sprite_list.push_back(sprites[i]);
            ui->comboBox_2->addItem(QString((std::string("Sprites") + convertByteToHexString(sprites[i]->id)).c_str()));
        }
    }
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->gfx_page == index){
            overwrite_list.push_back(false);
            dynamic_list.push_back(ppu_strings[i]);
            ui->comboBox_2->addItem(QString((std::string("Dynamic") + convertByteToHexString(ppu_strings[i]->id)).c_str()));
        }
    }
    ui->checkBox->setChecked(false);
    selected_page = index;
}


void selectCHRDialog::on_checkBox_clicked()
{
    int unsigned i;
    uint8_t selected_image = ui->comboBox_2->currentIndex();
    bool shared = !ui->checkBox->isChecked();
    if(selected_image < bg_list.size()){
        bg_arrangement * temp_bg = bg_list.at(selected_image);
        for(i=0;i<temp_bg->tiles.size();i++){
            if(temp_bg->tiles.at(i)->id < 0xF8){
                temp_bg->tiles.at(i)->shared = shared;
            }
        }
        ui->checkBox_2->setChecked(!page->t[selected_tile].shared);
        updateCHRMask();
        return;
    }
    selected_image -= bg_list.size();
    if(selected_image < sprite_list.size()){
        sprite * temp_sprite = sprite_list.at(selected_image);
        for(i=0;i<temp_sprite->tiles.size();i++){
            if(temp_sprite->tiles.at(i)->id < 0xF8){
                temp_sprite->tiles.at(i)->shared = shared;
            }
        }
        ui->checkBox_2->setChecked(!page->t[selected_tile].shared);
        updateCHRMask();
        return;
    }
    selected_image -= sprite_list.size();
    ppu_string * temp_dynamic = dynamic_list.at(selected_image);
    for(i=0;i<temp_dynamic->tiles.size();i++){
        if(temp_dynamic->tiles.at(i)->id < 0xF8){
            temp_dynamic->tiles.at(i)->shared = shared;
        }
    }
    ui->checkBox_2->setChecked(!page->t[selected_tile].shared);
    updateCHRMask();
}

void selectCHRDialog::updateCHRMask(){
    QRgb * edit_line;
    uint8_t tile_row;
    uint8_t base_column;
    QColor fill_color;
    unsigned int i,j,k;

    overwriteable_image = QImage(144,144,QImage::Format_ARGB32);
    for(i=0;i<0x100;i++){
        if(page->t[i].shared){
            fill_color = QColor(Qt::red);
        }
        else{
            fill_color = QColor(Qt::green);
        }
        fill_color.setAlphaF(0.8);
        tile_row = i>>4;
        base_column = i&0xf;
        base_column = (base_column<<3) + (base_column);
        for(j=0;j<8;j++){
            edit_line = (QRgb*)overwriteable_image.scanLine((tile_row<<3) + (tile_row) + j);
            for(k=0;k<8;k++){
                edit_line[base_column + k] = fill_color.rgba();
            }
        }
    }
    overwriteable_pixmap->setPixmap(QPixmap::fromImage(overwriteable_image));

}


void selectCHRDialog::on_comboBox_2_currentIndexChanged(int index)
{
    if(index<0){
        ui->label_2->setPixmap(QPixmap());
        return;
    }
    ui->checkBox->setChecked(overwrite_list.at(index));
    if((unsigned int)index < bg_list.size()){
        ui->label_2->setPixmap(QPixmap::fromImage(bg_list.at(index)->image));
        ui->label_2->resize(bg_list.at(index)->image.width(),bg_list.at(index)->image.height());
        return;
    }
    index -= bg_list.size();
    if((unsigned int)index < sprite_list.size()){
        ui->label_2->setPixmap(QPixmap::fromImage(sprite_list.at(index)->image));
        ui->label_2->resize(sprite_list.at(index)->image.width(),sprite_list.at(index)->image.height());
        return;
    }
    index -= sprite_list.size();
    ui->label_2->setPixmap(QPixmap::fromImage(dynamic_list.at(index)->image));
    ui->label_2->resize(dynamic_list.at(index)->image.width(),dynamic_list.at(index)->image.height());
}

void selectCHRDialog::accept(){
    selected_page = ui->comboBox->currentIndex();
    delete [] old_pages;
    QDialog::accept();
}

void selectCHRDialog::reject(){
    unsigned int i,j;
    for(i=0;i<num_chr_pages;i++){
        for(j=0;j<0x100;j++) CHR_pages[i].t[j].shared = old_pages[i].shared[j];
    }
    delete [] old_pages;
    setResult(0);
    QDialog::reject();
}

uint8_t selectCHRDialog::getSelectedPage(){
    return selected_page;
}
