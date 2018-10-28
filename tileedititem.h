#ifndef TILEEDITITEM_H
#define TILEEDITITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>

class tileEditItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    tileEditItem();
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    void setID(uint8_t);
    void setTileID(uint8_t);
    uint8_t getTileID();
    uint8_t getID();
    void setAttribs(uint8_t);
    void setArrangementOffset(uint8_t);
    void setFlip(bool);
    uint8_t getAttribs();
    uint8_t getArrangementOffset();
    bool getFlip();
    void setTileType(bool);
    bool getTileType();
signals:
    void tile_pressed(QGraphicsSceneMouseEvent *);
    void tile_released();
private:
    uint8_t id;
    uint8_t tile_id;
    uint8_t arrangement_offset;
    uint8_t attribs;
    bool flip;
    bool sprite_tile;
};

#endif // TILEEDITITEM_H
