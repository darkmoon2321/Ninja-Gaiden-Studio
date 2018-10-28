#include "arrangementedititem.h"

arrangementEditItem::arrangementEditItem(QObject *parent) : QObject(parent)
{

}

void arrangementEditItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
    QGraphicsPixmapItem::mousePressEvent(event);
    setSelected(true);
    emit arrangement_pressed();
    event->ignore();
}

/*void arrangementEditItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event){
    QGraphicsPixmapItem::mouseMoveEvent(event);
    if(event->button() == Qt::LeftButton){
        emit arrangement_mouse_move();
    }
}

void arrangementEditItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    emit arrangement_mouse_release();
}*/

void arrangementEditItem::setID(uint16_t i){
    id = i;
}

void arrangementEditItem::setTileID(uint8_t i){
    tile_id = i;
}

uint16_t arrangementEditItem::getID(){
    return id;
}

uint8_t arrangementEditItem::getTileID(){
    return tile_id;
}

void arrangementEditItem::storePosition(uint16_t x_in,uint16_t y_in){
    real_x = x_in;
    real_y = y_in;
}

uint16_t arrangementEditItem::getX(){
    return real_x;
}

uint16_t arrangementEditItem::getY(){
    return real_y;
}
