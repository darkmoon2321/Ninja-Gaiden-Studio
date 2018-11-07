#ifndef EDITSPRITEDIALOG_H
#define EDITSPRITEDIALOG_H

#include <QDialog>
#include "data_types.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "palette_set_selector.h"
#include "chritem.h"
#include "spriteedititem.h"
#include "spriteview.h"
#include <vector>
#include <QShortcut>
#include <QClipboard>
#include <QObjectList>
#include "arrangementedititem.h"
#include <QMenu>

namespace Ui {
class editSpriteDialog;
}

class editSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editSpriteDialog(QWidget *parent = 0);
    ~editSpriteDialog();
private slots:
    void on_color_selected(uint8_t,uint8_t);
    void bg_chr_selection_changed();
    void sprite_chr_selection_changed();
    void bg_chr_selection_noclick();
    void sprite_chr_selection_noclick();
    void copy_slot();
    void paste_slot();
    void delete_slot();
    void cut_slot();
    void undo_slot();
    void redo_slot();
    void clipboard_changed();
    void on_comboBox_currentIndexChanged(int index);
    void horizontalScroll(int);
    void verticalScroll(int);
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);
    void arrangement_clicked(QMouseEvent *);
    void arrangement_released();
    void on_comboBox_2_currentIndexChanged(int index);
    void on_comboBox_3_currentIndexChanged(int index);
    void on_spinBox_editingFinished();
    void on_spinBox_2_editingFinished();
    void on_verticalSlider_sliderMoved(int position);
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void resizeEvent(QResizeEvent *);
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void paste_position(QMouseEvent *);
    void showContextMenu(const QPoint&);

private:
    Ui::editSpriteDialog *ui;

    const static int UNDO_SIZE = 10;

    struct undo_data{
        CHR_page old_bg;
        CHR_page new_bg;
        CHR_page old_sprites;
        CHR_page new_sprites;
        sprite old_arrangement;
        sprite new_arrangement;
    };

    undo_data undo_actions[UNDO_SIZE];
    CHR_page sprite_page;
    CHR_page bg_page;
    palette_set pals;
    int undo_position;
    int undo_max;
    int undo_min;
    QPoint * widget_offsets;
    sprite edit_arrangement;
    QImage bg_indicators;
    QImage sprite_indicators;
    QImage tile_selector;
    QImage clipboard_image;
    QImage bg_tiles[4][0x100];
    QImage sprite_tiles[4][0x100];
    QImage arrangement_background;
    QGraphicsPixmapItem * tile_selector_pix_item;
    QGraphicsPixmapItem * bg_indicators_pix_item;
    QGraphicsPixmapItem * sprite_indicators_pix_item;
    QGraphicsPixmapItem * clipboard_item;
    QGraphicsPixmapItem * to_paste;
    spriteEditItem * overlay_item;
    palette_set_selector * sprite_palettes;
    sprite * sorted_list[0x100];
    bg_arrangement * sorted_bgs[0x100];
    QGraphicsScene new_scene;
    QGraphicsScene bg_CHR_scene;
    QGraphicsScene sprite_CHR_scene;
    QGraphicsScene clipboard_scene;
    spriteView * arrangement_view;
    uint8_t selected_palette;
    uint8_t selected_color;
    uint8_t selected_tile;
    uint8_t last_focus;
    uint8_t old_combo_index;
    int16_t arrangement_tile;
    bool sprite_tile_selected;
    bool edit_progress;
    bool draw_mode;
    bool paste_ready;
    chrItem * bg_chr_array[0x100];
    chrItem * sprite_chr_array[0x100];
    QShortcut * cut_shortcut;
    QShortcut * copy_shortcut;
    QShortcut * paste_shortcut;
    QShortcut * undo_shortcut;
    QShortcut * redo_shortcut;
    QShortcut * delete_shortcut;
    QClipboard * clipboard;
    QList <QWidget *> widget_list;
    std::vector <spriteEditItem *> image;
    uint16_t ppu_bases_possible[120];
    uint8_t ppu_base_chosen;

    void updateCHRMask();
    void drawArrangement();
    void drawCHR();
    void drawBackground();
    void reparseImage();
    uint16_t analyzeTile(NEStile *, uint8_t, uint8_t);
    spriteEditItem * allocateNewTile(int,int);
    bool duplicateTile(uint8_t);
    void drawPixel(uint8_t,uint8_t);
    void paste(QMouseEvent *);
    QList<QGraphicsItem *> itemsAtPos(const QPointF & pos);
    void accept();
};

#endif // EDITSPRITEDIALOG_H
