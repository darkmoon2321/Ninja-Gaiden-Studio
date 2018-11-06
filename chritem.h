#ifndef CHRITEM_H
#define CHRITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class chrItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    chrItem();
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void showSelected();
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void setID(uint8_t);
    uint8_t getID();
signals:
    void chr_pressed();
    void chr_selected();
private:
    uint8_t id;
};

#endif // CHRITEM_H
