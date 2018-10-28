#ifndef ARRANGEMENTEDITITEM_H
#define ARRANGEMENTEDITITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class arrangementEditItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    arrangementEditItem(QObject *);
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void setID(uint16_t);
    void setTileID(uint8_t);
    uint16_t getID();
    uint8_t getTileID();
    void storePosition(uint16_t,uint16_t);
    uint16_t getX();
    uint16_t getY();

signals:
    void arrangement_pressed();
    void arrangement_mouse_move();
    void arrangement_mouse_release();
private:
    uint16_t id;
    uint8_t tile_id;
    uint16_t real_x,real_y;
};

#endif // ARRANGEMENTEDITITEM_H
