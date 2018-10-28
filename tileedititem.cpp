#include "tileedititem.h"

tileEditItem::tileEditItem()
{

}

void tileEditItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
    emit tile_pressed(event);
}

void tileEditItem::setID(uint8_t i){
    id = i;
}

uint8_t tileEditItem::getID(){
    return id;
}

void tileEditItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event){
    if(event->buttons() == Qt::LeftButton){
        emit tile_pressed(event);
    }
}

void tileEditItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
    emit tile_released();
}

void tileEditItem::setAttribs(uint8_t arg){
    attribs = arg;
}

void tileEditItem::setArrangementOffset(uint8_t arg){
    arrangement_offset = arg;
}

uint8_t tileEditItem::getAttribs(){
    return attribs;
}

uint8_t tileEditItem::getArrangementOffset(){
    return arrangement_offset;
}

void tileEditItem::setFlip(bool arg){
    flip = arg;
}

bool tileEditItem::getFlip(){
    return flip;
}

void tileEditItem::setTileType(bool arg){
    sprite_tile = arg;
}

bool tileEditItem::getTileType(){
    return sprite_tile;
}

void tileEditItem::setTileID(uint8_t val){
    tile_id = val;
}

uint8_t tileEditItem::getTileID(){
    return tile_id;
}
