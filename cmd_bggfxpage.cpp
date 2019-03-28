#include "cmd_bggfxpage.h"

cmd_BGGFXpage::cmd_BGGFXpage()
{
    command = 0x08;
}

bool cmd_BGGFXpage::killSignal(){
    return false;
}

uint32_t cmd_BGGFXpage::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    lower = data[scene_pointer] & 0x80;
    chr_page = data[scene_pointer++] & 0x3F;
    bg_CHR_page = chr_page;
    image_changed = true;
    return scene_pointer;
}

uint32_t cmd_BGGFXpage::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = chr_page | ((lower)?0x80:00);
    return location;
}

std::string cmd_BGGFXpage::writeToTXT(){
    std::string result = "BG_GFXPAGE ";
    if(lower) result += "LOW ";
    result += convertByteToHexString(chr_page) + '\n';
    return result;
}

cmd_BGGFXpage::~cmd_BGGFXpage(){

}

void cmd_BGGFXpage::parseText(std::string text,std::string line,uint32_t & offset){
    int32_t temp_int;
    uint32_t line_offset = line.find("BG_GFXPAGE") + 10;
    temp_int = getIntFromTXT(line,line_offset);
    chr_page = temp_int&0x3F;   //If an integer was not found, temp_int will be FF
    lower = (std::string::npos == line.find("LOW")) ? false : true;
    bg_CHR_page = chr_page;
}

QString cmd_BGGFXpage::getName(){
    return QString("BG_GFXPAGE");
}
