#ifndef SPRITEEDITITEM_H
#define SPRITEEDITITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>

class spriteEditItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    spriteEditItem();
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
private:
    uint8_t id;
    uint8_t tile_id;
    uint8_t arrangement_offset;
    uint8_t attribs;
    bool flip;
    bool sprite_tile;
};


#endif // SPRITEEDITITEM_H
