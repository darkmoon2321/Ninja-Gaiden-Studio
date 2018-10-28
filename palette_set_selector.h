#ifndef PALETTE_SET_SELECTOR_H
#define PALETTE_SET_SELECTOR_H

#include <qwidget.h>
#include <QPushButton>
#include <QLabel>
#include "data_types.h"
#include "color_picker.h"
#include "dualbutton.h"
#include <QMouseEvent>
#include <QBitmap>

class palette_set_selector  : public QWidget
{
    Q_OBJECT

public:
    palette_set_selector(QWidget *, int, int,palette_set);
    ~palette_set_selector();
    void redraw();
    void setPalette(palette_set);
    palette_set getPalette();
    void setSelectedPalette(uint8_t);
    void moveEvent(QMoveEvent *event);
signals:
    void color_changed(uint8_t,uint8_t);
private slots:
    void colorPushed(QMouseEvent *);
private:
    palette_set p;
    QWidget * parent;
    int x;
    int y;
    dualButton buttons[13];
    QLabel labels[5];
    bool edit_mode;
    QLabel * color_border;
    uint8_t current_color;
    uint8_t current_palette;
    QPixmap color_border_pix;
};

#endif // PALETTE_SET_SELECTOR_H
