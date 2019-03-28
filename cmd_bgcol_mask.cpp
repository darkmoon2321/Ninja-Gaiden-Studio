#include "cmd_bgcol_mask.h"

cmd_BGCOL_MASK::cmd_BGCOL_MASK()
{
    command = 0x0A;
}

bool cmd_BGCOL_MASK::killSignal(){
    return false;
}

uint32_t cmd_BGCOL_MASK::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    color = data[scene_pointer++];
    current_palettes[0].nes_ubg = color;
    current_palettes[1].nes_ubg = color;
    mask_bits = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_BGCOL_MASK::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = color;
    data[location++] = mask_bits;
    return location;
}

std::string cmd_BGCOL_MASK::writeToTXT(){
    std::string result = "BGCOLOR " + convertByteToHexString(color) + " PPUMASKBITS " + convertByteToHexString(mask_bits) + '\n';
    return result;
}

cmd_BGCOL_MASK::~cmd_BGCOL_MASK(){

}

void cmd_BGCOL_MASK::parseText(std::string text,std::string line,uint32_t & offset){
    int32_t temp_int;
    uint32_t line_offset = line.find("BGCOLOR") + 7;
    temp_int = getIntFromTXT(line,line_offset);
    color = (temp_int>=0)?temp_int&0x3F : 0x0F;
    current_palettes[0].nes_ubg = color;
    current_palettes[1].nes_ubg = color;
    if(line.find("PPUMASKBITS") == std::string::npos){
        mask_bits = 0;
        return;
    }
    line_offset = line.find("PPUMASKBITS") + 11;
    temp_int = getIntFromTXT(line,line_offset);
    mask_bits = (temp_int>=0) ? temp_int : 0;
}

QString cmd_BGCOL_MASK::getName(){
    return QString("BGCOLOR_MASK");
}
