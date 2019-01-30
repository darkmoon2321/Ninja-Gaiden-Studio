#ifndef IMPORTBGDIALOG_H
#define IMPORTBGDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "data_types.h"
#include "palette_set_selector.h"
#include "chritem.h"
#include "tileedititem.h"
#include <QGraphicsSceneMouseEvent>
#include "arrangementedititem.h"
#include <vector>
#include <QShortcut>
#include <QClipboard>
#include "resizebgdialog.h"
#include <QMenu>

namespace Ui {
class importBGdialog;
}

class importBGdialog : public QDialog
{
    Q_OBJECT

public:
    explicit importBGdialog(QWidget *parent = 0);
    ~importBGdialog();
    void drawCHR();
    void drawTile();

private slots:
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_2_released();
    void on_color_selected(uint8_t,uint8_t);
    void on_checkBox_toggled(bool checked);
    void chr_selection_changed();
    void tile_clicked(QGraphicsSceneMouseEvent *);
    void tile_released();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void arrangement_clicked();
    void copy_slot();
    void paste_slot();
    void undo_slot();
    void redo_slot();
    void clipboard_changed();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_3_clicked();
    void resizeEvent(QResizeEvent *);
    void verticalScroll(int);
    void horizontalScroll(int);
    void showContextMenu(const QPoint &);
    void duplicateImage();

private:
    const static int UNDO_SIZE = 10;

    struct undo_action{
        int action_type;
        void * data;
    };
    struct undo_draw{
        NEStile old_tile;
        NEStile new_tile;
    };
    struct undo_paste{
        CHR_page old_page;
        CHR_page new_page;
        bg_arrangement old_arrangement;
        bg_arrangement new_arrangement;
    };
    struct undo_palette{
        std::string old_attribs;
        std::string new_attribs;
    };
    struct undo_resize{
        bg_arrangement old_arrangement;
        bg_arrangement new_arrangement;
    };

    Ui::importBGdialog *ui;
    palette_set_selector * bg_palettes;
    QGraphicsScene new_scene;
    QGraphicsPixmapItem * clipboard_item;
    QImage clipboard_image;
    bg_arrangement * sorted_list[0x100];
    QShortcut * copy_shortcut;
    QShortcut * paste_shortcut;
    QShortcut * undo_shortcut;
    QShortcut * redo_shortcut;
    QClipboard * clipboard;

    QImage tiles[4][0x100];
    chrItem * chr_array[0x100];
    QGraphicsScene chr_scene;
    QGraphicsScene tile_scene;
    QGraphicsScene clipboard_scene;
    QImage indicators;
    QGraphicsPixmapItem * indicators_pix_item;
    tileEditItem * current_tile;
    std::vector <arrangementEditItem *> image;
    QImage tile_selector;
    QGraphicsPixmapItem * tile_selector_pix_item;

    uint8_t selected_palette;
    uint8_t selected_color;
    uint8_t selected_tile;
    int16_t arrangement_tile;
    CHR_page page;
    palette_set pals;
    bg_arrangement edit_arrangement;
    int8_t last_focus;
    uint16_t last_arrangement_tile;
    bool stamp_mode;
    uint8_t old_combo_index;

    undo_action undo_actions[UNDO_SIZE];
    int undo_position;
    int undo_max;
    int undo_min;
    bool tile_edit_progress;
    QPoint widget_offsets[0x18];

    void resizeArrangement(uint8_t,uint8_t);
    void drawArrangement();
    void accept();
    void fillTransparent(const QImage & base,QImage & target);
    uint32_t colorDiff(uint8_t, QImage **);
    NEStile createNESTile(uint8_t,QImage *);
    void updateCHRMask();
};

#endif // IMPORTBGDIALOG_H
