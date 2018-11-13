#include "spriteedititem.h"

spriteEditItem::spriteEditItem()
{

}

void spriteEditItem::setID(uint8_t i){
    id = i;
}

uint8_t spriteEditItem::getID(){
    return id;
}

void spriteEditItem::setAttribs(uint8_t arg){
    attribs = arg;
}

void spriteEditItem::setArrangementOffset(uint8_t arg){
    arrangement_offset = arg;
}

uint8_t spriteEditItem::getAttribs(){
    return attribs;
}

uint8_t spriteEditItem::getArrangementOffset(){
    return arrangement_offset;
}

void spriteEditItem::setFlip(bool arg){
    flip = arg;
}

bool spriteEditItem::getFlip(){
    return flip;
}

void spriteEditItem::setTileType(bool arg){
    sprite_tile = arg;
}

bool spriteEditItem::getTileType(){
    return sprite_tile;
}

void spriteEditItem::setTileID(uint8_t val){
    tile_id = val;
}

uint8_t spriteEditItem::getTileID(){
    return tile_id;
}

spriteEditItem* spriteEditItem::copy(spriteEditItem * to_copy){
    tile_id = to_copy->tile_id;
    attribs = to_copy->attribs;
    flip = to_copy->flip;
    sprite_tile = to_copy->sprite_tile;
    setOffset(to_copy->offset());
}
