#include "importbgdialog.h"
#include "ui_importbgdialog.h"
#include <QFileDialog>
#include <QMessageBox>

importBGdialog::importBGdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::importBGdialog)
{
    ui->setupUi(this);
    unsigned int i,j,k;
    edit_arrangement.arrangement = "";
    edit_arrangement.attrib_bits = "";
    edit_arrangement.modified = false;
    setWindowTitle(QString("Background Editor"));
    indicators_pix_item = NULL;
    palette_set default_palette;
    default_palette.nes_ubg = 0x0f;
    for(i=0;i<4;i++){
        for(j=1;j<4;j++){
            default_palette.p[i].nes_colors[j] = 0x0f;
        }
    }
    bg_palettes = new palette_set_selector(this,10,600,default_palette);
    stamp_mode = false;
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);
    connect(bg_palettes,SIGNAL (color_changed(uint8_t,uint8_t)),this,SLOT (on_color_selected(uint8_t,uint8_t)));
    ui->checkBox->setChecked(false);
    uint16_t max_bg_id = 0;
    for(i=0;i<num_bgs;i++){
        if(bgs[i]->id > max_bg_id) max_bg_id = bgs[i]->id;
    }
    k=0;
    for(i=0;i<=max_bg_id;i++){
        for(j=0;j<num_bgs;j++){
            if(bgs[j]->id == i){
                sorted_list[k++] = bgs[j];
                ui->comboBox->addItem(QString(convertByteToHexString(bgs[j]->id).c_str()));
                break;
            }
        }
    }
    ui->comboBox->addItem(QString("New"));

    ui->newBGView->setScene(&new_scene);
    if(ui->comboBox->count() > 1){
        ui->label_4->setPixmap(QPixmap::fromImage(sorted_list[0]->image));
        ui->label_4->resize(sorted_list[0]->image.width(),sorted_list[0]->image.height());
        bg_palettes->setPalette(sorted_list[0]->bestPalette());
    }
    selected_palette = 0;
    selected_color = 0;
    selected_tile = 0xfA;
    indicators = QImage(176,176,QImage::Format_ARGB32);
    indicators.fill(Qt::gray);
    for(i=0;i<0x100;i++){
        chr_array[i] = new chrItem();
        chr_array[i]->setID(i);
        connect(chr_array[i],SIGNAL(chr_pressed()),this,SLOT(chr_selection_changed()));
        chr_array[i]->setOffset((i&0xf)*10,(i>>4)*10);
        chr_scene.addItem(chr_array[i]);
    }
    ui->CHR_view->setScene(&chr_scene);
    current_tile = new tileEditItem;
    indicators_pix_item = new QGraphicsPixmapItem;
    connect(current_tile,SIGNAL(tile_pressed(QGraphicsSceneMouseEvent *)),this,SLOT(tile_clicked(QGraphicsSceneMouseEvent *)));
    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());

    on_checkBox_toggled(false);
    indicators_pix_item->setOffset(0,0);
    tile_selector = QImage(12,12,QImage::Format_ARGB32);
    QRgb * edit_line;
    for(i=0;i<12;i++){
        edit_line = (QRgb *) tile_selector.scanLine(i);
        for(j=0;j<12;j++){
            if(i<2 || i>=10){
                edit_line[j] = QColor(Qt::black).rgba();
            }
            else{
                if(j<2 || j>=10){
                    edit_line[j] = QColor(Qt::black).rgba();
                }
                else{
                    edit_line[j] = QColor(Qt::transparent).rgba();
                }
            }
        }
    }
    tile_selector_pix_item = new QGraphicsPixmapItem(QPixmap::fromImage(tile_selector));
    chr_scene.addItem(tile_selector_pix_item);
    ui->tile_view->setScene(&tile_scene);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    current_tile->setPixmap(QPixmap::fromImage(tiles[selected_palette][selected_tile]));
    tile_scene.clear();
    tile_scene.addItem(current_tile);
    ui->tile_view->scale(20,20);
    //ui->tile_view->fitInView(current_tile,Qt::KeepAspectRatio);
    arrangement_tile = -1;
    ui->clipboard_view->setScene(&clipboard_scene);
    ui->newBGView->setDragMode(QGraphicsView::RubberBandDrag);
    last_focus = -1;
    copy_shortcut = new QShortcut(QKeySequence(QString("Ctrl+C")),this);
    paste_shortcut = new QShortcut(QKeySequence(QString("Ctrl+V")),this);
    undo_shortcut = new QShortcut(QKeySequence(QString("Ctrl+Z")),this);
    redo_shortcut = new QShortcut(QKeySequence(QString("Ctrl+Shift+Z")),this);
    connect(copy_shortcut,SIGNAL(activated()),this,SLOT(copy_slot()));
    connect(paste_shortcut,SIGNAL(activated()),this,SLOT(paste_slot()));
    connect(undo_shortcut,SIGNAL(activated()),this,SLOT(undo_slot()));
    connect(redo_shortcut,SIGNAL(activated()),this,SLOT(redo_slot()));
    ui->newBGView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->newBGView,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    clipboard = QApplication::clipboard();
    clipboard_image = clipboard->image();
    clipboard_item = new QGraphicsPixmapItem(clipboard->pixmap());
    clipboard_scene.addItem(clipboard_item);
    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(clipboard_changed()));
    for(i=0;i<UNDO_SIZE;i++){
        undo_actions[i].action_type = 0;
        undo_actions[i].data = NULL;
    }
    undo_min = 0;
    undo_max = 0;
    undo_position = 0;
    tile_edit_progress = false;
    connect(ui->horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScroll(int)));
    connect(ui->verticalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(verticalScroll(int)));


    widget_offsets[0x00] = ui->CHR_view->pos();
    widget_offsets[0x01] = ui->buttonBox->pos();
    widget_offsets[0x02] = ui->checkBox->pos();
    widget_offsets[0x03] = ui->clipboard_view->pos();
    widget_offsets[0x04] = ui->comboBox->pos();
    widget_offsets[0x05] = bg_palettes->pos();
    widget_offsets[0x06] = ui->label->pos();
    widget_offsets[0x07] = ui->label_2->pos();
    widget_offsets[0x08] = ui->label_3->pos();
    widget_offsets[0x09] = ui->label_4->pos();
    widget_offsets[0x0a] = ui->label_5->pos();
    widget_offsets[0x0b] = ui->label_6->pos();
    widget_offsets[0x0c] = ui->newBGView->pos();
    widget_offsets[0x0d] = ui->pushButton->pos();
    widget_offsets[0x0e] = ui->pushButton_10->pos();
    widget_offsets[0x0f] = ui->pushButton_2->pos();
    widget_offsets[0x10] = ui->pushButton_3->pos();
    widget_offsets[0x11] = ui->pushButton_4->pos();
    widget_offsets[0x12] = ui->pushButton_5->pos();
    widget_offsets[0x13] = ui->pushButton_8->pos();
    widget_offsets[0x14] = ui->pushButton_9->pos();
    widget_offsets[0x15] = ui->radioButton->pos();
    widget_offsets[0x16] = ui->radioButton_2->pos();
    widget_offsets[0x17] = ui->tile_view->pos();
    ui->horizontalScrollBar->raise();
    ui->verticalScrollBar->raise();
}

importBGdialog::~importBGdialog()
{
    delete ui;
    delete bg_palettes;
    for(unsigned int i=0;i<0x100;i++){
        delete chr_array[i];
    }
    delete current_tile;
    delete clipboard_item;
}

void importBGdialog::on_pushButton_clicked()
{
    tile_released();
    QString fileName = QFileDialog::getOpenFileName(this,QString(""),QString(""),tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));
    if(!fileName.isEmpty()){
        clipboard_image = QImage(fileName);
        clipboard->setImage(clipboard_image);
        clipboard_item->setPixmap(QPixmap::fromImage(clipboard_image));
        clipboard_scene.addItem(clipboard_item);
    }
}

void importBGdialog::on_comboBox_currentIndexChanged(int index)
{
    if(ui->comboBox->count() < num_bgs) return;
    tile_released();
    unsigned int i,j;
    if(edit_arrangement.modified){
        if(index == old_combo_index) return;
        QMessageBox msg;
        msg.setText(QString("Current image has been modified"));
        msg.setInformativeText(QString("Would you like to save before changing images?"));
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Save);
        int ret = msg.exec();
        switch(ret){
        case QMessageBox::Save:
            for(i=0;i<edit_arrangement.tiles.size();i++){
                CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(i)->id] = page.t[edit_arrangement.tiles.at(i)->id];
                edit_arrangement.tiles.at(i) = &(CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(i)->id]);
            }

            if(old_combo_index < (ui->comboBox->count() - 1)){
                *sorted_list[old_combo_index] = edit_arrangement;
                sorted_list[old_combo_index]->modified = false;
                sorted_list[old_combo_index]->generateImage();
            }
            else{
                bgs[num_bgs] = new bg_arrangement;
                *bgs[num_bgs] = edit_arrangement;
                bgs[num_bgs]->references = 0;
                bgs[num_bgs]->id = num_bgs;
                bgs[num_bgs]->modified = false;
                bgs[num_bgs]->generateImage();
                num_bgs++;
            }
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            ui->comboBox->setCurrentIndex(old_combo_index);
            return;
        default:
            break;
        }
    }
    for(i=0;i<num_chr_pages;i++){
        for(j=0;j<0x100;j++){
            CHR_pages[i].t[j].shared = false;
        }
    }
    for(i=0;i<num_bgs;i++){
        if(index == i) continue;
        for(j=0;j<sorted_list[i]->tiles.size();j++){
            sorted_list[i]->tiles.at(j)->shared = true;
        }
    }
    for(i=0;i<num_sprites;i++){
        for(j=0;j<sprites[i]->tiles.size();j++) sprites[i]->tiles.at(j)->shared = true;
    }
    for(i=0;i<num_ppu_strings;i++){
        for(j=0;j<ppu_strings[i]->tiles.size();j++) ppu_strings[i]->tiles.at(j)->shared = true;
    }
    if(index < (ui->comboBox->count() - 1)){
        ui->label_4->setPixmap(QPixmap::fromImage(sorted_list[index]->image));
        ui->label_4->resize(sorted_list[index]->image.width(),sorted_list[index]->image.height());
        bg_palettes->setPalette(sorted_list[index]->bestPalette());
        pals = bg_palettes->getPalette();

        edit_arrangement = *sorted_list[index];
        page = CHR_pages[sorted_list[index]->gfx_page];
        updateCHRMask();
        drawCHR();
        resizeArrangement(edit_arrangement.metatilesX<<2,edit_arrangement.metatilesY<<1);
        drawArrangement();
    }
    else{
        updateCHRMask();
        drawCHR();
        resizeArrangement(edit_arrangement.metatilesX<<2,edit_arrangement.metatilesY<<1);
        drawArrangement();
    }
    old_combo_index = index;
}

void importBGdialog::on_pushButton_2_released()     //Reset the default palettes for the image
{
    tile_released();
    if(ui->comboBox->currentIndex() < (ui->comboBox->count() - 1)){
        bg_palettes->setPalette(sorted_list[ui->comboBox->currentIndex()]->bestPalette());
        pals = bg_palettes->getPalette();
        drawCHR();
        drawArrangement();
    }
}

void importBGdialog::drawCHR(){
    unsigned int i,j,k,pal_counter;
    uint8_t color_index;
    pals.convertColors();
    QRgb * edit_line;
    for(pal_counter = 0;pal_counter<4;pal_counter++){
        for(i=0;i<0x100;i++){
            tiles[pal_counter][i] = QImage(8,8,QImage::Format_ARGB32);
            for(j=0;j<8;j++){
                edit_line = (QRgb*) tiles[pal_counter][i].scanLine(j);
                for(k=0;k<8;k++){
                    color_index = 0;
                    color_index |= ((page.t[i].t[j]) >> (7-k))&0x01;
                    color_index |= (((page.t[i].t[j+8]) >> (7-k))&0x01)<<1;
                    edit_line[k] = pals.p[pal_counter].p[color_index].rgb();
                }
            }
            if(pal_counter== selected_palette) chr_array[i]->setPixmap(QPixmap::fromImage(tiles[pal_counter][i]));
        }
    }

    current_tile->setPixmap(QPixmap::fromImage(tiles[selected_palette][selected_tile]));
    indicators_pix_item->setPixmap(QPixmap::fromImage(indicators));

}

void importBGdialog::on_color_selected(uint8_t p,uint8_t color){
    tile_released();
    pals = bg_palettes->getPalette();
    pals.convertColors();
    selected_color = color&3;
    selected_palette = p&3;
    if(last_focus == 1){
        uint16_t arrangement_metatile = last_arrangement_tile>>3;
        uint8_t mask = 3;
        uint8_t bits = selected_palette;
        //selected_palette = edit_arrangement.attrib_bits[arrangement_metatile>>1];
        if(!(arrangement_metatile&1)){
            bits<<=4;
            mask<<=4;
        }
        if(!(last_arrangement_tile&2)){
            bits<<=2;
            mask<<=2;
        }

        while(undo_max!=undo_position){
            switch(undo_actions[undo_max].action_type){
            case 1:
                delete ((undo_draw *)undo_actions[undo_max].data);
                break;
            case 2:
                delete ((undo_paste *)undo_actions[undo_max].data);
                break;
            case 3:
                delete ((undo_palette *)undo_actions[undo_max].data);
                break;
            case 4:
                delete ((undo_resize *)undo_actions[undo_max].data);
                break;
            default:
                break;
            }
            undo_actions[undo_max].data = NULL;
            undo_actions[undo_max].action_type= 0;
            undo_max--;
            if(undo_max<0) undo_max += UNDO_SIZE;
        }
        switch(undo_actions[undo_position].action_type){
        case 1:
            delete ((undo_draw *)undo_actions[undo_max].data);
            break;
        case 2:
            delete ((undo_paste *)undo_actions[undo_max].data);
            break;
        case 3:
            delete ((undo_palette *)undo_actions[undo_max].data);
            break;
        case 4:
            delete ((undo_resize *)undo_actions[undo_max].data);
            break;
        default:
            break;
        }
        undo_actions[undo_position].action_type = 3;
        undo_actions[undo_position].data = new undo_palette;
        ((undo_palette *)undo_actions[undo_position].data)->old_attribs = edit_arrangement.attrib_bits;
        edit_arrangement.attrib_bits[arrangement_metatile>>1] = (edit_arrangement.attrib_bits[arrangement_metatile>>1]&(0xff^mask)) | bits;
        ((undo_palette *)undo_actions[undo_position].data)->new_attribs = edit_arrangement.attrib_bits;
        undo_position++;
        if(undo_position >= UNDO_SIZE) undo_position -= UNDO_SIZE;
        undo_max = undo_position;
        if(undo_min == undo_max){
            undo_min++;
            if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
        }

    }
    drawCHR();
    drawArrangement();
}

void importBGdialog::on_checkBox_toggled(bool checked)
{
    tile_released();
    indicators_pix_item->setPixmap(QPixmap::fromImage(indicators));
    if(checked){
        chr_scene.addItem(indicators_pix_item);
        indicators_pix_item->setOffset(0,0);
        indicators_pix_item->setAcceptedMouseButtons(0);    //Don't allow interaction with the indicator mask
    }
    else{
        chr_scene.removeItem(indicators_pix_item);
    }

}

void importBGdialog::chr_selection_changed(){
    tile_released();
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    current_tile->setPixmap(QPixmap::fromImage(tiles[selected_palette][selected_tile]));
    last_focus = 2;
}

void importBGdialog::tile_clicked(QGraphicsSceneMouseEvent * event){
    if(!tile_edit_progress){
        tile_edit_progress = true;
        while(undo_max!=undo_position){
            switch(undo_actions[undo_max].action_type){
            case 1:
                delete ((undo_draw *)undo_actions[undo_max].data);
                break;
            case 2:
                delete ((undo_paste *)undo_actions[undo_max].data);
                break;
            case 3:
                delete ((undo_palette *)undo_actions[undo_max].data);
                break;
            case 4:
                delete ((undo_resize *)undo_actions[undo_max].data);
                break;
            default:
                break;
            }
            undo_actions[undo_max].data = NULL;
            undo_actions[undo_max].action_type= 0;
            undo_max--;
            if(undo_max<0) undo_max += UNDO_SIZE;
        }
        switch(undo_actions[undo_position].action_type){
        case 1:
            delete ((undo_draw *)undo_actions[undo_max].data);
            break;
        case 2:
            delete ((undo_paste *)undo_actions[undo_max].data);
            break;
        case 3:
            delete ((undo_palette *)undo_actions[undo_max].data);
            break;
        case 4:
            delete ((undo_resize *)undo_actions[undo_max].data);
            break;
        default:
            break;
        }
        undo_actions[undo_position].action_type = 1;
        undo_actions[undo_position].data = new undo_draw;
        ((undo_draw *)undo_actions[undo_position].data)->old_tile = page.t[selected_tile];
        ((undo_draw *)undo_actions[undo_position].data)->old_tile.id = selected_tile;
        ((undo_draw *)undo_actions[undo_position].data)->new_tile.id = selected_tile;
    }


    uint8_t x = event->pos().x();
    uint8_t y = event->pos().y();
    page.t[selected_tile].t[y] = (page.t[selected_tile].t[y]&(0xff ^ (1<<(7-x)))) | ((selected_color&1)<<(7-x));
    page.t[selected_tile].t[y + 8] = (page.t[selected_tile].t[y + 8]&(0xff ^ (1<<(7-x)))) | (((selected_color&2)>>1)<<(7-x));
    unsigned int i=0;
    for(i=0;i<edit_arrangement.tiles.size();i++){
        if(edit_arrangement.tiles.at(i)->id == selected_tile){
            edit_arrangement.modified = true;
            break;
        }
    }
    ((undo_draw *)undo_actions[undo_position].data)->new_tile = page.t[selected_tile];
    drawCHR();
    drawArrangement();
    //QMessageBox::critical(NULL,QString("Mouse X"),QString(convertByteToHexString(event->pos().x()).c_str()));

}

void importBGdialog::on_pushButton_9_clicked()  //CHR page zoom in button
{
    tile_released();
    QMatrix stretch= ui->CHR_view->matrix();
    if(stretch.m11() < 8){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())<<1));
        zoom_label.append("00%");
        ui->label_6->setText(zoom_label);
        ui->CHR_view->scale(2,2);
    }
}


void importBGdialog::on_pushButton_10_clicked()
{
    tile_released();
    QMatrix stretch = ui->CHR_view->matrix();
    if(stretch.m11() > 1){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())>>1));
        zoom_label.append("00%");
        ui->label_6->setText(zoom_label);
        ui->CHR_view->scale(0.5,0.5);
    }
}

void importBGdialog::resizeArrangement(uint8_t tile_x, uint8_t tile_y){
    unsigned int i,j,k;
    //unsigned int num_tiles;

    new_scene.clear();
/*    for(i=0;i<image.size();i++){
        delete image.at(i);         //This causes a segfault when changing image w/ combo box
    }*/
    image.clear();
    std::vector <NEStile *> temp_tiles;
    std::string temp_attribs = "";
    //num_tiles= tile_x * tile_y;
    //image = new arrangementEditItem * [num_tiles];
    QImage filler(8,8,QImage::Format_ARGB32);
    filler.fill(Qt::gray);
    arrangementEditItem * image_ptr;
    uint8_t metatiles_X = tile_x >>2;
    uint8_t metatiles_Y = tile_y >>1;
    uint16_t tile_num,old_tile_num;
    uint16_t attrib_offset;
    bool attrib_toggle = false;
    uint8_t attrib_value = 0;
    uint8_t temp_value;
    for(i=0;i<metatiles_Y;i++){
        for(j=0;j<metatiles_X;j++){
            uint16_t base_x = (j<<5) + (j<<2) + (j<<1);
            uint16_t base_y = (i<<4) + (i<<1) + i;

            attrib_offset = ((i*edit_arrangement.metatilesX) + j)>>1;
            if((i<edit_arrangement.metatilesY) && (j<edit_arrangement.metatilesX)){
                if(attrib_toggle){
                    temp_value = edit_arrangement.attrib_bits[attrib_offset];
                    if(!(((i*edit_arrangement.metatilesX)+j)&1)) temp_value>>=4;
                    temp_attribs += (temp_value&0xf) | (attrib_value);
                }
                else{
                    temp_value = edit_arrangement.attrib_bits[attrib_offset];
                    if((((i*edit_arrangement.metatilesX)+j)&1)) temp_value<<=4;
                    attrib_value = temp_value&0xf0;
                }
            }
            else{
                if(attrib_toggle){
                    temp_attribs += attrib_value;   //set default palette bits to 0
                }
                else{
                    attrib_value = 0;               //set default palette bits to 0
                }
            }
            attrib_toggle = !attrib_toggle;
            for(k=0;k<8;k++){
                tile_num = ((i*metatiles_X)<<3) + (j<<3) + k;
                old_tile_num = ((i*edit_arrangement.metatilesX)<<3) + (j<<3) + k;
                if((i<edit_arrangement.metatilesY) && (j<edit_arrangement.metatilesX)){
                    temp_tiles.push_back(&(page.t[edit_arrangement.tiles.at(old_tile_num)->id]));
                }
                else{
                    temp_tiles.push_back(&(page.t[0xFA]));
                }
                image_ptr = new arrangementEditItem(this);
                image_ptr->setID(tile_num);
                image_ptr->setOffset(base_x + ((k&3)<<3) + (k&3) + ((k&2)>>1),base_y + ((k&4)<<1) + ((k&4)>>2));
                image_ptr->storePosition((j<<5) + ((k&3)<<3),(i<<4) + ((k&4)<<1));
                image_ptr->setPixmap(QPixmap::fromImage(filler));
                connect(image_ptr,SIGNAL(arrangement_pressed()),this,SLOT(arrangement_clicked()));
                //connect(image_ptr,SIGNAL(arrangement_mouse_move()),this,SLOT(arrangement_mouse_select_area()));
                //connect(image_ptr,SIGNAL(arrangement_mouse_release()),this,SLOT(arrangement_mouse_release()));
                image_ptr->setFlag(QGraphicsItem::ItemIsSelectable);
                image.push_back(image_ptr);
                new_scene.addItem(image_ptr);
            }
        }
    }
    if(attrib_toggle){
        temp_attribs += attrib_value;
    }
    edit_arrangement.tiles = temp_tiles;
    edit_arrangement.attrib_bits = temp_attribs;
    edit_arrangement.metatilesX = metatiles_X;
    edit_arrangement.metatilesY = metatiles_Y;
    delete [] edit_arrangement.metatiles;
    edit_arrangement.metatiles = new metatile[metatiles_X*metatiles_Y];
}

void importBGdialog::drawArrangement(){
    unsigned int i,j,k;
    QImage edit_tile(8,8,QImage::Format_ARGB32);
    QRgb * edit_line;
    uint8_t color_index;
    uint8_t attribs;
    for(i=0;i<edit_arrangement.tiles.size();i++){
        attribs = edit_arrangement.attrib_bits[i>>4];
        if(!((i>>3)&1)) attribs>>=4;
        if(!(i&2)) attribs>>=2;
        attribs&=0x3;
        for(j=0;j<8;j++){
            edit_line = (QRgb*) edit_tile.scanLine(j);
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= ((edit_arrangement.tiles.at(i)->t[j]) >> (7-k))&0x01;
                color_index |= (((edit_arrangement.tiles.at(i)->t[j+8]) >> (7-k))&0x01)<<1;
                edit_line[k] = pals.p[attribs].p[color_index].rgb();
            }
        }
        image.at(i)->setTileID(edit_arrangement.tiles.at(i)->id);
        image.at(i)->setPixmap(QPixmap::fromImage(edit_tile));
    }
}

void importBGdialog::on_pushButton_4_clicked()  //BG arrangement zoom in button
{
    tile_released();
    QMatrix stretch= ui->newBGView->matrix();
    if(stretch.m11() < 8){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())<<1));
        zoom_label.append("00%");
        //ui->label_6->setText(zoom_label);
        ui->newBGView->scale(2,2);
    }
}

void importBGdialog::on_pushButton_5_clicked()  //BG arrangement zoom out button
{
    tile_released();
    QMatrix stretch= ui->newBGView->matrix();
    if(stretch.m11() > 1){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())>>1));
        zoom_label.append("00%");
        //ui->label_6->setText(zoom_label);
        ui->newBGView->scale(0.5,0.5);
    }
}

void importBGdialog::arrangement_clicked(){
    tile_released();
    arrangementEditItem * selection = qobject_cast<arrangementEditItem *>(sender());
    arrangement_tile = selection->getID();
    last_focus = 1;
    last_arrangement_tile = arrangement_tile;
    if(stamp_mode){
        paste_slot();
    }
    else{
        uint16_t arrangement_metatile = arrangement_tile>>3;
        selected_tile = selection->getTileID();
        tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);

        uint8_t old_palette = selected_palette;
        selected_palette = edit_arrangement.attrib_bits[arrangement_metatile>>1];
        if(!(arrangement_metatile&1)) selected_palette>>=4;
        if(!(arrangement_tile&2)) selected_palette>>=2;
        selected_palette&=3;
        if(selected_palette != old_palette){
            bg_palettes->setSelectedPalette(selected_palette);
            pals = bg_palettes->getPalette();
            pals.convertColors();
            drawCHR();
        }
        current_tile->setPixmap(QPixmap::fromImage(tiles[selected_palette][selected_tile]));
        selection->setSelected(true);
    }
}

void importBGdialog::accept(){
    unsigned int i;
    if(edit_arrangement.modified){
        for(i=0;i<edit_arrangement.tiles.size();i++){
            CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(i)->id] = page.t[edit_arrangement.tiles.at(i)->id];
            edit_arrangement.tiles.at(i) = &(CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(i)->id]);
        }

        if(ui->comboBox->currentIndex() < (ui->comboBox->count() - 1)){
            *sorted_list[ui->comboBox->currentIndex()] = edit_arrangement;
            sorted_list[ui->comboBox->currentIndex()]->modified = false;
        }
        else{
            bgs[num_bgs] = new bg_arrangement;
            *bgs[num_bgs] = edit_arrangement;
            bgs[num_bgs]->references = 0;
            bgs[num_bgs]->id = num_bgs;
            bgs[num_bgs]->modified = false;
            num_bgs++;
        }

    }
    QDialog::accept();
}

void importBGdialog::copy_slot(){
    if(last_focus == 1){    //Copy from the arrangement
        uint16_t min_x, max_x, min_y, max_y;
        bool any_selected = false;
        min_x = 0xffff;
        min_y = 0xffff;
        max_x = 0;
        max_y = 0;
        unsigned int i,j,k;
        arrangementEditItem * test_item;
        for(i=0;i<image.size();i++){
            test_item = image.at(i);
            if(test_item->isSelected()){
                any_selected = true;
                if(test_item->getX() < min_x) min_x = test_item->getX();
                if(test_item->getX() > max_x) max_x = test_item->getX();
                if(test_item->getY() < min_y) min_y = test_item->getY();
                if(test_item->getY() > max_y) max_y = test_item->getY();
            }
        }
        if(!any_selected){
            clipboard_image = image.at(last_arrangement_tile)->pixmap().toImage();
            clipboard->setImage(clipboard_image);
            return;
        }
        uint16_t pixel_width = max_x - min_x + 8;
        uint16_t pixel_height = max_y - min_y + 8;
        clipboard_image = QImage(pixel_width,pixel_height,QImage::Format_ARGB32);
        clipboard_image.fill(Qt::transparent);
        uint16_t base_x,base_y;
        //uint8_t color_index;
        //uint8_t tile_palette;
        QRgb * copy_line;
        QRgb * tile_line;
        for(i=0;i<image.size();i++){
            test_item = image.at(i);
            if(test_item->isSelected()){
                base_x = test_item->getX() - min_x;
                base_y = test_item->getY() - min_y;
                QImage tile_image(test_item->pixmap().toImage());
                for(j=0;j<8;j++){
                    copy_line = (QRgb *)clipboard_image.scanLine(base_y + j);
                    tile_line = (QRgb *)tile_image.scanLine(j);
                    for(k=0;k<8;k++){
                        copy_line[base_x + k] = tile_line[k];
                    }
                }
            }
        }
        clipboard->setImage(clipboard_image);
    }
    else if(last_focus == 2){   //Copy from the CHR page
        clipboard->setImage(tiles[selected_palette][selected_tile]);
    }
    else{   //Copy the tile currently being edited...wait.  Isn't this the same as from the CHR page?

    }
}

void importBGdialog::paste_slot(){
    tile_released();
    if(last_focus != 1) return;

    while(undo_max!=undo_position){
        switch(undo_actions[undo_max].action_type){
        case 1:
            delete ((undo_draw *)undo_actions[undo_max].data);
            break;
        case 2:
            delete ((undo_paste *)undo_actions[undo_max].data);
            break;
        case 3:
            delete ((undo_palette *)undo_actions[undo_max].data);
            break;
        case 4:
            delete ((undo_resize *)undo_actions[undo_max].data);
            break;
        default:
            break;
        }
        undo_actions[undo_max].data = NULL;
        undo_actions[undo_max].action_type= 0;
        undo_max--;
        if(undo_max<0) undo_max += UNDO_SIZE;
    }
    switch(undo_actions[undo_position].action_type){
    case 1:
        delete ((undo_draw *)undo_actions[undo_max].data);
        break;
    case 2:
        delete ((undo_paste *)undo_actions[undo_max].data);
        break;
    case 3:
        delete ((undo_palette *)undo_actions[undo_max].data);
        break;
    case 4:
        delete ((undo_resize *)undo_actions[undo_max].data);
        break;
    default:
        break;
    }
    undo_actions[undo_position].action_type = 2;
    undo_actions[undo_position].data = new undo_paste;
    ((undo_paste *)undo_actions[undo_position].data)->old_arrangement = edit_arrangement;
    ((undo_paste *)undo_actions[undo_position].data)->old_page = page;

    clipboard_image = clipboard->image();
    pals.convertColors();
    unsigned int i,j,k,l,m;
    uint16_t base_tile_x = 0;
    uint16_t base_tile_y = 0;
    uint16_t metatile_num = 0;
    uint8_t base_metatile_X = 0;
    uint8_t base_metatile_Y = 0;
    for(i=0;i<image.size();i++){    //Locate the write position to paste to within the arrangement
        if(((arrangementEditItem*)image.at(i))->isSelected()){
            metatile_num = ((arrangementEditItem*)image.at(i))->getID() >> 3;
            base_metatile_X = metatile_num % edit_arrangement.metatilesX;
            base_metatile_Y = metatile_num / edit_arrangement.metatilesX;
            base_tile_x = base_metatile_X << 2;
            base_tile_x += ((arrangementEditItem*)image.at(i))->getID()&0x3;
            base_tile_y = base_metatile_Y << 1;
            base_tile_y += (((arrangementEditItem*)image.at(i))->getID()&4)>>2;
            break;
        }
    }
    if(i>=image.size()){
        metatile_num = last_arrangement_tile >> 3;
        base_metatile_X = metatile_num % edit_arrangement.metatilesX;
        base_metatile_Y = metatile_num / edit_arrangement.metatilesX;
        base_tile_x = base_metatile_X << 2;
        base_tile_x += last_arrangement_tile&0x3;
        base_tile_y = base_metatile_Y << 1;
        base_tile_y += (last_arrangement_tile&4)>>2;
    }
    //Organize the clipboard image into tiles
    uint16_t tile_x = clipboard_image.width()>>3;
    uint16_t tile_y = clipboard_image.height()>>3;
    uint16_t num_clipboard_tiles = tile_x * tile_y;
    QImage * clipboard_tiles = new QImage[num_clipboard_tiles];
    int16_t * matches = new int16_t[num_clipboard_tiles];   //-2 = transparent, -1 = unmatched
    QRgb * tile_line;
    QRgb * clipboard_line;
    for(i=0;i<num_clipboard_tiles;i++){
        clipboard_tiles[i] = QImage(8,8,QImage::Format_ARGB32);
        clipboard_tiles[i].fill(Qt::transparent);
        matches[i] = -2;
        uint16_t base_pixel_x,base_pixel_y;
        base_pixel_x = (i%(clipboard_image.width()>>3))<<3;
        base_pixel_y = (i/(clipboard_image.width()>>3))<<3;
        for(j=0;j<8;j++){
            tile_line = (QRgb *)clipboard_tiles[i].scanLine(j);
            clipboard_line = (QRgb *)clipboard_image.scanLine(base_pixel_y + j);
            for(k=0;k<8;k++){
                if(qAlpha(clipboard_line[base_pixel_x + k]) == 0xff){
                    tile_line[k] = clipboard_line[base_pixel_x + k];
                    matches[i] = -1;
                }
            }
        }
    }
    QImage * quad_tile[4];
    bool from_arrangement[4];
    uint8_t palette_from_arrangement;
    for(i=base_tile_y&0xfffe;i<(base_tile_y + tile_y);i+=2){
        if(i >= (edit_arrangement.metatilesY<<1)) break;    //Quit if pasted image extends below bottom of arrangement
        for(j=base_tile_x&0xfffe;j<(base_tile_x + tile_x);j+=2){
            if(j >= (edit_arrangement.metatilesX<<2)) break;    //Quit if pasted image extends to the right of arrangement

            metatile_num = ((i>>1)*edit_arrangement.metatilesX) + (j>>2);
            //quad_tile[((i&1)<<1) + (j&1)] = &tiles[][edit_arrangement.tiles.at((metatile_num<<3) + ((i&1)<<2) + (j&3))];
            palette_from_arrangement = edit_arrangement.attrib_bits[metatile_num>>1];
            if(!(metatile_num&1)) palette_from_arrangement>>=4;
            if(!(j&2)) palette_from_arrangement>>=2;
            palette_from_arrangement&=3;
            if(i< base_tile_y){
                quad_tile[0] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3))->id];
                quad_tile[1] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + ((j+1)&3))->id];
                from_arrangement[0] = true;
                from_arrangement[1] = true;
            }
            else{
                if(j< base_tile_x){
                    quad_tile[0] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3))->id];
                    from_arrangement[0] = true;
                }
                else{
                    fillTransparent(tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3))->id],  \
                            clipboard_tiles[(i-base_tile_y)*tile_x + (j - base_tile_x)]);
                    quad_tile[0] = &clipboard_tiles[(i-base_tile_y)*tile_x + (j - base_tile_x)];
                    from_arrangement[0] = false;
                }
                if((j+1)<(base_tile_x + tile_x)){
                    fillTransparent(tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3))->id], \
                            clipboard_tiles[(i-base_tile_y)*tile_x + (j - base_tile_x)+1]);
                    quad_tile[1] = &clipboard_tiles[(i-base_tile_y)*tile_x + (j - base_tile_x)+1];
                    from_arrangement[1] = false;
                }
                else{
                    quad_tile[1] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3))->id];
                    from_arrangement[1] = true;
                }
            }
            if((i+1)<(base_tile_y + tile_y)){
                if(j< base_tile_x){
                    quad_tile[2] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3) + 4)->id];
                    from_arrangement[2] = true;
                }
                else{
                    fillTransparent(tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3) + 4)->id], \
                            clipboard_tiles[(i-base_tile_y+1)*tile_x + (j - base_tile_x)]);
                    quad_tile[2] = &clipboard_tiles[(i-base_tile_y+1)*tile_x + (j - base_tile_x)];
                    from_arrangement[2] = false;
                }
                if((j+1)<(base_tile_x + tile_x)){
                    fillTransparent(tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + ((j+1)&3) + 4)->id], \
                            clipboard_tiles[(i-base_tile_y+1)*tile_x + (j - base_tile_x)+1]);
                    quad_tile[3] = &clipboard_tiles[(i-base_tile_y+1)*tile_x + (j - base_tile_x)+1];
                    from_arrangement[3] = false;
                }
                else{
                    quad_tile[3] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + ((j+1)&3) + 4)->id];
                    from_arrangement[3] = true;
                }
            }
            else{
                quad_tile[2] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + (j&3) + 4)->id];
                quad_tile[3] = &tiles[palette_from_arrangement][edit_arrangement.tiles.at((metatile_num<<3) + ((j+1)&3) + 4)->id];
                from_arrangement[2] = true;
                from_arrangement[3] = true;
            }

            uint32_t color_diff_min = 0xffffffff;
            uint32_t color_diff;
            uint8_t best_color = 0;
            for(k=0;k<4;k++){
                color_diff = colorDiff(k,quad_tile);
                if(color_diff < color_diff_min){
                    color_diff_min = color_diff;
                    best_color = k;
                }
            }
            k = 3;
            l = best_color;
            if(!(metatile_num&1)){
                l<<=4;
                k<<=4;
            }
            if(!(j&2)){
                l<<=2;
                k<<=2;
            }
            edit_arrangement.attrib_bits[metatile_num>>1] = ((edit_arrangement.attrib_bits[metatile_num>>1])&(0xff^k)) | l;

            NEStile quad_NEStile[4];
            for(k=0;k<4;k++){
                uint16_t arrangement_index = (metatile_num<<3) + (j&2) + (k&1) + ((k&2)<<1);
                quad_NEStile[k] = createNESTile(best_color,quad_tile[k]);
                if(!from_arrangement[k]){
                    int16_t tile_count;
                    for(tile_count=0xff;tile_count>=0;tile_count--){
                        /*if(quad_NEStile[k] == page.t[tile_count]){
                            edit_arrangement.tiles.at(arrangement_index) = &page.t[tile_count];
                            break;
                        }*/
                        if(*quad_tile[k] == tiles[best_color][tile_count]){
                            edit_arrangement.tiles.at(arrangement_index) = &page.t[tile_count];
                            break;
                        }
                    }
                    if(tile_count<0){
                        //Here's where things get tricky.
                        uint8_t temp_index = edit_arrangement.tiles.at(arrangement_index)->id;
                        if(!CHR_pages[edit_arrangement.gfx_page].t[temp_index].shared){
                            unsigned int count = 0;
                            for(l=0;l<edit_arrangement.tiles.size();l++){
                                if(edit_arrangement.tiles.at(l)->id == temp_index) count++;
                            }
                            if(count==1){
                                page.t[temp_index] = quad_NEStile[k];   //Replace the old tile
                                continue;
                            }
                        }
                        for(l=0;l<0x100;l++){
                            if(!page.t[l].shared){
                                for(m=0;m<edit_arrangement.tiles.size();m++){
                                    if(edit_arrangement.tiles.at(m)->id == l) break;
                                }
                                if(m>=edit_arrangement.tiles.size()){
                                    page.t[l] = quad_NEStile[k];   //Replace the unused tile
                                    edit_arrangement.tiles.at(arrangement_index) = &(page.t[l]);
                                    break;
                                }
                            }
                        }
                        if(tile_x == 1 && tile_y == 1){
                            page.t[temp_index] = quad_NEStile[k];   //Replace the old tile
                            continue;
                        }
                    }
                }
            }

        }
    }
    edit_arrangement.modified = true;
    drawCHR();
    updateCHRMask();
    drawArrangement();
    ((undo_paste *)undo_actions[undo_position].data)->new_arrangement = edit_arrangement;
    ((undo_paste *)undo_actions[undo_position].data)->new_page = page;
    undo_position++;
    if(undo_position >= UNDO_SIZE) undo_position -= UNDO_SIZE;
    undo_max = undo_position;
    if(undo_min == undo_max){
        undo_min++;
        if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
    }

}

void importBGdialog::clipboard_changed(){
    clipboard_image = clipboard->image();
    clipboard_item->setPixmap(clipboard->pixmap());
    clipboard_scene.addItem(clipboard_item);
    clipboard_item->setOffset(0,0);
    ui->clipboard_view->fitInView(clipboard_item,Qt::KeepAspectRatio);
}

void importBGdialog::fillTransparent(const QImage & base,QImage & target){
    QRgb * base_line;
    QRgb * target_line;
    unsigned int i,j;
    for(i=0;i<8;i++){
        base_line = (QRgb *)base.scanLine(i);
        target_line = (QRgb *)target.scanLine(i);
        for(j=0;j<8;j++){
            if(qAlpha(target_line[j]) != 0xff){
                target_line[j] = base_line[j];
            }
        }
    }
}

uint32_t importBGdialog::colorDiff(uint8_t pal, QImage * quad_tile[4]){
    unsigned int i,j,k;
    QRgb * edit_line;
    uint32_t color_diff = 0;
    for(i=0;i<4;i++){
        for(j=0;j<8;j++){
            edit_line = (QRgb *)quad_tile[i]->scanLine(j);
            for(k=0;k<8;k++){
                color_diff += pals.p[pal].colorDiff(edit_line[k]);
            }
        }
    }
    return color_diff;
}

NEStile importBGdialog::createNESTile(uint8_t pal_num,QImage * img){
    unsigned int i,j;
    NEStile result;
    for(i=0;i<0x10;i++){
        result.t[i] = 0;
    }
    result.checksum = 0;
    result.id = 0;
    QRgb * edit_line;
    uint8_t color_index;
    for(i=0;i<8;i++){
        edit_line = (QRgb *)img->scanLine(i);
        for(j=0;j<8;j++){
            color_index = pals.p[pal_num].bestColor(edit_line[j]);
            result.t[i] |= (color_index&1)<<(7-j);
            result.t[i+8] |= (((color_index>>1)&1)<<(7-j));
        }
    }
    for(i=0;i<0x10;i++){
        result.checksum += result.t[i];
    }
    return result;
}

void importBGdialog::on_radioButton_clicked()
{
    tile_released();
    stamp_mode = false;
}


void importBGdialog::on_radioButton_2_clicked()
{
    tile_released();
    stamp_mode = true;
}

void importBGdialog::updateCHRMask(){
    QRgb * edit_line;
    uint8_t tile_row;
    uint8_t base_column;
    QColor fill_color;
    unsigned int i,j,k;
    for(i=0;i<0x100;i++){
        page.bg_used[i] = false;
    }
    for(i=0;i<edit_arrangement.tiles.size();i++){
        page.bg_used[edit_arrangement.tiles.at(i)->id] = true;
    }

    for(i=0;i<0x100;i++){
        page.t[i].id = i;
        if(page.t[i].shared || i>=0xf8){
            fill_color = QColor(Qt::red);
        }
        else if(page.bg_used[i]){
            fill_color = QColor(Qt::yellow);
        }
        else{
            fill_color = QColor(Qt::green);
        }
        fill_color.setAlphaF(0.8);
        tile_row = i>>4;
        base_column = i&0xf;
        base_column = (base_column<<3) + (base_column<<1);
        for(j=0;j<8;j++){
            edit_line = (QRgb*)indicators.scanLine((tile_row<<3) + (tile_row<<1) + j);
            for(k=0;k<8;k++){
                edit_line[base_column + k] = fill_color.rgba();
            }
        }
    }
    indicators_pix_item->setPixmap(QPixmap::fromImage(indicators));
}

void importBGdialog::on_pushButton_8_clicked()  //Resize button
{
    tile_released();
    resizeBGdialog resize_dialog(edit_arrangement.metatilesX,edit_arrangement.metatilesY,this);
    int result = resize_dialog.exec();
    if(result){
        while(undo_max!=undo_position){
            switch(undo_actions[undo_max].action_type){
            case 1:
                delete ((undo_draw *)undo_actions[undo_max].data);
                break;
            case 2:
                delete ((undo_paste *)undo_actions[undo_max].data);
                break;
            case 3:
                delete ((undo_palette *)undo_actions[undo_max].data);
                break;
            case 4:
                delete ((undo_resize *)undo_actions[undo_max].data);
                break;
            default:
                break;
            }
            undo_actions[undo_max].data = NULL;
            undo_actions[undo_max].action_type= 0;
            undo_max--;
            if(undo_max<0) undo_max += UNDO_SIZE;
        }
        switch(undo_actions[undo_position].action_type){
        case 1:
            delete ((undo_draw *)undo_actions[undo_max].data);
            break;
        case 2:
            delete ((undo_paste *)undo_actions[undo_max].data);
            break;
        case 3:
            delete ((undo_palette *)undo_actions[undo_max].data);
            break;
        case 4:
            delete ((undo_resize *)undo_actions[undo_max].data);
            break;
        default:
            break;
        }
        undo_actions[undo_position].action_type = 4;
        undo_actions[undo_position].data = new undo_resize;
        ((undo_resize *)undo_actions[undo_position].data)->old_arrangement = edit_arrangement;

        resizeArrangement(resize_dialog.getWidth()<<2,resize_dialog.getHeight()<<1);
        edit_arrangement.modified = true;
        ((undo_resize *)undo_actions[undo_position].data)->new_arrangement = edit_arrangement;
        undo_position++;
        if(undo_position >= UNDO_SIZE) undo_position -= UNDO_SIZE;
        undo_max = undo_position;
        if(undo_min == undo_max){
            undo_min++;
            if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
        }
    }
    updateCHRMask();
    drawArrangement();
}


void importBGdialog::on_pushButton_3_clicked()  //Recommend palettes button
{
    tile_released();
    if(!clipboard_image.width() || !clipboard_image.height()) return;
    unsigned int i,j,k;
    unsigned int color_count[0x40];
    QRgb * image_line;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    uint8_t closest_color;
    unsigned int max_count;
    QColor p[0x40];
    for(i=0;i<0x40;i++){
        p[i] = getColor(i);
        color_count[i] = 0;
    }
    QRgb col;
    int8_t best_matches[13];
    for(i=0;i<clipboard_image.height();i++){
        image_line = (QRgb *)clipboard_image.scanLine(i);
        for(j=0;j<clipboard_image.width();j++){
            col = image_line[j];
            min_diff = 0xffffffff;
            for(k=0;k<0x40;k++){
                total_diff = 0;
                if(p[k].red() >= qRed(col)){
                    total_diff = p[k].red() - qRed(col);
                }
                else{
                    total_diff = qRed(col) - p[k].red();
                }
                if(p[k].green() >= qGreen(col)){
                    total_diff += p[k].green() - qGreen(col);
                }
                else{
                    total_diff += qGreen(col) - p[k].green();
                }
                if(p[k].blue() >= qBlue(col)){
                    total_diff += p[k].blue() - qBlue(col);
                }
                else{
                    total_diff += qBlue(col) - p[k].blue();
                }
                if(total_diff < min_diff){
                    min_diff = total_diff;
                    closest_color = k;
                }
            }
            if(closest_color == 0xD) closest_color = 0xF;
            color_count[closest_color]++;
        }
    }
    for(i=0;i<13;i++){
        best_matches[i] = 0x0F;
        max_count = 0;
        for(j=0;j<0x40;j++){
            if(color_count[j]>max_count){
                for(k=0;k<i;k++){
                    if(best_matches[k] == j) break;
                }
                if(k<i) continue;
                max_count = color_count[j];
                best_matches[i] = j;
            }
        }
        if(!max_count){
            best_matches[i] = -1;
            break;
        }
    }
    QDialog * msg = new QDialog;
    msg->resize(500,100);
    msg->setModal(false);
    QLabel info(QString("Best Colors"),msg);
    info.setGeometry(25,0,150,50);
    QLabel * color_boxes = new QLabel[i];
    QPalette lbl_palette;
    for(j=0;j<i;j++){
        color_boxes[j].setParent(msg);
        color_boxes[j].setText(QString(convertByteToHexString(best_matches[j]).c_str()));
        color_boxes[j].setGeometry(j*30 + 25,50,25,25);
        lbl_palette = color_boxes[j].palette();
        color_boxes[j].setAutoFillBackground(true);
        lbl_palette.setColor(QPalette::Window,getColor(best_matches[j]));
        color_boxes[j].setPalette(lbl_palette);
    }
    msg->exec();
    delete [] color_boxes;
    /*unsigned int i,j,k,l,m;
    unsigned int quadtile_width = clipboard_image.width()>>4;
    unsigned int quadtile_height = clipboard_image.height()>>4;
    unsigned int color_count[0x40];
    QRgb * image_line;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    uint8_t closest_color;
    unsigned int max_count;
    NESpalette temp_palette;
    std::vector<NESpalette> palette_list;
    std::vector<int> palette_count_list;
    QColor p[0x40];
    QRgb col;
    for(i=0;i<0x40;i++){
        p[i] = getColor(i);
        color_count[i] = 0;
    }
    for(i=0;i<quadtile_height;i++){
        for(j=0;j<quadtile_width;j++){
            for(l=0;l<0x10;l++){
                image_line = (QRgb *)clipboard_image.scanLine(l + (i<<4));
                for(m=0;m<0x10;m++){
                    col = image_line[m + (j<<4)];
                    min_diff = 0xffffffff;
                    for(k=0;k<0x40;k++){
                        total_diff = 0;
                        if(p[k].red() >= qRed(col)){
                            total_diff = p[k].red() - qRed(col);
                        }
                        else{
                            total_diff = qRed(col) - p[k].red();
                        }
                        if(p[k].green() >= qGreen(col)){
                            total_diff += p[k].green() - qGreen(col);
                        }
                        else{
                            total_diff += qGreen(col) - p[k].green();
                        }
                        if(p[k].blue() >= qBlue(col)){
                            total_diff += p[k].blue() - qBlue(col);
                        }
                        else{
                            total_diff += qBlue(col) - p[k].blue();
                        }
                        if(total_diff < min_diff){
                            min_diff = total_diff;
                            closest_color = k;
                        }
                    }
                    if(closest_color == 0xD) closest_color = 0xF;
                    color_count[closest_color]++;
                }
            }
            for(l=0;l<4;l++) temp_palette.nes_colors[l] = 0xF;
            for(l=0;l<4;l++){
                max_count = 0;
                closest_color = 0x0F;
                for(k=0;k<0x40;k++){
                    if(color_count[k]>max_count){
                        for(m=0;m<l;m++){
                            if(temp_palette.nes_colors[m] == k) break;
                        }
                        if(m<l) continue;
                        max_count = color_count[k];
                        closest_color = k;
                    }
                }
                temp_palette.nes_colors[l] = closest_color;
            }
            for(l=0;l<palette_list.size();l++){
                if(palette_list.at(l).sameColors(temp_palette)){
                    palette_count_list.at(l)++;
                    break;
                }
            }
            if(l>=palette_list.size()){
                palette_list.push_back(temp_palette);
                palette_count_list.push_back(1);
            }
        }
    }
    palette_set optimized_palettes;
    int best_palette;
    for(i=0;i<4;i++){
        max_count = 0;
        best_palette = 0;
        for(j=0;j<palette_list.size();j++){
            if(palette_count_list.at(j) > max_count){
                for(k=0;k<i;k++){
                    if(optimized_palettes.p[k] == palette_list.at(j)) break;
                }
                if(k<i) continue;
                max_count = palette_count_list.at(j);
                best_palette = j;
            }
        }
        if(!max_count) break;
        optimized_palettes.p[i] = palette_list.at(best_palette);
    }
    j=0;
    for(;i<4;i++){
        optimized_palettes.p[i] = pals.p[j];
        optimized_palettes.p[i].nes_colors[0] = pals.nes_ubg;
    }

    for(i=0;i<0x40;i++) color_count[i] = 0;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            color_count[optimized_palettes.p[i].nes_colors[j]]++;
        }
    }
    max_count = 0;
    for(i=0;i<0x40;i++){
        if(color_count[i] > max_count){
            max_count = color_count[i];
            closest_color = i;
        }
    }
    if(!max_count) return;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            if(optimized_palettes.p[i].nes_colors[j] == closest_color && j){
                k= optimized_palettes.p[i].nes_colors[0];
                optimized_palettes.p[i].nes_colors[0] = closest_color;
                optimized_palettes.p[i].nes_colors[j] = j;
                break;
            }
        }
    }
    optimized_palettes.nes_ubg = closest_color;
    bg_palettes->setPalette(optimized_palettes);
    pals = bg_palettes->getPalette();
    drawCHR();
    drawArrangement();*/

}

void importBGdialog::tile_released(){
    if(tile_edit_progress){
        tile_edit_progress = false;
        undo_position++;
        if(undo_position>=UNDO_SIZE) undo_position -= UNDO_SIZE;
        undo_max = undo_position;
        if(undo_min == undo_max){
            undo_min++;
            if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
        }
    }
}

void importBGdialog::undo_slot(){
    tile_released();
    if(undo_position != undo_min){
        if(tile_edit_progress){
            page.t[((undo_draw *)(undo_actions[undo_position].data))->old_tile.id] = ((undo_draw *)(undo_actions[undo_position].data))->old_tile;
            undo_position--;
            if(undo_position<0) undo_position += UNDO_SIZE;
            drawCHR();
            drawArrangement();
            return;
        }
        undo_position--;
        if(undo_position<0) undo_position += UNDO_SIZE;
        switch(undo_actions[undo_position].action_type){
        case 1: //tile drawing
            page.t[((undo_draw *)(undo_actions[undo_position].data))->old_tile.id] = ((undo_draw *)(undo_actions[undo_position].data))->old_tile;
            break;
        case 2: //paste
            page = ((undo_paste *)undo_actions[undo_position].data)->old_page;
            edit_arrangement = ((undo_paste *)undo_actions[undo_position].data)->old_arrangement;
            updateCHRMask();
            break;
        case 3: //palette change
            edit_arrangement.attrib_bits = ((undo_palette *)undo_actions[undo_position].data)->old_attribs;
            break;
        case 4: //resize
            edit_arrangement = ((undo_resize *)undo_actions[undo_position].data)->old_arrangement;
            resizeArrangement(edit_arrangement.metatilesX<<2,edit_arrangement.metatilesY<<1);
            break;
        default:
            break;
        }
        drawCHR();
        drawArrangement();
    }
}

void importBGdialog::redo_slot(){
    if(undo_position != undo_max){
        if(!tile_edit_progress){
            switch(undo_actions[undo_position].action_type){
            case 1: //tile drawing
                page.t[((undo_draw *)(undo_actions[undo_position].data))->new_tile.id] = ((undo_draw *)(undo_actions[undo_position].data))->new_tile;
                break;
            case 2: //paste
                page = ((undo_paste *)undo_actions[undo_position].data)->new_page;
                edit_arrangement = ((undo_paste *)undo_actions[undo_position].data)->new_arrangement;
                updateCHRMask();
                break;
            case 3: //palette change
                edit_arrangement.attrib_bits = ((undo_palette *)undo_actions[undo_position].data)->new_attribs;
                break;
            case 4: //resize
                edit_arrangement = ((undo_resize *)undo_actions[undo_position].data)->new_arrangement;
                resizeArrangement(edit_arrangement.metatilesX<<2,edit_arrangement.metatilesY<<1);
                break;
            default:
                break;
            }
            drawCHR();
            drawArrangement();
            undo_position++;
            if(undo_position >= UNDO_SIZE) undo_position -= UNDO_SIZE;
        }
        tile_edit_progress = false;
    }
}

void importBGdialog::resizeEvent(QResizeEvent * event){
    ui->horizontalScrollBar->setGeometry(0,height()-25,width()-25,25);
    ui->verticalScrollBar->setGeometry(width()-25,0,25,height());

    QPoint scroll_offset(widget_offsets[0].x() - ui->CHR_view->x(),widget_offsets[0].y() - ui->CHR_view->y());
    int range;
    if((761-height()) > 0){
        range = ui->verticalScrollBar->maximum() - ui->verticalScrollBar->minimum();
        ui->verticalScrollBar->setValue(scroll_offset.y() * range / (761-height()));
    }
    if((809-width()) > 0){
        range = ui->horizontalScrollBar->maximum() - ui->horizontalScrollBar->minimum();
        ui->horizontalScrollBar->setValue(scroll_offset.x() * range / (809-width()));
    }

    if(width() < 809){
        ui->horizontalScrollBar->show();
    }
    else{
        ui->horizontalScrollBar->hide();
        ui->CHR_view->move(widget_offsets[0x00].x(), ui->CHR_view->y());
        ui->buttonBox->move(widget_offsets[0x01].x(), ui->buttonBox->y());
        ui->checkBox->move(widget_offsets[0x02].x(), ui->checkBox->y());
        ui->clipboard_view->move(widget_offsets[0x03].x(), ui->clipboard_view->y());
        ui->comboBox->move(widget_offsets[0x04].x(), ui->comboBox->y());
        bg_palettes->move(widget_offsets[0x05].x(), bg_palettes->pos().y());
        ui->label->move(widget_offsets[0x06].x(), ui->label->y());
        ui->label_2->move(widget_offsets[0x07].x(), ui->label_2->y());
        ui->label_3->move(widget_offsets[0x08].x(), ui->label_3->y());
        ui->label_4->move(widget_offsets[0x09].x(), ui->label_4->y());
        ui->label_5->move(widget_offsets[0x0a].x(), ui->label_5->y());
        ui->label_6->move(widget_offsets[0x0b].x(), ui->label_6->y());
        ui->newBGView->move(widget_offsets[0x0c].x(), ui->newBGView->y());
        ui->pushButton->move(widget_offsets[0x0d].x(), ui->pushButton->y());
        ui->pushButton_10->move(widget_offsets[0x0e].x(), ui->pushButton_10->y());
        ui->pushButton_2->move(widget_offsets[0x0f].x(), ui->pushButton_2->y());
        ui->pushButton_3->move(widget_offsets[0x10].x(), ui->pushButton_3->y());
        ui->pushButton_4->move(widget_offsets[0x11].x(), ui->pushButton_4->y());
        ui->pushButton_5->move(widget_offsets[0x12].x(), ui->pushButton_5->y());
        ui->pushButton_8->move(widget_offsets[0x13].x(), ui->pushButton_8->y());
        ui->pushButton_9->move(widget_offsets[0x14].x(), ui->pushButton_9->y());
        ui->radioButton->move(widget_offsets[0x15].x(), ui->radioButton->y());
        ui->radioButton_2->move(widget_offsets[0x16].x(), ui->radioButton_2->y());
        ui->tile_view->move(widget_offsets[0x17].x(), ui->tile_view->y());

    }
    if(height() < 761){
        ui->verticalScrollBar->show();
    }
    else{
        ui->verticalScrollBar->hide();
        ui->CHR_view->move(ui->CHR_view->x(),widget_offsets[0x00].y());
        ui->buttonBox->move(ui->buttonBox->x(),widget_offsets[0x01].y());
        ui->checkBox->move(ui->checkBox->x(),widget_offsets[0x02].y());
        ui->clipboard_view->move(ui->clipboard_view->x(),widget_offsets[0x03].y());
        ui->comboBox->move(ui->comboBox->x(),widget_offsets[0x04].y());
        bg_palettes->move(bg_palettes->pos().x(),widget_offsets[0x05].y());
        ui->label->move(ui->label->x(),widget_offsets[0x06].y());
        ui->label_2->move(ui->label_2->x(),widget_offsets[0x07].y());
        ui->label_3->move(ui->label_3->x(),widget_offsets[0x08].y());
        ui->label_4->move(ui->label_4->x(),widget_offsets[0x09].y());
        ui->label_5->move(ui->label_5->x(),widget_offsets[0x0a].y());
        ui->label_6->move(ui->label_6->x(),widget_offsets[0x0b].y());
        ui->newBGView->move(ui->newBGView->x(),widget_offsets[0x0c].y());
        ui->pushButton->move(ui->pushButton->x(),widget_offsets[0x0d].y());
        ui->pushButton_10->move(ui->pushButton_10->x(),widget_offsets[0x0e].y());
        ui->pushButton_2->move(ui->pushButton_2->x(),widget_offsets[0x0f].y());
        ui->pushButton_3->move(ui->pushButton_3->x(),widget_offsets[0x10].y());
        ui->pushButton_4->move(ui->pushButton_4->x(),widget_offsets[0x11].y());
        ui->pushButton_5->move(ui->pushButton_5->x(),widget_offsets[0x12].y());
        ui->pushButton_8->move(ui->pushButton_8->x(),widget_offsets[0x13].y());
        ui->pushButton_9->move(ui->pushButton_9->x(),widget_offsets[0x14].y());
        ui->radioButton->move(ui->radioButton->x(),widget_offsets[0x15].y());
        ui->radioButton_2->move(ui->radioButton_2->x(),widget_offsets[0x16].y());
        ui->tile_view->move(ui->tile_view->x(),widget_offsets[0x17].y());
    }
    QDialog::resizeEvent(event);
}

void importBGdialog::verticalScroll(int position){
    int range = ui->verticalScrollBar->maximum() - ui->verticalScrollBar->minimum();
    float fraction = (float)position/range;
    int offset = fraction * (761-height());

    ui->CHR_view->move(ui->CHR_view->x(),widget_offsets[0x00].y() - offset);
    ui->buttonBox->move(ui->buttonBox->x(),widget_offsets[0x01].y() - offset);
    ui->checkBox->move(ui->checkBox->x(),widget_offsets[0x02].y() - offset);
    ui->clipboard_view->move(ui->clipboard_view->x(),widget_offsets[0x03].y() - offset);
    ui->comboBox->move(ui->comboBox->x(),widget_offsets[0x04].y() - offset);
    bg_palettes->move(bg_palettes->pos().x(),widget_offsets[0x05].y() - offset);
    ui->label->move(ui->label->x(),widget_offsets[0x06].y() - offset);
    ui->label_2->move(ui->label_2->x(),widget_offsets[0x07].y() - offset);
    ui->label_3->move(ui->label_3->x(),widget_offsets[0x08].y() - offset);
    ui->label_4->move(ui->label_4->x(),widget_offsets[0x09].y() - offset);
    ui->label_5->move(ui->label_5->x(),widget_offsets[0x0a].y() - offset);
    ui->label_6->move(ui->label_6->x(),widget_offsets[0x0b].y() - offset);
    ui->newBGView->move(ui->newBGView->x(),widget_offsets[0x0c].y() - offset);
    ui->pushButton->move(ui->pushButton->x(),widget_offsets[0x0d].y() - offset);
    ui->pushButton_10->move(ui->pushButton_10->x(),widget_offsets[0x0e].y() - offset);
    ui->pushButton_2->move(ui->pushButton_2->x(),widget_offsets[0x0f].y() - offset);
    ui->pushButton_3->move(ui->pushButton_3->x(),widget_offsets[0x10].y() - offset);
    ui->pushButton_4->move(ui->pushButton_4->x(),widget_offsets[0x11].y() - offset);
    ui->pushButton_5->move(ui->pushButton_5->x(),widget_offsets[0x12].y() - offset);
    ui->pushButton_8->move(ui->pushButton_8->x(),widget_offsets[0x13].y() - offset);
    ui->pushButton_9->move(ui->pushButton_9->x(),widget_offsets[0x14].y() - offset);
    ui->radioButton->move(ui->radioButton->x(),widget_offsets[0x15].y() - offset);
    ui->radioButton_2->move(ui->radioButton_2->x(),widget_offsets[0x16].y() - offset);
    ui->tile_view->move(ui->tile_view->x(),widget_offsets[0x17].y() - offset);
}

void importBGdialog::horizontalScroll(int position){
    int range = ui->horizontalScrollBar->maximum() - ui->horizontalScrollBar->minimum();
    float fraction = (float)position/range;
    int offset = fraction * (809-width());

    ui->CHR_view->move(widget_offsets[0x00].x() - offset, ui->CHR_view->y());
    ui->buttonBox->move(widget_offsets[0x01].x() - offset, ui->buttonBox->y());
    ui->checkBox->move(widget_offsets[0x02].x() - offset, ui->checkBox->y());
    ui->clipboard_view->move(widget_offsets[0x03].x() - offset, ui->clipboard_view->y());
    ui->comboBox->move(widget_offsets[0x04].x() - offset, ui->comboBox->y());
    bg_palettes->move(widget_offsets[0x05].x() - offset, bg_palettes->pos().y());
    ui->label->move(widget_offsets[0x06].x() - offset, ui->label->y());
    ui->label_2->move(widget_offsets[0x07].x() - offset, ui->label_2->y());
    ui->label_3->move(widget_offsets[0x08].x() - offset, ui->label_3->y());
    ui->label_4->move(widget_offsets[0x09].x() - offset, ui->label_4->y());
    ui->label_5->move(widget_offsets[0x0a].x() - offset, ui->label_5->y());
    ui->label_6->move(widget_offsets[0x0b].x() - offset, ui->label_6->y());
    ui->newBGView->move(widget_offsets[0x0c].x() - offset, ui->newBGView->y());
    ui->pushButton->move(widget_offsets[0x0d].x() - offset, ui->pushButton->y());
    ui->pushButton_10->move(widget_offsets[0x0e].x() - offset, ui->pushButton_10->y());
    ui->pushButton_2->move(widget_offsets[0x0f].x() - offset, ui->pushButton_2->y());
    ui->pushButton_3->move(widget_offsets[0x10].x() - offset, ui->pushButton_3->y());
    ui->pushButton_4->move(widget_offsets[0x11].x() - offset, ui->pushButton_4->y());
    ui->pushButton_5->move(widget_offsets[0x12].x() - offset, ui->pushButton_5->y());
    ui->pushButton_8->move(widget_offsets[0x13].x() - offset, ui->pushButton_8->y());
    ui->pushButton_9->move(widget_offsets[0x14].x() - offset, ui->pushButton_9->y());
    ui->radioButton->move(widget_offsets[0x15].x() - offset, ui->radioButton->y());
    ui->radioButton_2->move(widget_offsets[0x16].x() - offset, ui->radioButton_2->y());
    ui->tile_view->move(widget_offsets[0x17].x() - offset, ui->tile_view->y());

}

void importBGdialog::showContextMenu(const QPoint & pos){
    QPoint global_pos = ui->newBGView->mapToGlobal(pos);

    QMenu context_menu;
    context_menu.addAction("Copy (Ctrl + C)",this,SLOT(copy_slot()));
    context_menu.addAction("Paste (Ctrl + V)",this,SLOT(paste_slot()));
    context_menu.addAction("Duplicate Entire Image",this,SLOT(duplicateImage()));
    QAction * selected_item = context_menu.exec(global_pos);
    if(selected_item){

    }
    else{
        //nothing was chosen
    }
}

void importBGdialog::duplicateImage(){
    ui->comboBox->setCurrentIndex(num_bgs);
    edit_arrangement.modified = true;
}
