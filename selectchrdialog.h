#ifndef SELECTCHRDIALOG_H
#define SELECTCHRDIALOG_H

#include <QDialog>
#include <data_types.h>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "chritem.h"

namespace Ui {
class selectCHRDialog;
}

class selectCHRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit selectCHRDialog(QWidget *parent = 0,uint8_t page_num = 0);
    ~selectCHRDialog();
    uint8_t getSelectedPage();

private slots:
void tile_changed();

void on_checkBox_2_clicked();

void on_comboBox_currentIndexChanged(int index);

void on_checkBox_clicked();

void on_comboBox_2_currentIndexChanged(int index);

private:
    struct SHARED_FLAGS{
        bool shared[0x100];
    };

    Ui::selectCHRDialog *ui;
    void drawCHR();
    QImage tiles[0x100];
    CHR_page * page;
    chrItem * chr_array[0x100];
    QGraphicsScene page_scene;
    uint8_t selected_tile;
    std::vector<bg_arrangement *> bg_list;
    std::vector<sprite *> sprite_list;
    std::vector<ppu_string *> dynamic_list;
    std::vector<bool> overwrite_list;
    QImage overwriteable_image;
    QGraphicsPixmapItem * overwriteable_pixmap;
    SHARED_FLAGS * old_pages;
    uint8_t selected_page;

    void updateCHRMask();
    void accept();
    void reject();
};

#endif // SELECTCHRDIALOG_H
