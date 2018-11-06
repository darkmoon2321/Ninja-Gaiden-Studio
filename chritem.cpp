#include "chritem.h"

chrItem::chrItem()
{

}

void chrItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
    emit chr_pressed();
}

void chrItem::showSelected(){
    emit chr_selected();
}

void chrItem::setID(uint8_t i){
    id = i;
}

uint8_t chrItem::getID(){
    return id;
}

