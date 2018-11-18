#include "editspritedialog.h"
#include "ui_editspritedialog.h"
#include <QMessageBox>
#include <QColor>

editSpriteDialog::editSpriteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editSpriteDialog)
{
    ui->setupUi(this);
    arrangement_view = new spriteView(this);
    arrangement_view->setGeometry(10,50,541,352);
    arrangement_view->setMouseTracking(true);
    ui->label_12->setGeometry(552,50,20,352);
    ui->label_12->setPixmap(QPixmap()); //initialize row usage indicators to empty pixmap
    sprite_page = new CHR_page;
    bg_page = &backup_page;
    to_paste = NULL;
    internal_copy = false;
    unsigned int i,j,k;
    draw_mode = false;
    paste_ready = false;
    edit_progress = false;
    edit_arrangement.modified = false;
    setWindowTitle(QString("Sprite Editor"));
    bg_indicators_pix_item = NULL;
    sprite_indicators_pix_item = NULL;
    palette_set default_palette;
    default_palette.nes_ubg = 0x0f;
    for(i=0;i<4;i++){
        for(j=1;j<4;j++){
            default_palette.p[i].nes_colors[j] = 0x0f;
        }
    }
    sprite_palettes = new palette_set_selector(this,10,540,default_palette);
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);
    arrangement_view->setDragMode(QGraphicsView::RubberBandDrag);
    connect(sprite_palettes,SIGNAL (color_changed(uint8_t,uint8_t)),this,SLOT (on_color_selected(uint8_t,uint8_t)));
    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(false);
    arrangement_background = QImage(256,240,QImage::Format_ARGB32);
    overlay_item = new spriteEditItem;
    new_scene.addItem(overlay_item);
    overlay_item->setOffset(0,0);

    //***********************************
    //***********************************
    //Make sure to connect new graphics view to my arrangement_clicked slot here
    connect(arrangement_view,SIGNAL(tile_pressed(QMouseEvent *)),this,SLOT(arrangement_clicked(QMouseEvent *)));
    connect(arrangement_view,SIGNAL(tile_released()),this,SLOT(arrangement_released()));
    connect(arrangement_view,SIGNAL(hover_event(QMouseEvent*)),this,SLOT(paste_position(QMouseEvent*)));
    //connect(overlay_item,SIGNAL(tile_pressed(QGraphicsSceneMouseEvent *)),this,SLOT(arrangement_clicked(QGraphicsSceneMouseEvent*)));
    //connect(overlay_item,SIGNAL(tile_released()),this,SLOT(arrangement_released()));
    uint16_t max_sprite_id = 0;
    for(i=0;i<num_sprites;i++){
        if(sprites[i]->id > max_sprite_id) max_sprite_id = sprites[i]->id;
    }
    k=0;
    for(i=0;i<=max_sprite_id;i++){
        for(j=0;j<num_sprites;j++){
            if(sprites[j]->id == i){
                sorted_list[k++] = sprites[j];
                ui->comboBox->addItem(QString(convertByteToHexString(sprites[j]->id).c_str()));
                break;
            }
        }
    }
    ui->comboBox->addItem(QString("New"));
    k=0;
    uint16_t max_bg_id = 0;
    for(i=0;i<num_bgs;i++){
        if(bgs[i]->id > max_bg_id) max_bg_id = bgs[i]->id;
    }
    for(i=0;i<=max_bg_id;i++){
        for(j=0;j<num_bgs;j++){
            if(bgs[j]->id == i){
                sorted_bgs[k++] = bgs[j];
                ui->comboBox_2->addItem(QString(convertByteToHexString(bgs[j]->id).c_str()));
                break;
            }
        }
    }
    ui->comboBox_2->addItem(QString("None"));
    ui->comboBox_2->setCurrentIndex(num_bgs);
    ui->comboBox_3->setCurrentIndex(0);
    arrangement_view->setScene(&new_scene);
    if(ui->comboBox->count() > 1){
        sprite_palettes->setPalette(sorted_list[0]->bestPalette());
    }
    selected_palette = 0;
    selected_color = 0;
    selected_tile = 0xfA;
    bg_indicators = QImage(176,176,QImage::Format_ARGB32);
    bg_indicators.fill(Qt::gray);
    sprite_indicators = QImage(176,176,QImage::Format_ARGB32);
    sprite_indicators.fill(Qt::gray);
    for(i=0;i<0x100;i++){
        bg_chr_array[i] = new chrItem();
        bg_chr_array[i]->setID(i);
        connect(bg_chr_array[i],SIGNAL(chr_pressed()),this,SLOT(bg_chr_selection_changed()));
        connect(bg_chr_array[i],SIGNAL(chr_selected()),this,SLOT(bg_chr_selection_noclick()));
        bg_chr_array[i]->setOffset((i&0xf)*10,(i>>4)*10);
        bg_CHR_scene.addItem(bg_chr_array[i]);
    }
    for(i=0;i<0x100;i++){
        sprite_chr_array[i] = new chrItem();
        sprite_chr_array[i]->setID(i);
        connect(sprite_chr_array[i],SIGNAL(chr_pressed()),this,SLOT(sprite_chr_selection_changed()));
        connect(sprite_chr_array[i],SIGNAL(chr_selected()),this,SLOT(sprite_chr_selection_noclick()));
        sprite_chr_array[i]->setOffset((i&0xf)*10,(i>>4)*10);
        sprite_CHR_scene.addItem(sprite_chr_array[i]);
    }
    ui->BG_chr_view->setScene(&bg_CHR_scene);
    ui->sprite_CHR_view->setScene(&sprite_CHR_scene);
    bg_indicators_pix_item = new QGraphicsPixmapItem;
    sprite_indicators_pix_item = new QGraphicsPixmapItem;
    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
    on_checkBox_toggled(false);
    on_checkBox_2_toggled(false);
    bg_indicators_pix_item->setOffset(0,0);
    sprite_indicators_pix_item->setOffset(0,0);
    tile_selector = QImage(22,12,QImage::Format_ARGB32);
    QRgb * edit_line;
    for(i=0;i<12;i++){
        edit_line = (QRgb *) tile_selector.scanLine(i);
        for(j=0;j<22;j++){
            if(i<2 || i>=10){
                edit_line[j] = QColor(Qt::black).rgba();
            }
            else{
                if(j<2 || j>=20){
                    edit_line[j] = QColor(Qt::black).rgba();
                }
                else{
                    edit_line[j] = QColor(Qt::transparent).rgba();
                }
            }
        }
    }
    tile_selector_pix_item = new QGraphicsPixmapItem(QPixmap::fromImage(tile_selector));
    sprite_tile_selected = true;
    sprite_CHR_scene.addItem(tile_selector_pix_item);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    arrangement_tile = -1;

    uint16_t temp_base = 0x2000;
    k=0;
    std::string base_string = "";
    for(i=0;i<15;i++){
        for(j=0;j<8;j++){
            ppu_bases_possible[k] = temp_base;
            base_string = convertByteToHexString(ppu_bases_possible[k]>>8);
            base_string += convertByteToHexString(ppu_bases_possible[k]);
            k++;
            ui->comboBox_3->addItem(QString(base_string.c_str()));
            temp_base +=4;
        }
        temp_base += 0x20;
    }
    ui->clipboard_view->setScene(&clipboard_scene);
    last_focus = -1;
    cut_shortcut = new QShortcut(QKeySequence(QString("Ctrl+X")),this);
    copy_shortcut = new QShortcut(QKeySequence(QString("Ctrl+C")),this);
    paste_shortcut = new QShortcut(QKeySequence(QString("Ctrl+V")),this);
    undo_shortcut = new QShortcut(QKeySequence(QString("Ctrl+Z")),this);
    redo_shortcut = new QShortcut(QKeySequence(QString("Ctrl+Shift+Z")),this);
    delete_shortcut = new QShortcut(QKeySequence(QString("Delete")),this);
    connect(cut_shortcut,SIGNAL(activated()),this,SLOT(cut_slot()));
    connect(copy_shortcut,SIGNAL(activated()),this,SLOT(copy_slot()));
    connect(paste_shortcut,SIGNAL(activated()),this,SLOT(paste_slot()));
    connect(undo_shortcut,SIGNAL(activated()),this,SLOT(undo_slot()));
    connect(redo_shortcut,SIGNAL(activated()),this,SLOT(redo_slot()));
    connect(delete_shortcut,SIGNAL(activated()),this,SLOT(delete_slot()));
    clipboard = QApplication::clipboard();
    clipboard_image = clipboard->image();
    clipboard_item = new QGraphicsPixmapItem(clipboard->pixmap());
    clipboard_scene.addItem(clipboard_item);
    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(clipboard_changed()));
    ui->spinBox->setRange(0,0x7F);
    ui->spinBox->setValue(0x40);
    ui->spinBox_2->setRange(0,0x7F);
    ui->spinBox_2->setValue(0x40);
    ui->verticalSlider->setRange(0,255);
    ui->verticalSlider->setValue(50);
    undo_min = 0;
    undo_max = 0;
    undo_position = 0;
    copy_tiles.clear();
    connect(ui->horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScroll(int)));
    connect(ui->verticalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(verticalScroll(int)));
    connect(arrangement_view->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(arrangement_viewport_changed()));
    connect(arrangement_view->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(arrangement_viewport_changed()));
    ui->horizontalScrollBar->raise();
    ui->verticalScrollBar->raise();
    widget_list.clear();
    arrangement_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(arrangement_view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    QObjectList child_list = children();
    for(i=0;i<child_list.size();i++){
        QWidget * temp_widget = qobject_cast<QWidget*>(child_list.at(i));
        if(temp_widget){
            if(temp_widget == ui->verticalScrollBar || temp_widget == ui->horizontalScrollBar) continue;
            widget_list.append(temp_widget);
        }
    }
    widget_offsets = new QPoint[widget_list.size()];
    for(i=0;i<widget_list.size();i++){
        widget_offsets[i] = ((QWidget *)widget_list[i])->pos();
    }
    updateCHRMask();
    drawCHR();
    drawBackground();
    drawArrangement();
}

editSpriteDialog::~editSpriteDialog()
{
    delete ui;
    delete arrangement_view;
    delete sprite_page;
}


void editSpriteDialog::on_comboBox_currentIndexChanged(int index)
{
    unsigned int i,j;
    uint8_t tiles_in_column;
    uint8_t tile_count = 0;
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
            for(i=0;i<edit_arrangement.arrangement.length();){
                if(i&0x80) break;
                i++;
                tiles_in_column = edit_arrangement.arrangement[i++];
                for(j=0;j<tiles_in_column;j++){
                    if(edit_arrangement.arrangement[i]&1){
                        CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(tile_count)->id] = sprite_page->t[edit_arrangement.tiles.at(tile_count)->id];
                        CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(tile_count+1)->id] = sprite_page->t[edit_arrangement.tiles.at(tile_count+1)->id];
                        edit_arrangement.tiles.at(tile_count) = &(CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(tile_count)->id]);
                        edit_arrangement.tiles.at(tile_count+1) = &(CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(tile_count+1)->id]);
                        tile_count+=2;
                        i+=2;
                    }
                    else{
                        CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at(tile_count)->id] = bg_page->t[edit_arrangement.tiles.at(tile_count)->id];
                        CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at(tile_count+1)->id] = bg_page->t[edit_arrangement.tiles.at(tile_count+1)->id];
                        edit_arrangement.tiles.at(tile_count) = &(CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at(tile_count)->id]);
                        edit_arrangement.tiles.at(tile_count+1) = &(CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at(tile_count+1)->id]);
                        tile_count+=2;
                        i+=2;
                    }
                }
            }

            if(old_combo_index < (ui->comboBox->count() - 1)){
                *sorted_list[old_combo_index] = edit_arrangement;
                sorted_list[old_combo_index]->modified = false;
                //sorted_list[old_combo_index]->generateImage();    ****can't generate image without pointer to start of rom
            }
            else{
                sprites[num_sprites] = new sprite;
                *sprites[num_sprites] = edit_arrangement;
                sprites[num_sprites]->references = 0;
                sprites[num_sprites]->id = num_sprites;
                sprites[num_sprites]->modified = false;
                //sprites[num_sprites]->generateImage();  ****can't generate image without pointer to start of the rom.
                num_sprites++;
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
    if(index < (ui->comboBox->count() - 1)){
        sprite_palettes->setPalette(sorted_list[index]->bestPalette());
        pals = sprite_palettes->getPalette();

        for(i=0;i<num_chr_pages;i++){
            for(j=0;j<0x100;j++) CHR_pages[i].t[j].shared = false;
        }
        for(i=0;i<num_bgs;i++){
            for(j=0;j<bgs[i]->tiles.size();j++) bgs[i]->tiles.at(j)->shared = true;
        }
        for(i=0;i<num_sprites;i++){
            if(index == i) continue;
            for(j=0;j<sorted_list[i]->tiles.size();j++) sorted_list[i]->tiles.at(j)->shared = true;
        }
        for(i=0;i<num_ppu_strings;i++){
            for(j=0;j<ppu_strings[i]->tiles.size();j++) ppu_strings[i]->tiles.at(j)->shared = true;
        }
        edit_arrangement = *sorted_list[index];
        *sprite_page = CHR_pages[sorted_list[index]->gfx_page];
        if(sorted_list[index]->gfx_page == sorted_list[index]->bg_gfx_page){
            bg_page = sprite_page;
        }
        else{
            bg_page = &backup_page;
            *bg_page = CHR_pages[sorted_list[index]->bg_gfx_page];
        }

        reparseImage();

        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();

    }
    else{
        ui->label_4->setPixmap(QPixmap());
    }
    old_combo_index = index;
    undo_min = 0;
    undo_max = 0;
    undo_position = 0;

}

void editSpriteDialog::on_checkBox_toggled(bool checked)    //bg indicators
{
    bg_indicators_pix_item->setPixmap(QPixmap::fromImage(bg_indicators));
    if(checked){
        bg_CHR_scene.addItem(bg_indicators_pix_item);
        bg_indicators_pix_item->setOffset(0,0);
        bg_indicators_pix_item->setAcceptedMouseButtons(0);    //Don't allow interaction with the indicator mask
    }
    else{
        bg_CHR_scene.removeItem(bg_indicators_pix_item);
    }

}

void editSpriteDialog::on_checkBox_2_toggled(bool checked)  //sprite indicators
{
    sprite_indicators_pix_item->setPixmap(QPixmap::fromImage(sprite_indicators));
    if(checked){
        sprite_CHR_scene.addItem(sprite_indicators_pix_item);
        sprite_indicators_pix_item->setOffset(0,0);
        sprite_indicators_pix_item->setAcceptedMouseButtons(0);    //Don't allow interaction with the indicator mask
    }
    else{
        sprite_CHR_scene.removeItem(sprite_indicators_pix_item);
    }
}

void editSpriteDialog::bg_chr_selection_changed(){
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    selected_tile &= 0xFE;
    bg_CHR_scene.addItem(tile_selector_pix_item);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    last_focus = 2;
    sprite_tile_selected = false;
}

void editSpriteDialog::sprite_chr_selection_changed(){
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    selected_tile &=0xFE;
    sprite_CHR_scene.addItem(tile_selector_pix_item);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    last_focus = 2;
    sprite_tile_selected = true;
}

void editSpriteDialog::bg_chr_selection_noclick(){
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    selected_tile &= 0xFE;
    bg_CHR_scene.addItem(tile_selector_pix_item);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    sprite_tile_selected = false;
}

void editSpriteDialog::sprite_chr_selection_noclick(){
    chrItem * selection = qobject_cast<chrItem *>(sender());
    selected_tile = selection->getID();
    selected_tile &=0xFE;
    sprite_CHR_scene.addItem(tile_selector_pix_item);
    tile_selector_pix_item->setOffset((selected_tile&0xf)*10 - 2,(selected_tile>>4)*10 - 2);
    sprite_tile_selected = true;
}

void editSpriteDialog::copy_slot(){
    int i,j,k;
    QRgb * get_scanline;
    QRgb * put_scanline;
    if(last_focus == 2){
        internal_copy = true;
        copy_tiles.clear();
        copy_data to_copy;
        to_copy.x = 0;
        to_copy.y = 0;
        to_copy.z_value = 0;
        to_copy.attribs = selected_palette;
        to_copy.flip = false;
        clipboard_image = QImage(8,16,QImage::Format_ARGB32);
        if(sprite_tile_selected){
            to_copy.tile_type = true;
            to_copy.tile[0] = sprite_page->t[selected_tile&0xFE];
            to_copy.tile[1] = sprite_page->t[selected_tile|0x01];
            to_copy.tile[0].id = selected_tile&0xFE;
            for(i=0;i<8;i++){
                get_scanline = (QRgb *)sprite_tiles[selected_palette][selected_tile&0xfe].scanLine(i);
                put_scanline = (QRgb *)clipboard_image.scanLine(i);
                for(j=0;j<8;j++){
                    put_scanline[j] = get_scanline[j];
                }
            }
            for(i=0;i<8;i++){
                get_scanline = (QRgb *)sprite_tiles[selected_palette][selected_tile|1].scanLine(i);
                put_scanline = (QRgb *)clipboard_image.scanLine(i+8);
                for(j=0;j<8;j++){
                    put_scanline[j] = get_scanline[j];
                }
            }
        }
        else{
            to_copy.tile_type = false;
            to_copy.tile[0] = bg_page->t[selected_tile&0xFE];
            to_copy.tile[1] = bg_page->t[selected_tile|0x01];
            to_copy.tile[0].id = selected_tile&0xFE;
            for(i=0;i<8;i++){
                get_scanline = (QRgb *)bg_tiles[selected_palette][selected_tile&0xfe].scanLine(i);
                put_scanline = (QRgb *)clipboard_image.scanLine(i);
                for(j=0;j<8;j++){
                    put_scanline[j] = get_scanline[j];
                }
            }
            for(i=0;i<8;i++){
                get_scanline = (QRgb *)bg_tiles[selected_palette][selected_tile|1].scanLine(i);
                put_scanline = (QRgb *)clipboard_image.scanLine(i+8);
                for(j=0;j<8;j++){
                    put_scanline[j] = get_scanline[j];
                }
            }
        }
        copy_tiles.push_back(to_copy);
    }
    else if(last_focus == 1){
        internal_copy = true;
        uint16_t min_x,max_x,min_y,max_y;
        spriteEditItem * current_tile;
        min_x = 0xffff;
        min_y = 0xffff;
        max_x = 0;
        max_y = 0;
        if(!image.size()) return;
        for(i=0;i<image.size();i++){
            current_tile = image.at(i);
            if(current_tile->isSelected()){
                if(current_tile->offset().x() < min_x){
                    min_x = current_tile->offset().x();
                }
                if(current_tile->offset().y() < min_y){
                    min_y = current_tile->offset().y();
                }
                if(current_tile->offset().x() > max_x){
                    max_x = current_tile->offset().x();
                }
                if(current_tile->offset().y() > max_y){
                    max_y = current_tile->offset().y();
                }
            }
        }
        max_x += 8;
        max_y += 16;
        QImage from_arrangement(max_x - min_x,max_y - min_y,QImage::Format_ARGB32);
        uint8_t color_index;
        NEStile * current_nestile;
        from_arrangement.fill(Qt::transparent);
        for(i=image.size()-1;i>=0;i--){
            current_tile = image.at(i);
            uint16_t tile_x = current_tile->offset().x() - min_x;
            uint16_t tile_y = current_tile->offset().y() - min_y;
            if(current_tile->isSelected()){
                if(current_tile->getTileType()){
                    current_nestile = &sprite_page->t[current_tile->getTileID()&0xfe];
                }
                else{
                    current_nestile = &bg_page->t[current_tile->getTileID()&0xfe];
                }
                for(j=0;j<8;j++){
                    put_scanline = (QRgb *)from_arrangement.scanLine(tile_y + j);
                    for(k=0;k<8;k++){
                        color_index = 0;
                        color_index |= (current_nestile->t[j] >> (7-k))&0x01;
                        color_index |= ((current_nestile->t[j+8] >> (7-k))&0x01)<<1;
                        if(!color_index) continue;
                        if(current_tile->getFlip()){
                            put_scanline[tile_x + 7 - k] = pals.p[current_tile->getAttribs()].p[color_index].rgb();
                        }
                        else{
                            put_scanline[tile_x + k] = pals.p[current_tile->getAttribs()].p[color_index].rgb();
                        }
                    }
                }
                if(current_tile->getTileType()){
                    current_nestile = &sprite_page->t[current_tile->getTileID() | 0x01];
                }
                else{
                    current_nestile = &bg_page->t[current_tile->getTileID() | 0x01];
                }
                for(j=0;j<8;j++){
                    put_scanline = (QRgb *)from_arrangement.scanLine(tile_y + j + 8);
                    for(k=0;k<8;k++){
                        color_index = 0;
                        color_index |= (current_nestile->t[j] >> (7-k))&0x01;
                        color_index |= ((current_nestile->t[j+8] >> (7-k))&0x01)<<1;
                        if(!color_index) continue;
                        if(current_tile->getFlip()){
                            put_scanline[tile_x + 7 - k] = pals.p[current_tile->getAttribs()].p[color_index].rgb();
                        }
                        else{
                            put_scanline[tile_x + k] = pals.p[current_tile->getAttribs()].p[color_index].rgb();
                        }
                    }
                }
            }
        }
        clipboard_image = from_arrangement;
        copy_tiles.clear();
        copy_data to_copy;
        uint8_t num_selected = 0;
        for(i=0;i<image.size();i++){
            if(image.at(i)->isSelected()){
                to_copy.x = image.at(i)->offset().x() - min_x;
                to_copy.y = image.at(i)->offset().y() - min_y;
                to_copy.z_value = num_selected++;
                to_copy.tile_type = image.at(i)->getTileType();
                to_copy.tile[0] = (to_copy.tile_type) ? sprite_page->t[image.at(i)->getTileID()&0xFE] : bg_page->t[image.at(i)->getTileID()&0xFE];
                to_copy.tile[1] = (to_copy.tile_type) ? sprite_page->t[image.at(i)->getTileID()|0x01] : bg_page->t[image.at(i)->getTileID()|0x01];
                to_copy.attribs = image.at(i)->getAttribs();
                to_copy.flip = image.at(i)->getFlip();
                copy_tiles.push_back(to_copy);
            }
        }
    }
    clipboard->setImage(clipboard_image);
    return;
}

void editSpriteDialog::paste_slot(){
    int i,j;
    QRgb * edit_line;
    paste_ready = true;
    QColor pixel_color;
    QImage image_to_paste = clipboard_image;
    for(i=0;i<image_to_paste.height();i++){
        edit_line = (QRgb *) image_to_paste.scanLine(i);
        for(j=0;j<image_to_paste.width();j++){
            pixel_color = QColor::fromRgba(edit_line[j]);
            if(pixel_color.alpha()!=0xff) continue;
            pixel_color.setAlpha(200);
            edit_line[j] = pixel_color.rgba();
        }
    }
    to_paste = new QGraphicsPixmapItem(QPixmap::fromImage(image_to_paste));
    new_scene.addItem(to_paste);
    to_paste->setOffset(uint8_t(spritexy_values[0]+0x80),uint8_t(spritexy_values[0]+0x80));
    to_paste->setZValue(0xFF);
}

void editSpriteDialog::undo_slot(){
    arrangement_released();
    if(paste_ready){
        new_scene.removeItem(to_paste);
        paste_ready = false;
        return;
    }
    if(undo_position != undo_min){
        if(edit_progress){
            *bg_page = undo_actions[undo_position].old_bg;
            *sprite_page = undo_actions[undo_position].old_sprites;
            edit_arrangement = undo_actions[undo_position].old_arrangement;
            undo_position--;
            if(undo_position<0) undo_position += UNDO_SIZE;
            reparseImage();
            updateCHRMask();
            drawCHR();
            drawBackground();
            drawArrangement();
            return;
        }
        undo_position--;
        if(undo_position<0) undo_position += UNDO_SIZE;
        *bg_page = undo_actions[undo_position].old_bg;
        *sprite_page = undo_actions[undo_position].old_sprites;
        edit_arrangement = undo_actions[undo_position].old_arrangement;
        reparseImage();
        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();
    }

}

void editSpriteDialog::redo_slot(){
    if(undo_position != undo_max){
        if(!edit_progress){
            *bg_page = undo_actions[undo_position].new_bg;
            *sprite_page = undo_actions[undo_position].new_sprites;
            edit_arrangement = undo_actions[undo_position].new_arrangement;
            reparseImage();
            updateCHRMask();
            drawCHR();
            drawBackground();
            drawArrangement();
            undo_position++;
            if(undo_position >= UNDO_SIZE) undo_position -= UNDO_SIZE;
        }
        edit_progress = false;
    }
}

void editSpriteDialog::clipboard_changed(){
    clipboard_image = clipboard->image();
    clipboard_item->setPixmap(clipboard->pixmap());
    clipboard_scene.addItem(clipboard_item);
    clipboard_item->setOffset(0,0);
    ui->clipboard_view->fitInView(clipboard_item,Qt::KeepAspectRatio);
    if(!internal_copy) copy_tiles.clear();
    internal_copy = false;
}

void editSpriteDialog::horizontalScroll(int position){
    int i;
    int range = ui->horizontalScrollBar->maximum() - ui->horizontalScrollBar->minimum();
    float fraction = (float)position/range;
    int offset = fraction * (805-width());

    for(i=0;i<widget_list.size();i++){
        widget_list[i]->move(widget_offsets[i].x() - offset,widget_list[i]->y());
    }
}

void editSpriteDialog::verticalScroll(int position){
    int i;
    int range = ui->verticalScrollBar->maximum() - ui->verticalScrollBar->minimum();
    float fraction = (float)position/range;
    int offset = fraction * (711-height());

    for(i=0;i<widget_list.size();i++){
        widget_list[i]->move(widget_list[i]->x(),widget_offsets[i].y() - offset);
    }
}

void editSpriteDialog::updateCHRMask(){
    QRgb * edit_line;
    uint8_t tile_row;
    uint8_t base_column;
    uint8_t tile_count = 0;
    uint8_t tiles_in_column;
    QColor fill_color;
    unsigned int i,j,k;
    for(i=0;i<0x100;i++){
        bg_page->sprite_used[i] = false;
        sprite_page->sprite_used[i] = false;
    }
    for(i=0;i<edit_arrangement.arrangement.length();){
        if(edit_arrangement.arrangement[i++]&0x80) break;
        tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            if(edit_arrangement.arrangement[i]&1){
                sprite_page->sprite_used[edit_arrangement.tiles.at(tile_count)->id] = true;
                sprite_page->sprite_used[edit_arrangement.tiles.at(tile_count+1)->id] = true;
            }
            else{
                bg_page->sprite_used[edit_arrangement.tiles.at(tile_count)->id] = true;
                bg_page->sprite_used[edit_arrangement.tiles.at(tile_count+1)->id] = true;
            }
            tile_count+=2;
            i+=2;
        }
    }

    for(i=0;i<0x100;i++){
        bg_page->t[i].id = i;
        if(bg_page->t[i].shared || i>=0xf8){
            fill_color = QColor(Qt::red);
        }
        else if(bg_page->sprite_used[i]){
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
            edit_line = (QRgb*)bg_indicators.scanLine((tile_row<<3) + (tile_row<<1) + j);
            for(k=0;k<8;k++){
                edit_line[base_column + k] = fill_color.rgba();
            }
        }
    }
    bg_indicators_pix_item->setPixmap(QPixmap::fromImage(bg_indicators));

    for(i=0;i<0x100;i++){
        sprite_page->t[i].id = i;
        if(sprite_page->t[i].shared || i>=0xf8){
            fill_color = QColor(Qt::red);
        }
        else if(sprite_page->sprite_used[i]){
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
            edit_line = (QRgb*)sprite_indicators.scanLine((tile_row<<3) + (tile_row<<1) + j);
            for(k=0;k<8;k++){
                edit_line[base_column + k] = fill_color.rgba();
            }
        }
    }
    sprite_indicators_pix_item->setPixmap(QPixmap::fromImage(sprite_indicators));

}

void editSpriteDialog::drawArrangement(){
    unsigned int i,j,k,l;
    QImage edit_tile(8,16,QImage::Format_ARGB32);
    QRgb * edit_line;
    uint8_t color_index;
    uint8_t attribs;
    uint8_t tiles_in_column;
    uint8_t tile_count = 0;
    uint8_t x,y;
    bool hflip;

    for(i=0;i<edit_arrangement.arrangement.length();){
        if(edit_arrangement.arrangement[i]&0x80) break;
        x = edit_arrangement.arrangement[i++];
        if(x>=0x29) x=0;
        x = (uint8_t)(spritexy_values[x] + 0x80);
        tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            hflip = edit_arrangement.arrangement[i+1]&1;
            attribs = (edit_arrangement.arrangement[i+1]&0x6)>>1;
            y = (uint8_t)(0x80 + spritexy_values[((uint8_t)edit_arrangement.arrangement[i+1])>>3]);
            if(hflip){
                for(k=0;k<8;k++){
                    edit_line = (QRgb*) edit_tile.scanLine(k);
                    for(l=0;l<8;l++){
                        color_index = 0;
                        color_index |= ((edit_arrangement.tiles.at(tile_count)->t[k]) >> (7-l))&0x01;
                        color_index |= (((edit_arrangement.tiles.at(tile_count)->t[k+8]) >> (7-l))&0x01)<<1;
                        edit_line[7-l] = (color_index) ? pals.p[attribs].p[color_index].rgb() : Qt::transparent;
                    }
                }
                for(k=0;k<8;k++){
                    edit_line = (QRgb*) edit_tile.scanLine(k+8);
                    for(l=0;l<8;l++){
                        color_index = 0;
                        color_index |= ((edit_arrangement.tiles.at(tile_count+1)->t[k]) >> (7-l))&0x01;
                        color_index |= (((edit_arrangement.tiles.at(tile_count+1)->t[k+8]) >> (7-l))&0x01)<<1;
                        edit_line[7-l] = (color_index) ? pals.p[attribs].p[color_index].rgb() : Qt::transparent;
                    }
                }
            }
            else{
                for(k=0;k<8;k++){
                    edit_line = (QRgb*) edit_tile.scanLine(k);
                    for(l=0;l<8;l++){
                        color_index = 0;
                        color_index |= ((edit_arrangement.tiles.at(tile_count)->t[k]) >> (7-l))&0x01;
                        color_index |= (((edit_arrangement.tiles.at(tile_count)->t[k+8]) >> (7-l))&0x01)<<1;
                        edit_line[l] = (color_index) ? pals.p[attribs].p[color_index].rgb() : Qt::transparent;
                    }
                }
                for(k=0;k<8;k++){
                    edit_line = (QRgb*) edit_tile.scanLine(k+8);
                    for(l=0;l<8;l++){
                        color_index = 0;
                        color_index |= ((edit_arrangement.tiles.at(tile_count+1)->t[k]) >> (7-l))&0x01;
                        color_index |= (((edit_arrangement.tiles.at(tile_count+1)->t[k+8]) >> (7-l))&0x01)<<1;
                        edit_line[l] = (color_index) ? pals.p[attribs].p[color_index].rgb() : Qt::transparent;
                    }
                }
            }
            image.at(tile_count>>1)->setTileID(edit_arrangement.tiles.at(tile_count)->id | (edit_arrangement.arrangement[i]&1));
            image.at(tile_count>>1)->setPixmap(QPixmap::fromImage(edit_tile));
            image.at(tile_count>>1)->setOffset(x,y);
            tile_count+=2;
            i+=2;
        }
    }
    for(i=0;i<image.size();i++){
        image.at(i)->setZValue((image.size()-i)+1);
    }
    drawRowUsage();
}

void editSpriteDialog::drawCHR(){
    unsigned int i,j,k,pal_counter;
    uint8_t color_index;
    pals.convertColors();
    QRgb * edit_line;
    for(pal_counter = 0;pal_counter<4;pal_counter++){
        for(i=0;i<0x100;i++){
            bg_tiles[pal_counter][i] = QImage(8,8,QImage::Format_ARGB32);
            for(j=0;j<8;j++){
                edit_line = (QRgb*) bg_tiles[pal_counter][i].scanLine(j);
                for(k=0;k<8;k++){
                    color_index = 0;
                    color_index |= ((bg_page->t[i].t[j]) >> (7-k))&0x01;
                    color_index |= (((bg_page->t[i].t[j+8]) >> (7-k))&0x01)<<1;
                    edit_line[k] = pals.p[pal_counter].p[color_index].rgb();
                }
            }
            if(pal_counter== selected_palette) bg_chr_array[i]->setPixmap(QPixmap::fromImage(bg_tiles[pal_counter][i]));
        }
    }

    for(pal_counter = 0;pal_counter<4;pal_counter++){
        for(i=0;i<0x100;i++){
            sprite_tiles[pal_counter][i] = QImage(8,8,QImage::Format_ARGB32);
            for(j=0;j<8;j++){
                edit_line = (QRgb*) sprite_tiles[pal_counter][i].scanLine(j);
                for(k=0;k<8;k++){
                    color_index = 0;
                    color_index |= ((sprite_page->t[i].t[j]) >> (7-k))&0x01;
                    color_index |= (((sprite_page->t[i].t[j+8]) >> (7-k))&0x01)<<1;
                    edit_line[k] = pals.p[pal_counter].p[color_index].rgb();
                }
            }
            if(pal_counter== selected_palette) sprite_chr_array[i]->setPixmap(QPixmap::fromImage(sprite_tiles[pal_counter][i]));
        }
    }

    bg_indicators_pix_item->setPixmap(QPixmap::fromImage(bg_indicators));
    sprite_indicators_pix_item->setPixmap(QPixmap::fromImage(sprite_indicators));
}

void editSpriteDialog::on_color_selected(uint8_t p,uint8_t color){
    pals = sprite_palettes->getPalette();
    pals.convertColors();
    selected_color = color&3;
    selected_palette = p&3;

    drawCHR();
    drawArrangement();
}

void editSpriteDialog::arrangement_clicked(QMouseEvent * event){
    if(paste_ready){
        paste(event);
        return;
    }
    struct mask_pixels{
        uint8_t arrangement_target;
        uint8_t x;
        uint8_t y;
        uint16_t analysis;
    };

    spriteEditItem * target = NULL;
    std::vector<mask_pixels> masks;
    uint8_t x,y;
    uint8_t arrangement_target;
    int i;
    NEStile * CHR_target;

    uint8_t scene_coordinate_x = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).x();
    uint8_t scene_coordinate_y = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).y();

    QList<QGraphicsItem *> target_candidates = new_scene.items(QPointF(scene_coordinate_x,scene_coordinate_y));

    if(!draw_mode && !paste_ready){
        last_focus = 1;
        if(!target_candidates.size()) return;
        for(i=0;i<target_candidates.size();i++){
            if(((spriteEditItem *)(target_candidates.at(i)))->getID() == 0xff){
                return;
            }
            mask_pixels current_mask;
            x = scene_coordinate_x;
            y = scene_coordinate_y;
            x -= ((spriteEditItem *)(target_candidates.at(i)))->offset().x();
            y -= ((spriteEditItem *)(target_candidates.at(i)))->offset().y();
            arrangement_target = ((spriteEditItem *)(target_candidates.at(i)))->getID();
            current_mask.arrangement_target = arrangement_target;
            current_mask.x = x;
            current_mask.y = y;
            if(y>=8){
                y-=8;
                CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
            }
            else{
                CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
            }
            current_mask.analysis = analyzeTile(CHR_target,x,y);
            if(current_mask.analysis&4) break;
        }
        if(i>=target_candidates.size()){
            target = (spriteEditItem *)(target_candidates.at(0));
        }
        else{
            target = (spriteEditItem *)(target_candidates.at(i));
        }

        arrangement_tile = target->getID();
        if(arrangement_tile == 0xff){
            arrangement_tile = -1;
            return;
        }
        if(target->getTileType()){
            sprite_chr_array[target->getTileID()&0xfe]->showSelected();
        }
        else{
            bg_chr_array[target->getTileID()&0xfe]->showSelected();
        }
        uint8_t old_palette = selected_palette;
        selected_palette = target->getAttribs()&3;
        if(selected_palette != old_palette){
            sprite_palettes->setSelectedPalette(selected_palette);
            pals = sprite_palettes->getPalette();
            pals.convertColors();
            drawCHR();
        }
        target->setSelected(true);
        return;
    }

    for(i=0;i<target_candidates.size();i++){
        if(((spriteEditItem *)(target_candidates.at(i)))->getID() == 0xff){
            target = (spriteEditItem *)(target_candidates.at(i));
            break;
        }
        else if(selected_color && (((spriteEditItem *)(target_candidates.at(i)))->getAttribs() == selected_palette)){
            target = (spriteEditItem *)(target_candidates.at(i));
            break;
        }
        else{
            if(selected_color){
                int j;
                for(j=0;j<4;j++){
                    if(pals.p[selected_palette].nes_colors[selected_color] == pals.p[((spriteEditItem *)(target_candidates.at(i)))->getAttribs()].nes_colors[j]) break;
                }
                if(j<4){
                    target = (spriteEditItem *)(target_candidates.at(i));
                    break;
                }
            }
            //**********put masks stuff here.
            //don't draw on the current tile.  Allow the mouse click to propagate to the tile beneath
            mask_pixels current_mask;
            x = scene_coordinate_x;
            y = scene_coordinate_y;
            x -= ((spriteEditItem *)(target_candidates.at(i)))->offset().x();
            y -= ((spriteEditItem *)(target_candidates.at(i)))->offset().y();
            arrangement_target = ((spriteEditItem *)(target_candidates.at(i)))->getID();
            current_mask.arrangement_target = arrangement_target;
            current_mask.x = x;
            current_mask.y = y;
            if(y>=8){
                y-=8;
                CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
            }
            else{
                CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
            }
            current_mask.analysis = analyzeTile(CHR_target,x,y);
            masks.push_back(current_mask);  //Create a stack of pixels currently drawn underneath the cursor.
        }
    }
    if(!target) return;

    x = scene_coordinate_x;
    y = scene_coordinate_y;
    x -= target->offset().x();
    y -= target->offset().y();
    arrangement_target = target->getID();

    if(!edit_progress){
        edit_progress = true;
        undo_max = undo_position;
        //Setup restore point in the event of undo
        undo_actions[undo_position].old_arrangement = edit_arrangement;
        undo_actions[undo_position].old_bg = *bg_page;
        undo_actions[undo_position].old_sprites = *sprite_page;
    }

    if(selected_color && (arrangement_target == 0xff)){
        //If we aren't using the universal bg color, create a new tile to match the color
        //create new tile here
        target = allocateNewTile(scene_coordinate_x,scene_coordinate_y);
        if(!target) return;
        x = scene_coordinate_x - target->offset().x();
        y = scene_coordinate_y - target->offset().y();
        arrangement_target = target->getID();
        if(y>=8){
            y-=8;
            CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
        }
        else{
            CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
        }
        CHR_target->shared = false;
        CHR_target->t[y] = (CHR_target->t[y]&(0xff ^ (1<<(7-x)))) | ((selected_color&1)<<(7-x));
        CHR_target->t[y+8] = (CHR_target->t[y + 8]&(0xff ^ (1<<(7-x)))) | (((selected_color&2)>>1)<<(7-x));
        CHR_target->checksum = 0;
        for(i=0;i<0x10;i++){
            CHR_target->checksum += CHR_target->t[i];
        }
        compactQuick();
        edit_arrangement.modified = true;
        undo_actions[undo_position].new_arrangement = edit_arrangement;
        undo_actions[undo_position].new_bg = *bg_page;
        undo_actions[undo_position].new_sprites = *sprite_page;
        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();
        return;
    }
    for(i=0;i<masks.size();i++){ //Check if the pixel is masked by another tile
        uint8_t temp_analysis = masks.at(i).analysis;
        if(temp_analysis & 4){
            //attempt to erase the pixel(s) above and draw the color where needed
            uint8_t temp_x = masks.at(i).x;
            uint8_t temp_y = masks.at(i).y;
            uint8_t temp_target = masks.at(i).arrangement_target;
            if(temp_analysis&3){
                //****attempt to allocate new tile
                if(!duplicateTile(temp_target)) return;
            }
            if(temp_y>=8){
                temp_y-=8;
                CHR_target = edit_arrangement.tiles.at((temp_target<<1) + 1);
            }
            else{
                CHR_target = edit_arrangement.tiles.at(temp_target<<1);
            }
            CHR_target->t[temp_y] = CHR_target->t[temp_y]&(0xff ^ (1<<(7-temp_x)));
            CHR_target->t[temp_y+8] = CHR_target->t[temp_y + 8]&(0xff ^ (1<<(7-temp_x)));
            CHR_target->checksum = 0;
            for(int j=0;j<0x10;j++){
                CHR_target->checksum += CHR_target->t[j];
            }
        }
    }
    if(!selected_color){
        compactQuick();
        edit_arrangement.modified = true;
        undo_actions[undo_position].new_arrangement = edit_arrangement;
        undo_actions[undo_position].new_bg = *bg_page;
        undo_actions[undo_position].new_sprites = *sprite_page;
        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();
        return;
    }
    uint8_t pixel_color = 0xff;
    if(target->getAttribs() == selected_palette){
        pixel_color = selected_color;
    }
    else{
        for(i=0;i<4;i++){
            if(pals.p[selected_palette].nes_colors[selected_color] == pals.p[target->getAttribs()].nes_colors[i]) break;
        }
        if(i>=4){
            return;    //exit.  We should never reach this point.
        }
        //use the matching color in the other palette instead of the selected color in selected palette
        pixel_color = i;
    }
    if(y>=8){
        y-=8;
        CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
    }
    else{
        CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
    }
    NEStile temp_tile;
    if(target->getTileType()){
        temp_tile = sprite_page->t[CHR_target->id];
    }
    else{
        temp_tile = bg_page->t[CHR_target->id];
    }
    temp_tile.t[y] = (temp_tile.t[y]&(0xff ^ (1<<(7-x)))) | ((pixel_color&1)<<(7-x));
    temp_tile.t[y+8] = (temp_tile.t[y + 8]&(0xff ^ (1<<(7-x)))) | (((pixel_color&2)>>1)<<(7-x));
    temp_tile.checksum = 0;
    for(i=0;i<0x10;i++){
        temp_tile.checksum += temp_tile.t[i];
    }
    uint16_t tile_analysis = analyzeTile(CHR_target,x,y) & 3;
    if(tile_analysis){
        arrangement_target = target->getID();
        if(!duplicateTile(arrangement_target)) return;  //attempt to allocate new tile
        y = scene_coordinate_y;
        y -= target->offset().y();
        if(y>=8){
            y-=8;
            CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
        }
        else{
            CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
        }
    }
    *CHR_target = temp_tile;
    compactQuick();
    edit_arrangement.modified = true;
    undo_actions[undo_position].new_arrangement = edit_arrangement;
    undo_actions[undo_position].new_bg = *bg_page;
    undo_actions[undo_position].new_sprites = *sprite_page;
    updateCHRMask();
    drawCHR();
    drawBackground();
    drawArrangement();

}

void editSpriteDialog::arrangement_released(){
    if(edit_progress){
        edit_progress = false;
        undo_position++;
        if(undo_position>=UNDO_SIZE) undo_position -= UNDO_SIZE;
        undo_max = undo_position;
        if(undo_min == undo_max){
            undo_min++;
            if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
        }
    }
}


void editSpriteDialog::on_comboBox_2_currentIndexChanged(int index) //BG overlay selection combobox
{
    drawBackground();
}


void editSpriteDialog::on_comboBox_3_currentIndexChanged(int index) //PPU base offset selection combobox
{
    drawBackground();
}

void editSpriteDialog::drawBackground(){
    uint16_t startingX;
    uint16_t startingY;
    uint16_t ppu_base_chosen = ui->comboBox_3->currentIndex();
    if(ppu_base_chosen>120) ppu_base_chosen = 0;
    startingX = ppu_bases_possible[ppu_base_chosen]&0x1F;
    startingX <<= 3;
    startingY = ppu_bases_possible[ppu_base_chosen]>>5;
    startingY &= 0x1F;
    startingY <<= 3;
    int i,j;
    QRgb * edit_line;
    QRgb * copy_line;
    QColor temp_color = pals.ubg_color;
    uint16_t selected_background = ui->comboBox_2->currentIndex();
    temp_color.setAlpha(ui->verticalSlider->value());
    for(i=0;i<arrangement_background.height();i++){
        edit_line = (QRgb *)arrangement_background.scanLine(i);
        for(j=0;j<arrangement_background.width();j++){
            edit_line[j] = temp_color.rgba();
        }
    }
    if(selected_background < num_bgs){
        for(i=0;i<sorted_bgs[selected_background]->image.height();i++){
            if((i+startingY) >= 240) break;
            edit_line = (QRgb *)arrangement_background.scanLine(i+startingY);
            copy_line = (QRgb *)sorted_bgs[selected_background]->image.scanLine(i);
            for(j=0;j<sorted_bgs[selected_background]->image.width();j++){
                if((j + startingX) >= 256) break;
                temp_color = QColor::fromRgb(copy_line[j]);
                temp_color.setAlpha(ui->verticalSlider->value());
                edit_line[j + startingX] = temp_color.rgba();
            }
        }
    }
    overlay_item->setPixmap(QPixmap::fromImage(arrangement_background));
    overlay_item->setOffset(0x80 - (ui->spinBox->value()<<1),0x80 - (ui->spinBox_2->value()<<1));
    overlay_item->setID(0xff);
    overlay_item->setZValue(0);
}


void editSpriteDialog::on_spinBox_editingFinished()
{
    drawBackground();
    image.at(0)->setZValue(20);
}


void editSpriteDialog::on_spinBox_2_editingFinished()
{
    drawBackground();
}


void editSpriteDialog::on_verticalSlider_sliderMoved(int position)
{
    drawBackground();
}

uint16_t editSpriteDialog::analyzeTile(NEStile * to_check, uint8_t x, uint8_t y){
    uint16_t result = 0;
    uint8_t selected_count = 0;
    int i;

    for(i=0;i<edit_arrangement.tiles.size();i++){
        if(to_check == edit_arrangement.tiles.at(i)){
            selected_count++;
        }
    }
    if(selected_count > 1) result |= 1;     //bit 0 determines if tile is reused within the image
    if(to_check->shared) result |= 2;   //bit 1 determines whether tile is shared with other images

    uint8_t color_index = 0;
    color_index |= ((to_check->t[y]) >> (7-x))&0x01;
    color_index |= (((to_check->t[y+8]) >> (7-x))&0x01)<<1;
    if(color_index) result |= 4;    //bit 2 determines whether specific pixel is opaque.

    return result;
}

spriteEditItem * editSpriteDialog::allocateNewTile(int mouse_x,int mouse_y){
    if(edit_arrangement.tiles.size() >= 0x7E) return NULL;
    if(mouse_x < 0x30 || mouse_x >= 0xC8) return NULL;
    if(mouse_y < 0x30 || mouse_y >= 0xD0) return NULL;
    if(image.size()>=0x3F) return NULL;
    QPoint target_pixel(mouse_x,mouse_y);
    spriteEditItem * result;
    bool sprite_free = false;
    uint8_t tile_free;
    int i,j,k;
    uint8_t tile_count = 0;
    uint8_t tiles_in_column;
    uint8_t temp_x,temp_y;
    uint8_t test_x, test_y;
    for(i=0;i<0x100;i+=2){
        if(sprite_page->sprite_used[i] || sprite_page->t[i].shared) continue;
        if(sprite_page->sprite_used[i+1] || sprite_page->t[i+1].shared) continue;
        for(j=0;j<0x10;j++){
            sprite_page->t[i].t[j] = 0x00;
            sprite_page->t[i+1].t[j] = 0x00;
        }
        sprite_page->t[i].checksum = 0;
        sprite_page->t[i+1].checksum = 0;
        sprite_free = true;
        break;
    }
    if(i>=0x100){
        for(i=0;i<0x100;i+=2){
            if(bg_page->sprite_used[i] || bg_page->t[i].shared) continue;
            if(bg_page->sprite_used[i+1] || bg_page->t[i+1].shared) continue;
            for(j=0;j<0x10;j++){
                bg_page->t[i].t[j] = 0x00;
                bg_page->t[i+1].t[j] = 0x00;
            }
            bg_page->t[i].checksum = 0;
            bg_page->t[i+1].checksum = 0;
            break;
        }
        if(i>=0x100) return NULL;
    }
    tile_free = i;
    uint8_t min_diff = 0xff;
    uint8_t temp_diff;
    int8_t best_offset = -1;
    for(i=0;i<0x29;i++){
        if(mouse_x >= (uint8_t)(spritexy_values[i] + 0x80)){
            temp_diff = mouse_x - (spritexy_values[i] + 0x80);
        }
        else{
            continue;   //only consider cases where mouse X is greater than the left side of the potential tile
        }
        if(temp_diff < min_diff){
            min_diff = temp_diff;
            if(min_diff < 8) best_offset = i;
        }
    }
    if(best_offset >= 0){
        mouse_x = (uint8_t)(spritexy_values[best_offset] + 0x80);
    }
    else{
        return NULL;
    }
    min_diff = 0xff;
    best_offset = -1;
    for(i=0;i<0x20;i++){
        if(mouse_y >= (uint8_t)(spritexy_values[i] + 0x80)){
            temp_diff = mouse_y - (spritexy_values[i] + 0x80);
        }
        else{
            continue;   //only consider cases where mouse X is greater than the left side of the potential tile
        }
        if(temp_diff < min_diff){
            min_diff = temp_diff;
            if(min_diff < 0x10) best_offset = i;
        }
    }
    if(best_offset >= 0){
        mouse_y = (uint8_t)(spritexy_values[best_offset] + 0x80);
    }
    else{
        return NULL;
    }
    int column_start = -1;
    bool matched = false;
    bool old_matched = false;
    bool rematch_already_attempted = false;
    struct color_match{
        int x;
        int y;
        bool overlap;
    };
    std::vector<color_match>overlap_list;
    bool tile_overlap;
    int restore_x = mouse_x;
    int restore_y = mouse_y;


    for(i=0;i<edit_arrangement.arrangement.length();){
        if(!matched) column_start = i;      //We want to store the earliest column overlapping desired position to draw on top.
        if(edit_arrangement.arrangement[i]&0x80) break;
        if((edit_arrangement.arrangement[i]&0x7F) >= 0x29){
            temp_x = spritexy_values[0] + 0x80;
        }
        else{
            temp_x = spritexy_values[(uint8_t)(edit_arrangement.arrangement[i]&0x7F)] + 0x80;
        }
        i++;
        tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            tile_overlap = false;
            temp_y = 0x80 + spritexy_values[(((uint8_t)edit_arrangement.arrangement[i+1])>>3)];
            if(temp_y >= mouse_y){
                if((temp_y - mouse_y) < 0x10){
                    tile_count++;
                    if(temp_x >= mouse_x){
                        if((temp_x - mouse_x) < 8){
                            if(!matched) matched = true;
                            tile_overlap = true;
                        }
                    }
                    else{
                        if((mouse_x - temp_y) < 8){
                            if(!matched) matched = true;
                            tile_overlap = true;
                        }
                    }
                }
            }
            else{
                if((mouse_y - temp_y) < 0x10){
                    tile_count++;
                    if(temp_x >= mouse_x){
                        if((temp_x - mouse_x) < 8){
                            if(!matched) matched = true;
                            tile_overlap = true;
                        }
                    }
                    else{
                        if((mouse_x - temp_y) < 8){
                            if(!matched) matched = true;
                            tile_overlap = true;
                        }
                    }
                }
            }
            if((((uint8_t)edit_arrangement.arrangement[i+1]&6)>>1) == selected_palette){
                color_match overlap_item;
                overlap_item.x = temp_x;
                overlap_item.y = temp_y;
                overlap_item.overlap = tile_overlap;
                overlap_list.push_back(overlap_item);
            }
            i+=2;
        }
        if(i>=(edit_arrangement.arrangement.length() - 1)){    //Check the list of matching palette tiles at the end to confirm positioning
            if(rematch_already_attempted){
                for(j=0;j<overlap_list.size();j++){
                    if(overlap_list.at(j).overlap) break;
                }
                if((j<overlap_list.size()) || (tile_count>7)){
                    mouse_x = restore_x;
                    mouse_y = restore_y;
                    matched = old_matched;
                }
                break;
            }
            for(j=0;j<overlap_list.size();j++){
                if(overlap_list.at(j).overlap){
                    test_x = overlap_list.at(j).x;
                    test_y = overlap_list.at(j).y;
                    if(target_pixel.x() < test_x){
                        test_x = test_x - 8;
                    }
                    else if(target_pixel.x() >= test_x + 8){
                        test_x = test_x + 8;
                    }
                    if(target_pixel.y() < test_y){
                        test_y = test_y - 0x10;
                    }
                    else if(target_pixel.y() >= test_y + 0x10){
                        test_y = test_y + 0x10;
                    }
                    for(k=0;k<0x29;k++){
                        if((uint8_t)spritexy_values[k] == (uint8_t)(test_x - 0x80)) break;
                    }
                    if(k>=0x29) continue;
                    for(k=0;k<0x20;k++){
                        if((uint8_t)spritexy_values[k] == (uint8_t)(test_y - 0x80)) break;
                    }
                    if(k>=0x20) continue;
                    break;
                }
            }
            if(j<overlap_list.size()){
                overlap_list.clear();
                i = 0;
                mouse_x = test_x;
                mouse_y = test_y;
                tile_count = 0;
                old_matched = matched;
                matched = false;
                rematch_already_attempted = true;
            }
        }
    }
    if(tile_count > 7) return NULL; //Make sure there aren't more than 8 tiles in any row
    if(!matched) column_start = 0;

    for(i=0;i<image.size();i++){
        new_scene.removeItem(image.at(i));
        delete image.at(i);
    }
    image.clear();

    //Need to add the tile to the arrangement.  Draw it on top
    std::string temp_arrangement = "";
    std::vector<NEStile *>temp_tiles;

    tile_count = 0;
    for(i=0;i<edit_arrangement.arrangement.length();){
        if(i >= column_start) break;
        temp_arrangement += edit_arrangement.arrangement[i++];
        tiles_in_column = edit_arrangement.arrangement[i];
        temp_arrangement += edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count));
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count+1));
            image.push_back(new spriteEditItem);
            new_scene.addItem(image.at(tile_count>>1));
            image.at(tile_count>>1)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
            image.at(tile_count>>1)->setID(tile_count>>1);
            image.at(tile_count>>1)->setArrangementOffset(tile_count>>1);
            image.at(tile_count>>1)->setFlag(QGraphicsItem::ItemIsSelectable);
            tile_count+=2;
            temp_arrangement += edit_arrangement.arrangement[i++];
            temp_arrangement += edit_arrangement.arrangement[i++];
        }
    }
    result = new spriteEditItem;
    image.push_back(result);
    new_scene.addItem(result);
    result->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    result->setTileType(sprite_free);
    result->setID(tile_count>>1);
    result->setTileID(tile_free);
    result->setPixmap(QPixmap::fromImage(QImage(8,16,QImage::Format_ARGB32)));
    result->setOffset(mouse_x,mouse_y);
    result->setFlag(QGraphicsItem::ItemIsSelectable);
    if((edit_arrangement.arrangement.length() && (uint8_t)(spritexy_values[(uint8_t)edit_arrangement.arrangement[i]] + 0x80)) == mouse_x){
        temp_arrangement += edit_arrangement.arrangement[i++];
        tiles_in_column = edit_arrangement.arrangement[i++] + 1;
        temp_arrangement += tiles_in_column;
        temp_arrangement += tile_free | ((sprite_free)? 1 : 0);
        for(j=0;j<0x20;j++){
            if(((uint8_t)(spritexy_values[j] + 0x80)) == mouse_y) break;
        }
        temp_arrangement += (j<<3) | (selected_palette << 1);
        if(sprite_free){
            temp_tiles.push_back(&sprite_page->t[tile_free]);
            temp_tiles.push_back(&sprite_page->t[tile_free + 1]);
        }
        else{
            temp_tiles.push_back(&bg_page->t[tile_free]);
            temp_tiles.push_back(&bg_page->t[tile_free + 1]);
        }
        j=1;
        for(;j<tiles_in_column;j++){
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count));
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count+1));
            image.push_back(new spriteEditItem);
            new_scene.addItem(image.at((tile_count>>1)+1));
            image.at((tile_count>>1)+1)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
            image.at((tile_count>>1)+1)->setID((tile_count>>1)+1);
            image.at((tile_count>>1)+1)->setArrangementOffset((tile_count>>1)+1);
            image.at((tile_count>>1)+1)->setFlag(QGraphicsItem::ItemIsSelectable);
            tile_count+=2;
            temp_arrangement += edit_arrangement.arrangement[i++];
            temp_arrangement += edit_arrangement.arrangement[i++];
        }
    }
    else{
        for(j=0;j<0x29;j++){
            if(((uint8_t)((spritexy_values[j] + 0x80))) == mouse_x) break;
        }
        temp_arrangement += j;
        temp_arrangement += 0x01;
        temp_arrangement += tile_free | ((sprite_free)? 1 : 0);
        for(j=0;j<0x20;j++){
            if(((uint8_t)(spritexy_values[j] + 0x80)) == mouse_y) break;
        }
        temp_arrangement += (j<<3) | (selected_palette << 1);
        if(sprite_free){
            temp_tiles.push_back(&sprite_page->t[tile_free]);
            temp_tiles.push_back(&sprite_page->t[tile_free + 1]);
        }
        else{
            temp_tiles.push_back(&bg_page->t[tile_free]);
            temp_tiles.push_back(&bg_page->t[tile_free + 1]);
        }
    }

    for(;i<edit_arrangement.arrangement.length();){
        temp_arrangement += edit_arrangement.arrangement[i++];
        tiles_in_column = edit_arrangement.arrangement[i];
        temp_arrangement += edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count));
            temp_tiles.push_back(edit_arrangement.tiles.at(tile_count+1));
            image.push_back(new spriteEditItem);
            new_scene.addItem(image.at((tile_count>>1)+1));
            image.at((tile_count>>1)+1)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
            image.at((tile_count>>1)+1)->setID((tile_count>>1)+1);
            image.at((tile_count>>1)+1)->setArrangementOffset((tile_count>>1)+1);
            image.at((tile_count>>1)+1)->setFlag(QGraphicsItem::ItemIsSelectable);
            tile_count+=2;
            temp_arrangement += edit_arrangement.arrangement[i++];
            temp_arrangement += edit_arrangement.arrangement[i++];
        }
    }
    edit_arrangement.arrangement = temp_arrangement;
    edit_arrangement.tiles = temp_tiles;

    tile_count = 0;
    for(i=0;i<edit_arrangement.arrangement.length();){
        temp_x = edit_arrangement.arrangement[i];
        if(edit_arrangement.arrangement[i++]&0x80) break;
        if(temp_x>=0x29) temp_x=0;
        temp_x = (uint8_t)(spritexy_values[temp_x] + 0x80);
        tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            temp_y = (uint8_t)(0x80 + spritexy_values[((uint8_t)edit_arrangement.arrangement[i+1])>>3]);
            image.at(tile_count>>1)->setTileID((edit_arrangement.arrangement[i]&0x1)| ((edit_arrangement.tiles.at(tile_count)->id)&0xFE));
            image.at(tile_count>>1)->setAttribs((edit_arrangement.arrangement[i+1]&0x6)>>1);
            image.at(tile_count>>1)->setFlip(edit_arrangement.arrangement[i+1]&1);
            image.at(tile_count>>1)->setTileType(edit_arrangement.arrangement[i]&0x1);
            image.at(tile_count>>1)->setOffset(temp_x,temp_y);
            i+=2;
            tile_count+=2;
        }
    }
    if(sprite_free){
        sprite_page->sprite_used[tile_free] = true;
        sprite_page->sprite_used[tile_free + 1] = true;
    }
    else{
        bg_page->sprite_used[tile_free] = true;
        bg_page->sprite_used[tile_free + 1] = true;
    }
    for(i=0;i<image.size();i++){
        image.at(i)->setZValue((image.size()-i)+1);
    }
    return result;
}

bool editSpriteDialog::duplicateTile(uint8_t arrangement_offset){
    int i;
    uint8_t tile_free;
    bool sprite_free = false;
    NEStile * result = NULL;
    for(i=0;i<0x100;i+=2){
        if(sprite_page->sprite_used[i] || sprite_page->t[i].shared) continue;
        if(sprite_page->sprite_used[i+1] || sprite_page->t[i+1].shared) continue;
        sprite_free = true;
        break;
    }
    if(i>=0x100){
        for(i=0;i<0x100;i+=2){
            if(bg_page->sprite_used[i] || bg_page->t[i].shared) continue;
            if(bg_page->sprite_used[i+1] || bg_page->t[i+1].shared) continue;
            break;
        }
        if(i>=0x100) return false;
    }
    tile_free = i;
    if(sprite_free){
        if(image.at(arrangement_offset)->getTileType()){
            sprite_page->t[tile_free] = sprite_page->t[image.at(arrangement_offset)->getTileID() & 0xFE];
            edit_arrangement.tiles.at(arrangement_offset<<1) = &sprite_page->t[tile_free];
            sprite_page->t[tile_free|1] = sprite_page->t[image.at(arrangement_offset)->getTileID() | 1];
            edit_arrangement.tiles.at((arrangement_offset<<1) + 1) = &sprite_page->t[tile_free|1];
        }
        else{
            sprite_page->t[tile_free] = bg_page->t[image.at(arrangement_offset)->getTileID() & 0xFE];
            edit_arrangement.tiles.at(arrangement_offset<<1) = &sprite_page->t[tile_free];
            sprite_page->t[tile_free|1] = bg_page->t[image.at(arrangement_offset)->getTileID() | 1];
            edit_arrangement.tiles.at((arrangement_offset<<1) + 1) = &sprite_page->t[tile_free|1];
        }
    }
    else{
        if(image.at(arrangement_offset)->getTileType()){
            bg_page->t[tile_free] = sprite_page->t[image.at(arrangement_offset)->getTileID() & 0xFE];
            edit_arrangement.tiles.at(arrangement_offset<<1) = &bg_page->t[tile_free];
            bg_page->t[tile_free|1] = sprite_page->t[image.at(arrangement_offset)->getTileID() | 1];
            edit_arrangement.tiles.at((arrangement_offset<<1) + 1) = &bg_page->t[tile_free|1];
        }
        else{
            bg_page->t[tile_free] = bg_page->t[image.at(arrangement_offset)->getTileID() & 0xFE];
            edit_arrangement.tiles.at(arrangement_offset<<1) = &bg_page->t[tile_free];
            bg_page->t[tile_free|1] = bg_page->t[image.at(arrangement_offset)->getTileID() | 1];
            edit_arrangement.tiles.at((arrangement_offset<<1) + 1) = &bg_page->t[tile_free|1];
        }
    }
    image.at(arrangement_offset)->setTileType(sprite_free);
    image.at(arrangement_offset)->setTileID(tile_free | ((sprite_free)? 1:0));
    i=edit_arrangement.locateTileInArrangement(arrangement_offset);
    edit_arrangement.arrangement[i] = tile_free | ((sprite_free)?1:0);
    updateCHRMask();

    return true;
}

void editSpriteDialog::on_pushButton_5_clicked()    //zoom in
{
    arrangement_released();
    QMatrix stretch= arrangement_view->matrix();
    if(stretch.m11() < 8){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())<<1));
        zoom_label.append("00%");
        ui->arrangement_zoom->setText(zoom_label);
        arrangement_view->scale(2,2);
        drawRowUsage();
    }
}

void editSpriteDialog::on_pushButton_6_clicked()    //zoom out
{
    arrangement_released();
    QMatrix stretch= arrangement_view->matrix();
    if(stretch.m11() > 1){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())>>1));
        zoom_label.append("00%");
        ui->arrangement_zoom->setText(zoom_label);
        arrangement_view->scale(0.5,0.5);
        drawRowUsage();
    }
}


void editSpriteDialog::on_pushButton_clicked()      //BG CHR zoom in
{
    arrangement_released();
    QMatrix stretch= ui->BG_chr_view->matrix();
    if(stretch.m11() < 8){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())<<1));
        zoom_label.append("00%");
        ui->bg_zoom->setText(zoom_label);
        ui->BG_chr_view->scale(2,2);
    }
}

void editSpriteDialog::on_pushButton_2_clicked()    //BG CHR zoom out
{
    arrangement_released();
    QMatrix stretch= ui->BG_chr_view->matrix();
    if(stretch.m11() > 1){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())>>1));
        zoom_label.append("00%");
        ui->bg_zoom->setText(zoom_label);
        ui->BG_chr_view->scale(0.5,0.5);
    }
}

void editSpriteDialog::on_pushButton_3_clicked()    //Sprite CHR zoom in
{
    arrangement_released();
    QMatrix stretch= ui->sprite_CHR_view->matrix();
    if(stretch.m11() < 8){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())<<1));
        zoom_label.append("00%");
        ui->sprite_zoom->setText(zoom_label);
        ui->sprite_CHR_view->scale(2,2);
    }
}

void editSpriteDialog::on_pushButton_4_clicked()    //Sprite CHR zoom out
{
    arrangement_released();
    QMatrix stretch= ui->sprite_CHR_view->matrix();
    if(stretch.m11() > 1){
        QString zoom_label(convertToASCII(((uint8_t)stretch.m11())>>1));
        zoom_label.append("00%");
        ui->sprite_zoom->setText(zoom_label);
        ui->sprite_CHR_view->scale(0.5,0.5);
    }
}

void editSpriteDialog::reparseImage(){
    int i,j;
    for(i=0;i<image.size();i++){
        new_scene.removeItem(image.at(i));
        delete image.at(i);
    }
    image.clear();
    for(i=0;i<(edit_arrangement.tiles.size()>>1);i++){
        image.push_back(new spriteEditItem());
        new_scene.addItem(image.at(i));
        image.at(i)->setID(i);
        image.at(i)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
        image.at(i)->setArrangementOffset(i);
        image.at(i)->setFlag(QGraphicsItem::ItemIsSelectable);
    }

    uint8_t tile_count = 0;
    for(i=0;i<0x100;i++){
        sprite_page->sprite_used[i] = false;
        bg_page->bg_used[i] = false;
    }
    for(i=0;i<edit_arrangement.arrangement.length();){
        if(edit_arrangement.arrangement[i++]&0x80) break;
        uint8_t tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            if(edit_arrangement.arrangement[i]&0x1){
                edit_arrangement.tiles.at(tile_count) = &(sprite_page->t[edit_arrangement.tiles.at(tile_count)->id]);
                edit_arrangement.tiles.at(tile_count+1) = &(sprite_page->t[edit_arrangement.tiles.at(tile_count+1)->id]);
                sprite_page->sprite_used[edit_arrangement.tiles.at(tile_count)->id] = true;
                sprite_page->sprite_used[edit_arrangement.tiles.at(tile_count+1)->id] = true;
            }
            else{
                edit_arrangement.tiles.at(tile_count) = &(bg_page->t[edit_arrangement.tiles.at(tile_count)->id]);
                edit_arrangement.tiles.at(tile_count+1) = &(bg_page->t[edit_arrangement.tiles.at(tile_count+1)->id]);
                bg_page->sprite_used[edit_arrangement.tiles.at(tile_count)->id] = true;
                bg_page->sprite_used[edit_arrangement.tiles.at(tile_count+1)->id] = true;
            }
            image.at(tile_count>>1)->setTileID((edit_arrangement.arrangement[i]&0x1)| ((edit_arrangement.tiles.at(tile_count)->id)&0xFE));
            image.at(tile_count>>1)->setAttribs((edit_arrangement.arrangement[i+1]&0x6)>>1);
            image.at(tile_count>>1)->setFlip(edit_arrangement.arrangement[i+1]&1);
            image.at(tile_count>>1)->setTileType(edit_arrangement.arrangement[i]&0x1);
            i+=2;
            tile_count+=2;
        }
    }
}

void editSpriteDialog::resizeEvent(QResizeEvent * event){
    unsigned int i;

    ui->horizontalScrollBar->setGeometry(0,height()-25,width()-25,25);
    ui->verticalScrollBar->setGeometry(width()-25,0,25,height());

    QPoint scroll_offset(widget_offsets[0].x() - widget_list[0]->x(),widget_offsets[0].y() - widget_list[0]->y());
    int range;
    if((711-height()) > 0){
        range = ui->verticalScrollBar->maximum() - ui->verticalScrollBar->minimum();
        ui->verticalScrollBar->setValue(scroll_offset.y() * range / (711-height()));
    }
    if((805-width()) > 0){
        range = ui->horizontalScrollBar->maximum() - ui->horizontalScrollBar->minimum();
        ui->horizontalScrollBar->setValue(scroll_offset.x() * range / (805-width()));
    }

    if(width() < 805){
        ui->horizontalScrollBar->show();
    }
    else{
        ui->horizontalScrollBar->hide();
        for(i=0;i<widget_list.size();i++){
            widget_list[i]->move(widget_offsets[i].x(),widget_list[i]->y());
        }
    }
    if(height() < 711){
        ui->verticalScrollBar->show();
    }
    else{
        ui->verticalScrollBar->hide();
        for(i=0;i<widget_list.size();i++){
            widget_list[i]->move(widget_list[i]->x(),widget_offsets[i].y());
        }
    }
    QDialog::resizeEvent(event);
}

void editSpriteDialog::on_radioButton_clicked()     //select radio button
{
    draw_mode = false;
    arrangement_view->setDragMode(QGraphicsView::RubberBandDrag);
}

void editSpriteDialog::on_radioButton_2_clicked()
{
    draw_mode = true;
    arrangement_view->setDragMode(QGraphicsView::NoDrag);
}

void editSpriteDialog::paste_position(QMouseEvent * event){
    if(paste_ready){
        int i;
        int mouse_x = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).x();
        int mouse_y = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).y();
        mouse_x -= clipboard_image.width()>>1;
        mouse_y -= clipboard_image.height()>>1;

        uint16_t min_diff = 0xffff;
        uint16_t temp_diff;
        int8_t best_offset = -1;
        for(i=0;i<0x29;i++){
            if(mouse_x >= (uint8_t)(spritexy_values[i] + 0x80)){
                temp_diff = mouse_x - (uint8_t)(spritexy_values[i] + 0x80);
            }
            else{
                temp_diff = (uint8_t)(spritexy_values[i] + 0x80) - mouse_x;
            }
            if(temp_diff < min_diff){
                min_diff = temp_diff;
                if(min_diff < 8) best_offset = i;
            }
        }
        if(best_offset >= 0){
            mouse_x = (uint8_t)(spritexy_values[best_offset] + 0x80);
        }
        else{
            mouse_x = to_paste->offset().x();
        }

        min_diff = 0xffff;
        best_offset = -1;
        for(i=0;i<0x20;i++){
            if(mouse_y >= (uint8_t)(spritexy_values[i] + 0x80)){
                temp_diff = mouse_y - (uint8_t)(spritexy_values[i] + 0x80);
            }
            else{
                temp_diff = (uint8_t)(spritexy_values[i] + 0x80) - mouse_y;
            }
            if(temp_diff < min_diff){
                min_diff = temp_diff;
                if(min_diff < 0x10) best_offset = i;
            }
        }
        if(best_offset >= 0){
            mouse_y = (uint8_t)(spritexy_values[best_offset] + 0x80);
        }
        else{
            mouse_y = to_paste->offset().y();
        }
        to_paste->setOffset(mouse_x,mouse_y);
    }
}

void editSpriteDialog::paste(QMouseEvent * event){
    struct mask_pixels{
        uint8_t arrangement_target;
        uint8_t x;
        uint8_t y;
        uint16_t analysis;
    };

    undo_max = undo_position;
    //Setup restore point in the event of undo
    undo_actions[undo_position].old_arrangement = edit_arrangement;
    undo_actions[undo_position].old_bg = *bg_page;
    undo_actions[undo_position].old_sprites = *sprite_page;

    spriteEditItem * target = NULL;
    std::vector<mask_pixels> masks;
    uint8_t x,y;
    uint8_t preferred_palette;
    uint8_t arrangement_target;
    int i,j,k,l;
    NEStile * CHR_target;
    int pixel_x = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).x();
    int pixel_y = (arrangement_view->mapToScene(event->localPos().x(),event->localPos().y())).y();
    pixel_x -= clipboard_image.width()>>1;
    pixel_y -= clipboard_image.height()>>1;
    uint8_t old_palette = selected_palette;
    uint8_t old_color = selected_color;

    uint32_t min_diff = 0xffff;
    uint16_t temp_diff;
    int8_t best_offset = -1;
    for(i=0;i<0x29;i++){
        if(pixel_x >= (uint8_t)(spritexy_values[i] + 0x80)){
            temp_diff = pixel_x - (uint8_t)(spritexy_values[i] + 0x80);
        }
        else{
            temp_diff = (uint8_t)(spritexy_values[i] + 0x80) - pixel_x;
        }
        if(temp_diff < min_diff){
            min_diff = temp_diff;
            if(min_diff < 8) best_offset = i;
        }
    }
    if(best_offset >= 0){
        pixel_x = (uint8_t)(spritexy_values[best_offset] + 0x80);
    }
    else{
        pixel_x = to_paste->offset().x();
    }

    min_diff = 0xffff;
    best_offset = -1;
    for(i=0;i<0x20;i++){
        if(pixel_y >= (uint8_t)(spritexy_values[i] + 0x80)){
            temp_diff = pixel_y - (uint8_t)(spritexy_values[i] + 0x80);
        }
        else{
            temp_diff = (uint8_t)(spritexy_values[i] + 0x80) - pixel_y;
        }
        if(temp_diff < min_diff){
            min_diff = temp_diff;
            if(min_diff < 0x10) best_offset = i;
        }
    }
    if(best_offset >= 0){
        pixel_y = (uint8_t)(spritexy_values[best_offset] + 0x80);
    }
    else{
        pixel_y = to_paste->offset().y();
    }

    if(copy_tiles.size()){
        uint8_t paste_x,paste_y,overlap_position;
        std::string temp_arrangement;
        std::vector<NEStile *> temp_tiles;
        std::vector<spriteEditItem *> temp_image;
        bool tile_match;
        for(i=copy_tiles.size()-1;i>=0;i--){    //draw the lowest priority tiles first.
            temp_arrangement = "";
            temp_tiles.clear();
            temp_image.clear();
            tile_match = false;
            NEStile flip_tile[2];
            flip_tile[0] = copy_tiles.at(i).tile[0];
            flip_tile[1] = copy_tiles.at(i).tile[1];
            flip_tile[0].hflip();
            flip_tile[1].hflip();
            if(copy_tiles.at(i).tile_type){
                if(copy_tiles.at(i).tile[0] == sprite_page->t[copy_tiles.at(i).tile[0].id & 0xFE] && \
                        copy_tiles.at(i).tile[1] == sprite_page->t[copy_tiles.at(i).tile[0].id | 0x01]){
                    copy_tiles.at(i).flip = false;
                    tile_match = true;
                }
                if(flip_tile[0] == sprite_page->t[copy_tiles.at(i).tile[0].id & 0xFE] && \
                        flip_tile[1] == sprite_page->t[copy_tiles.at(i).tile[0].id | 0x01]){
                    copy_tiles.at(i).flip = false;
                    tile_match = true;
                }
            }
            else{
                if(copy_tiles.at(i).tile[0] == bg_page->t[copy_tiles.at(i).tile[0].id & 0xFE] && \
                        copy_tiles.at(i).tile[1] == bg_page->t[copy_tiles.at(i).tile[0].id | 0x01]){
                    copy_tiles.at(i).flip = false;
                    tile_match = true;
                }
                if(flip_tile[0] == bg_page->t[copy_tiles.at(i).tile[0].id & 0xFE] && \
                        flip_tile[1] == bg_page->t[copy_tiles.at(i).tile[0].id | 0x01]){
                    copy_tiles.at(i).flip = false;
                    tile_match = true;
                }
            }
            if(!tile_match){
                for(j=0;j<0x100;j+=2){  //look for matching tiles in bg and sprite pages
                    if(!sprite_page->sprite_used[j] && !sprite_page->t[j].shared && !sprite_page->t[j+1].shared) continue;
                    if(copy_tiles.at(i).tile[0] == sprite_page->t[j] && copy_tiles.at(i).tile[1] == sprite_page->t[j|1]){
                        copy_tiles.at(i).tile_type = true;
                        copy_tiles.at(i).tile[0].id = j;
                        copy_tiles.at(i).flip = false;
                        break;
                    }
                    if(flip_tile[0] == sprite_page->t[j] && flip_tile[1] == sprite_page->t[j|1]){
                        copy_tiles.at(i).tile_type = true;
                        copy_tiles.at(i).tile[0].id = j;
                        copy_tiles.at(i).flip = true;
                        break;
                    }
                    if(copy_tiles.at(i).tile[0] == bg_page->t[j] && copy_tiles.at(i).tile[1] == bg_page->t[j|1]){
                        copy_tiles.at(i).tile_type = false;
                        copy_tiles.at(i).tile[0].id = j;
                        copy_tiles.at(i).flip = false;
                        break;
                    }
                    if(flip_tile[0] == bg_page->t[j] && flip_tile[1] == bg_page->t[j|1]){
                        copy_tiles.at(i).tile_type = false;
                        copy_tiles.at(i).tile[0].id = j;
                        copy_tiles.at(i).flip = true;
                        break;
                    }
                }
                if(j>=0x100){   //if no matching tiles found, look for unused tiles instead
                    for(j=0;j<0x100;j+=2){
                        if(!sprite_page->sprite_used[j] && !sprite_page->sprite_used[j|1] && \
                                !sprite_page->t[j].shared && !sprite_page->t[j|1].shared){
                            sprite_page->t[j] = copy_tiles.at(i).tile[0];
                            sprite_page->t[j|1] = copy_tiles.at(i).tile[1];
                            copy_tiles.at(i).tile[0].id = j;
                            copy_tiles.at(i).tile_type = true;
                            break;
                        }
                    }
                    if(j>=0x100){
                        for(j=0;j<0x100;j+=2){
                            if(!bg_page->sprite_used[j] && !bg_page->sprite_used[j|1] && \
                                    !bg_page->t[j].shared && !bg_page->t[j|1].shared){
                                bg_page->t[j] = copy_tiles.at(i).tile[0];
                                bg_page->t[j|1] = copy_tiles.at(i).tile[1];
                                copy_tiles.at(i).tile[0].id = j;
                                copy_tiles.at(i).tile_type = false;
                                break;
                            }
                        }
                    }
                }
            }
            if(!tile_match && j>=0x100) continue;    //Cannot match tile, and no space to make a new one
            paste_x = copy_tiles.at(i).x + pixel_x;
            paste_y = copy_tiles.at(i).y + pixel_y;
            overlap_position = overlappingTile(QPoint(paste_x,paste_y));
            for(j=0;j<image.size();j++){
                if(image.at(j)->offset().x() == paste_x) break;
            }
            if(j>overlap_position || j>=image.size()){
                for(j=0;j<0x29;j++){
                    if((uint8_t)(spritexy_values[j] + 0x80) == paste_x) break;
                }
                if(j>=0x29) continue;   //don't draw tiles that don't line up exactly in the case of an exact copy.
                temp_arrangement += (uint8_t)j;
                temp_arrangement += 0x01;
                temp_arrangement += (copy_tiles.at(i).tile[0].id & 0xFE) | ((copy_tiles.at(i).tile_type)? 1 : 0);
                for(j=0;j<0x20;j++){
                    if((uint8_t)(spritexy_values[j] + 0x80) == paste_y) break;
                }
                if(j>=0x20) continue;   //don't draw tiles that don't line up exactly in the case of an exact copy.
                temp_arrangement += (j<<3) | (copy_tiles.at(i).attribs << 1) | (copy_tiles.at(i).flip ? 1 : 0);
                for(j=0;j<edit_arrangement.arrangement.length();j++) temp_arrangement += edit_arrangement.arrangement[j];
                if(copy_tiles.at(i).tile_type){
                    temp_tiles.push_back(&sprite_page->t[copy_tiles.at(i).tile[0].id & 0xFE]);
                    temp_tiles.push_back(&sprite_page->t[copy_tiles.at(i).tile[0].id | 0x01]);
                    sprite_page->sprite_used[copy_tiles.at(i).tile[0].id & 0xFE] = true;
                    sprite_page->sprite_used[copy_tiles.at(i).tile[0].id | 0x01] = true;
                }
                else{
                    temp_tiles.push_back(&bg_page->t[copy_tiles.at(i).tile[0].id & 0xFE]);
                    temp_tiles.push_back(&bg_page->t[copy_tiles.at(i).tile[0].id | 0x01]);
                    bg_page->sprite_used[copy_tiles.at(i).tile[0].id & 0xFE] = true;
                    bg_page->sprite_used[copy_tiles.at(i).tile[0].id | 0x01] = true;
                }
                for(j=0;j<edit_arrangement.tiles.size();j++) temp_tiles.push_back(edit_arrangement.tiles.at(j));
                spriteEditItem * current_item = new spriteEditItem;
                current_item->setTileID(copy_tiles.at(i).tile[0].id);
                current_item->setID(0);
                current_item->setArrangementOffset(0);
                current_item->setAttribs(copy_tiles.at(i).attribs);
                current_item->setFlip(copy_tiles.at(i).flip);
                current_item->setTileType(copy_tiles.at(i).tile_type);
                new_scene.addItem(current_item);
                current_item->setOffset(paste_x,paste_y);
                current_item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
                current_item->setFlag((QGraphicsItem::ItemIsSelectable));
                //don't bother setting the pixmap yet
                temp_image.push_back(current_item);
                for(j=0;j<image.size();j++) temp_image.push_back(image.at(j));
            }
            else{
                //j>overlap_position || j>=image.size()
                //current value in j is the tile # of the first tile sharing the same column as the location we wish to paste to
                uint8_t arrangement_position = j;
                uint8_t column_location = edit_arrangement.locateColumnInArrangement(j);
                for(k=0;k<(column_location+1);k++) temp_arrangement += edit_arrangement.arrangement[k];
                temp_arrangement += edit_arrangement.arrangement[k++] + 1;  //add one to the # of tiles in the columm
                temp_arrangement += (copy_tiles.at(i).tile[0].id & 0xFE) | (copy_tiles.at(i).tile_type ? 1 : 0);
                for(j=0;j<0x20;j++){
                    if((uint8_t)(spritexy_values[j] + 0x80) == paste_y) break;
                }
                if(j>=0x20) continue;   //don't draw tiles that don't line up exactly in the case of an exact copy.
                temp_arrangement += (j<<3) | (copy_tiles.at(i).attribs << 1) | (copy_tiles.at(i).flip ? 1 : 0);
                for(;k<edit_arrangement.arrangement.length();k++) temp_arrangement += edit_arrangement.arrangement[k];
                for(j=0;j<(arrangement_position<<1);j++) temp_tiles.push_back(edit_arrangement.tiles.at(j));
                if(copy_tiles.at(i).tile_type){
                    temp_tiles.push_back(&sprite_page->t[copy_tiles.at(i).tile[0].id & 0xFE]);
                    temp_tiles.push_back(&sprite_page->t[copy_tiles.at(i).tile[0].id | 0x01]);
                    sprite_page->sprite_used[copy_tiles.at(i).tile[0].id & 0xFE] = true;
                    sprite_page->sprite_used[copy_tiles.at(i).tile[0].id | 0x01] = true;
                }
                else{
                    temp_tiles.push_back(&bg_page->t[copy_tiles.at(i).tile[0].id & 0xFE]);
                    temp_tiles.push_back(&bg_page->t[copy_tiles.at(i).tile[0].id | 0x01]);
                    bg_page->sprite_used[copy_tiles.at(i).tile[0].id & 0xFE] = true;
                    bg_page->sprite_used[copy_tiles.at(i).tile[0].id | 0x01] = true;
                }
                for(;j<edit_arrangement.tiles.size();j++) temp_tiles.push_back(edit_arrangement.tiles.at(j));
                for(j=0;j<arrangement_position;j++) temp_image.push_back(image.at(j));
                spriteEditItem * current_item = new spriteEditItem;
                current_item->setTileID(copy_tiles.at(i).tile[0].id);
                current_item->setID(arrangement_position);
                current_item->setArrangementOffset(arrangement_position);
                current_item->setAttribs(copy_tiles.at(i).attribs);
                current_item->setFlip(copy_tiles.at(i).flip);
                current_item->setTileType(copy_tiles.at(i).tile_type);
                new_scene.addItem(current_item);
                current_item->setOffset(paste_x,paste_y);
                current_item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
                current_item->setFlag((QGraphicsItem::ItemIsSelectable));
                //don't bother setting the pixmap yet
                temp_image.push_back(current_item);
                for(;j<image.size();j++){
                    image.at(j)->setID(j+1);
                    image.at(j)->setArrangementOffset(j+1);
                    temp_image.push_back(image.at(j));
                }
            }
            edit_arrangement.arrangement = temp_arrangement;
            edit_arrangement.tiles = temp_tiles;
            image = temp_image;
        }
        compactSlow();
        edit_arrangement.modified = true;
        undo_actions[undo_position].new_arrangement = edit_arrangement;
        undo_actions[undo_position].new_bg = *bg_page;
        undo_actions[undo_position].new_sprites = *sprite_page;
        undo_position++;
        if(undo_position>=UNDO_SIZE) undo_position -= UNDO_SIZE;
        undo_max = undo_position;
        if(undo_min == undo_max){
            undo_min++;
            if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
        }
        selected_palette = old_palette;
        selected_color = old_color;
        paste_ready = false;
        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();
        new_scene.removeItem(to_paste);
        return;
    }

    uint8_t scene_coordinate_x;
    uint8_t scene_coordinate_y;

    QList<QGraphicsItem *> target_candidates;
    QRgb * image_line;
    pals.convertColors();
    QImage temp_image = clipboard_image;

    uint8_t tile_width,tile_height;
    uint8_t tile_width_max = (temp_image.width()>>3) + ((temp_image.width()&0x7)? 1 : 0);
    uint8_t tile_height_max = (temp_image.height()>>4) + ((temp_image.height()&0xF)? 1 : 0);
    uint32_t current_diff;

    for(tile_height = 0;tile_height < tile_height_max;tile_height++){
        for(tile_width = 0;tile_width < tile_width_max;tile_width++){
            compactQuick(); //clean up duplicate tiles at each tile boundary
            //at every new tile boundary we need to identify a preferred palette
            QRgb * tile_test_line;
             for(i=0;i<4;i++){
                 min_diff = 0xffffffff;
                 current_diff = 0;
                 for(int m=0;m<16;m++){
                    if(((tile_height<<4)+m) >= temp_image.height()) break;
                    tile_test_line = (QRgb *)temp_image.scanLine((tile_height<<4)+m);
                    for(int n=0;n<8;n++){
                        if((n+(tile_width<<3)) >= temp_image.width()) break;
                        if(!QColor::fromRgba(tile_test_line[n+(tile_width<<3)]).alpha()) continue;
                        current_diff += pals.p[i].colorDiffSprite(tile_test_line[n+(tile_width<<3)]);
                    }
                 }
                 if(current_diff < min_diff){
                     min_diff = current_diff;
                     preferred_palette = i;
                 }
             }
            for(k=0;k<16;k++){
                scene_coordinate_y = pixel_y + (tile_height<<4) + k;
                if(((tile_height<<4) + k) >= temp_image.height()) break;
                image_line = (QRgb *) temp_image.scanLine((tile_height<<4) + k);
                for(l=0;l<8;l++){
                    scene_coordinate_x = pixel_x + (tile_width<<3) + l;
                    if(((tile_width<<3) + l) >= temp_image.width()) break;
                    if(!QColor::fromRgba(image_line[(tile_width<<3) + l]).alpha()) continue; //don't attempt to draw blank pixels
                    min_diff = pals.p[preferred_palette].colorDiffSprite(image_line[(tile_width<<3) + l]);
                    selected_palette = preferred_palette;
                    for(i=0;i<4;i++){
                        if(i==preferred_palette) continue;
                        current_diff = pals.p[i].colorDiffSprite(image_line[(tile_width<<3) + l]);
                        if(current_diff < min_diff){
                            min_diff = current_diff;
                            selected_palette = i;
                        }
                    }
                    selected_color = pals.p[selected_palette].bestSpriteColor(image_line[(tile_width<<3) + l]);
                    //target_candidates= new_scene.items(QPointF(scene_coordinate_x,scene_coordinate_y),Qt::IntersectsItemBoundingRect);
                    target_candidates = itemsAtPos(QPointF(scene_coordinate_x,scene_coordinate_y));
                    masks.clear();
                    for(i=0;i<target_candidates.size();i++){
                        if(target_candidates.at(i) == to_paste) continue;
                        if(((spriteEditItem *)(target_candidates.at(i)))->getID() == 0xff){
                            target = (spriteEditItem *)(target_candidates.at(i));
                            break;
                        }
                        else if(((spriteEditItem *)(target_candidates.at(i)))->getAttribs() == selected_palette){
                            target = (spriteEditItem *)(target_candidates.at(i));
                            break;
                        }
                        else{
                            int j;
                            for(j=0;j<4;j++){
                                if(pals.p[selected_palette].nes_colors[selected_color] == pals.p[((spriteEditItem *)(target_candidates.at(i)))->getAttribs()].nes_colors[j]) break;
                            }
                            if(j<4){
                                target = (spriteEditItem *)(target_candidates.at(i));
                                break;
                            }
                            //don't draw on the current tile.  Allow the mouse click to propagate to the tile beneath
                            mask_pixels current_mask;
                            x = scene_coordinate_x;
                            y = scene_coordinate_y;
                            x -= ((spriteEditItem *)(target_candidates.at(i)))->offset().x();
                            y -= ((spriteEditItem *)(target_candidates.at(i)))->offset().y();
                            arrangement_target = ((spriteEditItem *)(target_candidates.at(i)))->getID();
                            current_mask.arrangement_target = arrangement_target;
                            current_mask.x = x;
                            current_mask.y = y;
                            if(y>=8){
                                y-=8;
                                CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
                            }
                            else{
                                CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
                            }
                            current_mask.analysis = analyzeTile(CHR_target,x,y);
                            masks.push_back(current_mask);  //Create a stack of pixels currently drawn underneath the cursor.
                        }
                    }
                    if(!target) continue;
                    x = scene_coordinate_x;
                    y = scene_coordinate_y;
                    x -= target->offset().x();
                    y -= target->offset().y();
                    if((y>=16 || x >= 8) || (y < 0 || x < 0)){
                        y=0;
                        x=0;
                    }
                    arrangement_target = target->getID();
                    if(arrangement_target == 0xff){
                        if(selected_color){ //If we aren't using the universal bg color, create a new tile to match the color.  Else do nothing
                            //create new tile here
                            target = allocateNewTile(scene_coordinate_x,scene_coordinate_y);
                            if(!target) continue;
                            x = scene_coordinate_x - target->offset().x();
                            y = scene_coordinate_y - target->offset().y();
                            arrangement_target = target->getID();
                            if(y>=8){
                                y-=8;
                                CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
                            }
                            else{
                                CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
                            }
                            if((y>=16 || x >= 8) || (y < 0 || x < 0)){
                                y=0;
                                x=0;
                            }
                            CHR_target->shared = false;
                            CHR_target->t[y] = (CHR_target->t[y]&(0xff ^ (1<<(7-x)))) | ((selected_color&1)<<(7-x));
                            CHR_target->t[y+8] = (CHR_target->t[y + 8]&(0xff ^ (1<<(7-x)))) | (((selected_color&2)>>1)<<(7-x));
                            CHR_target->checksum = 0;
                            for(i=0;i<0x10;i++){
                                CHR_target->checksum += CHR_target->t[i];
                            }
                        }
                        continue;
                    }
                    for(i=0;i<masks.size();i++){ //Check if the pixel is masked by another tile
                        uint8_t temp_analysis = masks.at(i).analysis;
                        if(temp_analysis & 4){
                            //attempt to erase the pixel(s) above and draw the color where needed
                            uint8_t temp_x = masks.at(i).x;
                            uint8_t temp_y = masks.at(i).y;
                            uint8_t temp_target = masks.at(i).arrangement_target;
                            if(temp_analysis&3){
                                //****attempt to allocate new tile
                                if(!duplicateTile(temp_target)) break;
                            }
                            if(temp_y>=8){
                                temp_y-=8;
                                CHR_target = edit_arrangement.tiles.at((temp_target<<1) + 1);
                            }
                            else{
                                CHR_target = edit_arrangement.tiles.at(temp_target<<1);
                            }
                            CHR_target->t[temp_y] = CHR_target->t[temp_y]&(0xff ^ (1<<(7-temp_x))); //problem with the mask****
                            CHR_target->t[temp_y+8] = CHR_target->t[temp_y + 8]&(0xff ^ (1<<(7-temp_x))); //erases pixel next to it
                            CHR_target->checksum = 0;
                            for(int j=0;j<0x10;j++){
                                CHR_target->checksum += CHR_target->t[j];
                            }
                        }
                    }
                    if(i<masks.size()) continue;
                    uint8_t pixel_color = 0xff;
                    if(target->getAttribs() == selected_palette){
                        pixel_color = selected_color;
                    }
                    else{
                        for(i=0;i<4;i++){
                            if(pals.p[selected_palette].nes_colors[selected_color] == pals.p[target->getAttribs()].nes_colors[i]) break;
                        }
                        if(i>=4){
                            continue;    //exit.  We should never reach this point.
                        }
                        //use the matching color in the other palette instead of the selected color in selected palette
                        pixel_color = i;
                    }
                    //if(target->getAttribs() == selected_palette){
                    if(y>=8){
                        y-=8;
                        CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
                    }
                    else{
                        CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
                    }
                    NEStile temp_tile;
                    if(target->getTileType()){
                        temp_tile = sprite_page->t[CHR_target->id];
                    }
                    else{
                        temp_tile = bg_page->t[CHR_target->id];
                    }
                    temp_tile.t[y] = (temp_tile.t[y]&(0xff ^ (1<<(7-x)))) | ((pixel_color&1)<<(7-x));
                    temp_tile.t[y+8] = (temp_tile.t[y + 8]&(0xff ^ (1<<(7-x)))) | (((pixel_color&2)>>1)<<(7-x));
                    temp_tile.checksum = 0;
                    for(i=0;i<0x10;i++){
                        temp_tile.checksum += temp_tile.t[i];
                    }
                    uint16_t tile_analysis = analyzeTile(CHR_target,x,y) & 3;
                    if(tile_analysis){
                        arrangement_target = target->getID();
                        if(!duplicateTile(arrangement_target)) continue;  //attempt to allocate new tile
                        y = scene_coordinate_y;
                        y -= target->offset().y();
                        if(y>=8){
                            y-=8;
                            CHR_target = edit_arrangement.tiles.at((arrangement_target<<1) + 1);
                        }
                        else{
                            CHR_target = edit_arrangement.tiles.at(arrangement_target<<1);
                        }
                    }
                    *CHR_target = temp_tile;
                    //}
                }
            }
        }
    }
    compactSlow();
    edit_arrangement.modified = true;
    undo_actions[undo_position].new_arrangement = edit_arrangement;
    undo_actions[undo_position].new_bg = *bg_page;
    undo_actions[undo_position].new_sprites = *sprite_page;
    undo_position++;
    if(undo_position>=UNDO_SIZE) undo_position -= UNDO_SIZE;
    undo_max = undo_position;
    if(undo_min == undo_max){
        undo_min++;
        if(undo_min >= UNDO_SIZE) undo_min -= UNDO_SIZE;
    }
    selected_palette = old_palette;
    selected_color = old_color;
    paste_ready = false;
    updateCHRMask();
    drawCHR();
    drawBackground();
    drawArrangement();
    new_scene.removeItem(to_paste);
}

QList<QGraphicsItem *> editSpriteDialog::itemsAtPos(const QPointF & pos){
    QList<QGraphicsItem *> result;
    unsigned int i,j;

    for(i=0;i<image.size();i++){
        if((pos.x() >= ((spriteEditItem *)image.at(i))->offset().x()) && (pos.x() < (((spriteEditItem *)image.at(i))->offset().x() + 8))){
            if((pos.y() >= ((spriteEditItem *)image.at(i))->offset().y()) && (pos.y() < (((spriteEditItem *)image.at(i))->offset().y() + 16))){
                for(j=0;j<result.size();j++){
                    if(((spriteEditItem *)result.at(j))->zValue() < ((spriteEditItem *)image.at(i))->zValue()) break;
                }
                result.insert(j,image.at(i));
            }
        }
    }
    result.append(overlay_item);
    return result;
}

void editSpriteDialog::showContextMenu(const QPoint &pos){
    QPoint global_pos = arrangement_view->mapToGlobal(pos);

    QMenu context_menu;
    context_menu.addAction("Cut (Ctrl + X)",this,SLOT(cut_slot()));
    context_menu.addAction("Copy (Ctrl + C)",this,SLOT(copy_slot()));
    context_menu.addAction("Paste (Ctrl + V)",this,SLOT(paste_slot()));
    context_menu.addAction("Delete",this,SLOT(delete_slot()));
    QAction * selected_item = context_menu.exec(global_pos);
    if(selected_item){

    }
    else{
        //nothing was chosen
    }

}

void editSpriteDialog::accept(){
    unsigned int i;

    compactSlow();
    for(i=0;i<image.size();i++){
        if(((spriteEditItem *)image.at(i))->getTileType()){
            CHR_pages[edit_arrangement.gfx_page].t[((spriteEditItem *)image.at(i))->getTileID() & 0xFE] = \
                    sprite_page->t[((spriteEditItem *)image.at(i))->getTileID() & 0xFE];
            CHR_pages[edit_arrangement.gfx_page].t[((spriteEditItem *)image.at(i))->getTileID() | 0x01] = \
                    sprite_page->t[((spriteEditItem *)image.at(i))->getTileID() | 0x01];
            edit_arrangement.tiles.at(i<<1) = &CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at(i<<1)->id];
            edit_arrangement.tiles.at((i<<1)+1) = &CHR_pages[edit_arrangement.gfx_page].t[edit_arrangement.tiles.at((i<<1)+1)->id];
        }
        else{
            CHR_pages[edit_arrangement.bg_gfx_page].t[((spriteEditItem *)image.at(i))->getTileID() & 0xFE] = \
                    bg_page->t[((spriteEditItem *)image.at(i))->getTileID() & 0xFE];
            CHR_pages[edit_arrangement.bg_gfx_page].t[((spriteEditItem *)image.at(i))->getTileID() | 0x01] = \
                    bg_page->t[((spriteEditItem *)image.at(i))->getTileID() | 0x01];
            edit_arrangement.tiles.at(i<<1) = &CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at(i<<1)->id];
            edit_arrangement.tiles.at((i<<1)+1) = &CHR_pages[edit_arrangement.bg_gfx_page].t[edit_arrangement.tiles.at((i<<1)+1)->id];
        }

    }
    if(ui->comboBox->currentIndex() < (ui->comboBox->count() - 1)){
        *sorted_list[ui->comboBox->currentIndex()] = edit_arrangement;
        sorted_list[ui->comboBox->currentIndex()]->modified = false;
    }
    else{
        sprites[num_sprites] = new sprite;
        *sprites[num_sprites] = edit_arrangement;
        sprites[num_sprites]->references = 0;
        sprites[num_sprites]->id = num_sprites;
        sprites[num_sprites]->modified = false;
        num_sprites++;
    }
    QDialog::accept();
}

void editSpriteDialog::delete_slot(){
    arrangement_released();
    if(last_focus == 1){
        undo_max = undo_position;
        //Setup restore point in the event of undo
        undo_actions[undo_position].old_arrangement = edit_arrangement;
        undo_actions[undo_position].old_bg = *bg_page;
        undo_actions[undo_position].old_sprites = *sprite_page;
        unsigned int i,j;
        std::string temp_arrangement = "";
        std::vector<NEStile *>temp_tiles;
        uint8_t write_position;
        uint8_t tiles_in_column;
        uint8_t temp_tiles_in_column;
        uint8_t tile_number = 0;
        uint8_t column_position;
        bool at_least_one_tile;
        for(i=0;i<edit_arrangement.arrangement.length();){
            write_position = temp_arrangement.length();
            column_position = edit_arrangement.arrangement[i++];
            tiles_in_column = edit_arrangement.arrangement[i++];
            temp_tiles_in_column = tiles_in_column;
            at_least_one_tile = false;
            for(j=0;j<tiles_in_column;j++){
                if(image.at(tile_number>>1)->isSelected()){
                    temp_tiles_in_column--;
                    i+=2;
                    tile_number+=2;
                }
                else{
                    if(!at_least_one_tile){
                        temp_arrangement+=column_position;
                        temp_arrangement+=tiles_in_column;
                        at_least_one_tile = true;
                    }
                    temp_arrangement+=edit_arrangement.arrangement[i++];
                    temp_arrangement+=edit_arrangement.arrangement[i++];
                    temp_tiles.push_back(edit_arrangement.tiles.at(tile_number++));
                    temp_tiles.push_back(edit_arrangement.tiles.at(tile_number++));
                }
            }
            if(temp_tiles_in_column){
                temp_arrangement[write_position+1] = temp_tiles_in_column;
            }
        }
        edit_arrangement.arrangement = temp_arrangement;
        edit_arrangement.tiles = temp_tiles;
        compactQuick();
        reparseImage();
        edit_arrangement.modified = true;
        undo_actions[undo_position].new_arrangement = edit_arrangement;
        undo_actions[undo_position].new_bg = *bg_page;
        undo_actions[undo_position].new_sprites = *sprite_page;
        updateCHRMask();
        drawCHR();
        drawBackground();
        drawArrangement();
    }
}

void editSpriteDialog::cut_slot(){
    copy_slot();
    delete_slot();
}

void editSpriteDialog::compactQuick(){
    unsigned int i,j;
    uint8_t tiles_in_column;
    NEStile temp_tile[2];
    uint8_t tile_count = 0;
    uint8_t tile_id;
    uint16_t temp_int;

    for(i=0;i<edit_arrangement.arrangement.length();){
        if(edit_arrangement.arrangement[i++]&0x80) break;
        tiles_in_column = edit_arrangement.arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            temp_tile[0] = *edit_arrangement.tiles.at(tile_count);
            temp_tile[1] = *edit_arrangement.tiles.at(tile_count+1);
            tile_id = image.at(tile_count>>1)->getTileID() & 0xFE;
            temp_int = tileCompareQuick(temp_tile,tile_id,image.at(tile_count>>1)->getTileType());
            if(temp_int < 0x200){
                if(edit_arrangement.arrangement[i]&1){
                    sprite_page->sprite_used[tile_id] = false;
                    sprite_page->sprite_used[tile_id+1] = false;
                }
                else{
                    bg_page->sprite_used[tile_id] = false;
                    bg_page->sprite_used[tile_id+1] = false;
                }
                if(temp_int >= 0x100){
                    temp_int -= 0x100;
                    edit_arrangement.tiles.at(tile_count) = &bg_page->t[temp_int];
                    edit_arrangement.tiles.at(tile_count+1) = &bg_page->t[temp_int+1];
                    edit_arrangement.arrangement[i] = temp_int & 0xFE;
                    image.at(tile_count>>1)->setTileID(temp_int & 0xFE);
                }
                else{
                    edit_arrangement.tiles.at(tile_count) = &sprite_page->t[temp_int];
                    edit_arrangement.tiles.at(tile_count+1) = &sprite_page->t[temp_int+1];
                    edit_arrangement.arrangement[i] = temp_int | 0x01;
                    image.at(tile_count>>1)->setTileID(temp_int | 0x01);
                }
                edit_arrangement.arrangement[i+1]=edit_arrangement.arrangement[i+1]&0xFE;   //clear the flip bit
                image.at(tile_count>>1)->setFlip(false);
                i+=2;
                tile_count+=2;
                continue;
            }
            temp_tile[0].hflip();
            temp_tile[1].hflip();
            temp_int = tileCompareQuick(temp_tile,tile_id,image.at(tile_count>>1)->getTileType());
            if(temp_int < 0x200){
                if(edit_arrangement.arrangement[i]&1){
                    sprite_page->sprite_used[tile_id] = false;
                    sprite_page->sprite_used[tile_id+1] = false;
                }
                else{
                    bg_page->sprite_used[tile_id] = false;
                    bg_page->sprite_used[tile_id+1] = false;
                }
                if(temp_int >= 0x100){
                    temp_int -= 0x100;
                    edit_arrangement.tiles.at(tile_count) = &bg_page->t[temp_int];
                    edit_arrangement.tiles.at(tile_count+1) = &bg_page->t[temp_int+1];
                    edit_arrangement.arrangement[i] = temp_int & 0xFE;
                    image.at(tile_count>>1)->setTileID(temp_int & 0xFE);
                }
                else{
                    edit_arrangement.tiles.at(tile_count) = &sprite_page->t[temp_int];
                    edit_arrangement.tiles.at(tile_count+1) = &sprite_page->t[temp_int+1];
                    edit_arrangement.arrangement[i] = temp_int | 0x01;
                    image.at(tile_count>>1)->setTileID(temp_int | 0x01);
                }
                edit_arrangement.arrangement[i+1]=edit_arrangement.arrangement[i+1] | 0x01;   //set the flip bit
                image.at(tile_count>>1)->setFlip(true);
                i+=2;
                tile_count+=2;
                continue;
            }
            i+=2;
            tile_count+=2;
        }
    }
    for(i=0;i<edit_arrangement.tiles.size();i+=2){
        if(!(edit_arrangement.tiles.at(i)->checksum) && !(edit_arrangement.tiles.at(i+1)->checksum)){
            deleteTile(i>>1);
            i=0;    //restart the loop
        }
    }
}

uint16_t editSpriteDialog::tileCompareQuick(NEStile to_test[2],uint8_t tile_id,bool is_sprite){
    unsigned int i;
    tile_id &= 0xFE;
    if(sprite_page == bg_page) is_sprite = true;
    for(i=0;i<0x100;i+=2){
        if(is_sprite && i==tile_id) continue;
        if(!sprite_page->sprite_used[i] && !sprite_page->t[i].shared){
            continue;
        }
        if((to_test[0]==sprite_page->t[i]) && (to_test[1]==sprite_page->t[i|1])) break;
    }
    if(i<0x100) return i;
    if(sprite_page == bg_page) return 0x200;
    for(i=0;i<0x100;i+=2){
        if(!is_sprite && i==tile_id) continue;
        if(!bg_page->sprite_used[i] && !bg_page->t[i].shared) continue;
        if((to_test[0]==bg_page->t[i]) && (to_test[1]==bg_page->t[i])) break;
    }
    return i+0x100;
}

void editSpriteDialog::compactSlow(){
    unsigned int i,j,k,l;
    QList<QGraphicsItem *> item_list;
    bool pixel_masked;
    bool sprite_matched;
    bool flip_matched;
    uint8_t mask_x, mask_y,mask_id;
    uint8_t tile_x, tile_y,tile_id,color_index,color_index2;
    int offset_diff_x,offset_diff_y;
    NEStile * temp_tile;
    bool complete_mask[16][8];
    uint8_t to_match_indices[16][8];
    uint8_t temp_pals[16];
    uint8_t tile_attribs;
    uint8_t palette_indices[16];
    for(i=0;i<16;i++) palette_indices[i] = i;
    palette_indices[4] = 0;
    palette_indices[8] = 0;
    palette_indices[0xC] = 0;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            temp_pals[(i<<2) + j] = pals.p[i].nes_colors[j];
        }
    }
    for(i=0;i<16;i++){
        if(!(i&0x3))continue;
        for(j=i+1;j<16;j++){
            if(!(j&0x3))continue;   //if a non-transparent color matches the universal bg color, do NOT match it to UBG, since it won't be transparent
            if(temp_pals[i]==temp_pals[j]) palette_indices[j] = palette_indices[i];
        }
    }

    compactQuick();
    for(i=0;i<image.size();i++){
        tile_id = image.at(i)->getTileID() & 0xFE;
        tile_x = image.at(i)->offset().x();
        tile_y = image.at(i)->offset().y();
        tile_attribs = image.at(i)->getAttribs();
        sprite_matched = false;
        for(j=0;j<16;j++){
            for(k=0;k<8;k++){
                pixel_masked = false;
                item_list = itemsAtPos(QPointF(tile_x + k,tile_y + j));
                for(l=0;l<item_list.size();l++){
                    if(item_list.at(l) == image.at(i) || (((spriteEditItem*)item_list.at(l))->getID() == 0xff)) break;
                    mask_x = ((spriteEditItem *)item_list.at(l))->offset().x();
                    mask_y = ((spriteEditItem *)item_list.at(l))->offset().y();
                    offset_diff_x = mask_x - tile_x;
                    offset_diff_y = mask_y - tile_y;
                    mask_x = k - offset_diff_x;
                    mask_y = j - offset_diff_y;
                    if(mask_y >= 16 || mask_x >= 8) continue;
                    mask_id = ((spriteEditItem *)item_list.at(l))->getTileID()&0xFE;
                    if(mask_y >= 8){
                        mask_id |= 1;
                        mask_y -= 8;
                    }
                    if(((spriteEditItem *)item_list.at(l))->getTileType()){
                        temp_tile = &sprite_page->t[mask_id];
                    }
                    else{
                        temp_tile = &bg_page->t[mask_id];
                    }
                    color_index = 0;
                    color_index |= (temp_tile->t[mask_y] >> (7-mask_x))&0x01;
                    color_index |= ((temp_tile->t[mask_y+8] >> (7-mask_x))&0x01)<<1;
                    if(color_index){
                        pixel_masked = true;
                        break;
                    }
                }
                complete_mask[j][k] = pixel_masked;
            }
        }
        if(image.at(i)->getTileType()){
            temp_tile = &sprite_page->t[tile_id];
        }
        else{
            temp_tile = &bg_page->t[tile_id];
        }
        for(j=0;j<8;j++){
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= (temp_tile->t[j] >> (7-k))&0x01;
                color_index |= ((temp_tile->t[j+8] >> (7-k))&0x01)<<1;
                to_match_indices[j][k] = palette_indices[(tile_attribs<<2) + color_index];
            }
        }
        if(image.at(i)->getTileType()){
            temp_tile = &sprite_page->t[tile_id | 1];
        }
        else{
            temp_tile = &bg_page->t[tile_id | 1];
        }
        for(j=0;j<8;j++){
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= (temp_tile->t[j] >> (7-k))&0x01;
                color_index |= ((temp_tile->t[j+8] >> (7-k))&0x01)<<1;
                to_match_indices[j+8][k] = palette_indices[(tile_attribs<<2) + color_index];
            }
        }
        //check if tile is completely masked or if only visible portion is transparent
        for(j=0;j<16;j++){
            for(k=0;k<8;k++){
                if(!complete_mask[j][k] && to_match_indices[j][k]) break;
            }
            if(k<8) break;
        }
        if(j>=16){
            deleteTile(i);  //eliminate fully masked/transparent tiles
            i--;
            continue;
        }
        //if we don't wish to match unmasked portions of tiles based on color, stop here
        if(!ui->checkBox_3->isChecked()) continue;
        //If tile is already shared with another image, don't attempt to compact it
        if(image.at(i)->getTileType()){
            if(sprite_page->t[image.at(i)->getTileID() & 0xFE].shared || sprite_page->t[image.at(i)->getTileID() | 0x01].shared) continue;
        }
        else{
            if(bg_page->t[image.at(i)->getTileID() & 0xFE].shared || bg_page->t[image.at(i)->getTileID() | 0x01].shared) continue;
        }
        //check sprite CHR page for matches.
        uint8_t temp_palette;
        flip_matched = false;
        for(j=0;j<0x100;j+=2){
            if(tile_id == j && image.at(i)->getTileType()) continue;
            for(temp_palette = 0;temp_palette<4;temp_palette++){
                for(k=0;k<8;k++){
                    for(l=0;l<8;l++){
                        //ignore masked pixels
                        if(complete_mask[k][l]) continue;
                        color_index = 0;
                        color_index |= (sprite_page->t[j].t[k] >> (7-l))&0x01;
                        color_index |= ((sprite_page->t[j].t[k+8] >> (7-l))&0x01)<<1;
                        if(to_match_indices[k][l] != palette_indices[(temp_palette<<2) + color_index]) break;
                    }
                    if(l<8) break;  //break here if tile does NOT match
                }
                if(k>=8){
                    for(k=0;k<8;k++){
                        for(l=0;l<8;l++){
                            //ignore masked pixels
                            if(complete_mask[k+8][l]) continue;
                            color_index = 0;
                            color_index |= (sprite_page->t[j|1].t[k] >> (7-l))&0x01;
                            color_index |= ((sprite_page->t[j|1].t[k+8] >> (7-l))&0x01)<<1;
                            if(to_match_indices[k+8][l] != palette_indices[(temp_palette<<2) + color_index]) break;
                        }
                        if(l<8) break; //break here if tile does NOT match
                    }
                    if(k>=8) break; //break here if we have found a match
                }
                //If we fail to match to an unflipped tile, attempt to match to the flipped version.
                for(k=0;k<8;k++){
                    for(l=0;l<8;l++){
                        //ignore masked pixels
                        if(complete_mask[k][7-l]) continue;
                        color_index = 0;
                        color_index |= (sprite_page->t[j].t[k] >> (7-l))&0x01;
                        color_index |= ((sprite_page->t[j].t[k+8] >> (7-l))&0x01)<<1;
                        if(to_match_indices[k][7-l] != palette_indices[(temp_palette<<2) + color_index]) break;
                    }
                    if(l<8) break;  //break here if tile does NOT match
                }
                if(k>=8){
                    for(k=0;k<8;k++){
                        for(l=0;l<8;l++){
                            //ignore masked pixels
                            if(complete_mask[k+8][7-l]) continue;
                            color_index = 0;
                            color_index |= (sprite_page->t[j|1].t[k] >> (7-l))&0x01;
                            color_index |= ((sprite_page->t[j|1].t[k+8] >> (7-l))&0x01)<<1;
                            if(to_match_indices[k+8][7-l] != palette_indices[(temp_palette<<2) + color_index]) break;
                        }
                        if(l<8) break; //break here if tile does NOT match
                    }
                    if(k>=8){
                        flip_matched = true;
                        break; //break here if we have found a match
                    }
                }
            }
            if(temp_palette<4){
                sprite_matched = true;
                break;   //we have found a match
            }
        }
        if(sprite_page != bg_page && !sprite_matched){
            //look for matches in the bg CHR page)
            for(j=0;j<0x100;j+=2){
                if(tile_id == j && !image.at(i)->getTileType()) continue;
                for(temp_palette = 0;temp_palette<4;temp_palette++){
                    for(k=0;k<8;k++){
                        for(l=0;l<8;l++){
                            //ignore masked pixels
                            if(complete_mask[k][l]) continue;
                            color_index = 0;
                            color_index |= (bg_page->t[j].t[k] >> (7-l))&0x01;
                            color_index |= ((bg_page->t[j].t[k+8] >> (7-l))&0x01)<<1;
                            if(to_match_indices[k][l] != palette_indices[(temp_palette<<2) + color_index]) break;
                        }
                        if(l<8) break;  //break here if tile does NOT match
                    }
                    if(k>=8){
                        for(k=0;k<8;k++){
                            for(l=0;l<8;l++){
                                //ignore masked pixels
                                if(complete_mask[k+8][l]) continue;
                                color_index = 0;
                                color_index |= (bg_page->t[j|1].t[k] >> (7-l))&0x01;
                                color_index |= ((bg_page->t[j|1].t[k+8] >> (7-l))&0x01)<<1;
                                if(to_match_indices[k+8][l] != palette_indices[(temp_palette<<2) + color_index]) break;
                            }
                            if(l<8) break; //break here if tile does NOT match
                        }
                        if(k>=8) break; //break here if we have found a match
                    }
                    //If we fail to match to an unflipped tile, attempt to match to the flipped version.
                    for(k=0;k<8;k++){
                        for(l=0;l<8;l++){
                            //ignore masked pixels
                            if(complete_mask[k][7-l]) continue;
                            color_index = 0;
                            color_index |= (bg_page->t[j].t[k] >> (7-l))&0x01;
                            color_index |= ((bg_page->t[j].t[k+8] >> (7-l))&0x01)<<1;
                            if(to_match_indices[k][7-l] != palette_indices[(temp_palette<<2) + color_index]) break;
                        }
                        if(l<8) break;  //break here if tile does NOT match
                    }
                    if(k>=8){
                        for(k=0;k<8;k++){
                            for(l=0;l<8;l++){
                                //ignore masked pixels
                                if(complete_mask[k+8][7-l]) continue;
                                color_index = 0;
                                color_index |= (bg_page->t[j|1].t[k] >> (7-l))&0x01;
                                color_index |= ((bg_page->t[j|1].t[k+8] >> (7-l))&0x01)<<1;
                                if(to_match_indices[k+8][7-l] != palette_indices[(temp_palette<<2) + color_index]) break;
                            }
                            if(l<8) break; //break here if tile does NOT match
                        }
                        if(k>=8){
                            flip_matched = true;
                            break; //break here if we have found a match
                        }
                    }
                }
            }
            if(temp_palette<4) break;   //we have found a match
        }
        if(j<0x100){
            //we have a match!
            k = edit_arrangement.locateTileInArrangement(i);
            edit_arrangement.arrangement[k] = (sprite_matched) ? (j|1) : j;
            edit_arrangement.arrangement[k+1] = (edit_arrangement.arrangement[k+1]&0xF9) | (temp_palette<<1);
            edit_arrangement.tiles.at(i<<1) = (sprite_matched) ? &sprite_page->t[j] : &bg_page->t[j];
            edit_arrangement.tiles.at((i<<1)+1) = (sprite_matched) ? &sprite_page->t[j|1] : &bg_page->t[j|1];
            image.at(i)->setTileID((sprite_matched) ? (j|1) : j);
            image.at(i)->setAttribs(temp_palette);
        }
    }
    reparseImage();
}

void editSpriteDialog::deleteTile(uint8_t tile_num){
    unsigned int i,j;
    std::string temp_arrangement = "";
    std::vector<NEStile *>temp_tiles;
    if(tile_num >= (edit_arrangement.tiles.size()>>1)) return;

    uint8_t column_position = edit_arrangement.locateColumnInArrangement(tile_num);
    uint8_t tile_position = edit_arrangement.locateTileInArrangement(tile_num);
    uint8_t tiles_in_column = edit_arrangement.arrangement[column_position + 1] - 1;

    for(i=0;i<(edit_arrangement.tiles.size()>>1);i++){
        if(i==tile_num) continue;
        temp_tiles.push_back(edit_arrangement.tiles.at(i<<1));
        temp_tiles.push_back(edit_arrangement.tiles.at((i<<1)+1));
    }
    for(i=0;i<column_position;i++) temp_arrangement += edit_arrangement.arrangement[i];
    if(tiles_in_column){
        temp_arrangement += edit_arrangement.arrangement[i++];  //write the byte that determines column X position
        temp_arrangement += tiles_in_column;
        for(++i;i<tile_position;i++) temp_arrangement += edit_arrangement.arrangement[i];
        i+=2;
    }
    else{
        i+=4;
    }
    for(;i<edit_arrangement.arrangement.length();i++) temp_arrangement += edit_arrangement.arrangement[i];
    edit_arrangement.arrangement = temp_arrangement;
    edit_arrangement.tiles = temp_tiles;
    new_scene.removeItem(image.at(tile_num));
    for(i=tile_num+1;i<image.size();i++){
        ((spriteEditItem *)image.at(i - 1))->copy((spriteEditItem*)image.at(i));
    }
    image.pop_back();
}

void editSpriteDialog::drawRowUsage(){
    unsigned int i,j;
    int y_position;
    uint8_t row_usage;
    QImage indicator_image(ui->label_12->width(),ui->label_12->height(),QImage::Format_ARGB32);
    QColor line_color;
    indicator_image.fill(Qt::transparent);
    QRgb * edit_line;
    uint8_t row_counts[0x100];
    for(i=0;i<0x100;i++) row_counts[i] = 0;
    for(i=0;i<image.size();i++){
        y_position = image.at(i)->offset().y();
        for(j=0;j<16;j++){
            row_counts[y_position + j]++;
        }
    }

    for(i=0;i<ui->label_12->height();i++){
        y_position = (arrangement_view->mapToScene(QPoint(0,i))).y();
        if(y_position < 0 || y_position >= 0x100) continue;
        row_usage = row_counts[y_position];
        edit_line = (QRgb *)indicator_image.scanLine(i);
        switch(row_usage){
        case 1:
        case 2:
        case 3:
        case 4:
            line_color = Qt::green;
            break;
        case 5:
        case 6:
        case 7:
            line_color = Qt::yellow;
            break;
        case 8:
            line_color = Qt::red;
            break;
        default:
            line_color = Qt::transparent;
            break;
        }
        row_usage<<=1;
        for(j=0;j<row_usage;j++){
            edit_line[j] = line_color.rgb();
        }
    }
    ui->label_12->setPixmap(QPixmap::fromImage(indicator_image));
}

void editSpriteDialog::arrangement_viewport_changed(){
    drawRowUsage();
}

uint8_t editSpriteDialog::overlappingTile(QPoint pos){
    unsigned int i;
    uint8_t tile_x,tile_y;

    for(i=0;i<image.size();i++){
        tile_x = image.at(i)->offset().x();
        tile_y = image.at(i)->offset().y();
        if((pos.x() >= (tile_x+8)) || (pos.x() <= (tile_x - 8))) continue;
        if((pos.y() >= (tile_y+16)) || (pos.y() <= (tile_y - 16))) continue;
        return i;
    }
    return 0xff;
}
