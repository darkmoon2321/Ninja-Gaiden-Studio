#include "cmd_blackout.h"

cmd_blackout::cmd_blackout()
{
    command = 0x0E;
}

bool cmd_blackout::killSignal(){
    return false;
}

uint32_t cmd_blackout::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint8_t temp = data[scene_pointer++];
    erase_bg = temp&0x80;
    erase_sprites = temp&0x40;
    return scene_pointer;
}

uint32_t cmd_blackout::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    uint8_t temp = 0;
    if(erase_bg) temp |= 0x80;
    if(erase_sprites) temp |= 0x40;
    data[location++] = temp;
    return location;
}

std::string cmd_blackout::writeToTXT(){
    std::string result = "BLACKOUT ";
    if(erase_bg){
        if(erase_sprites){
            result += "ALL\n";
        }
        else{
            result += "BG\n";
        }
    }
    else{
        if(erase_sprites){
            result += "SPRITES\n";
        }
        else{
            result += "GLITCH\n";
        }
    }
    return result;
}

cmd_blackout::~cmd_blackout(){

}

void cmd_blackout::parseText(std::string text,std::string line,uint32_t & offset){
    erase_bg = false;
    erase_sprites = false;
    if(std::string::npos != line.find("GLITCH")){
        return;
    }
    if(std::string::npos != line.find("ALL")){
        erase_bg = true;
        erase_sprites = true;
    }
    if(std::string::npos != line.find("BG")){
        erase_bg = true;
    }
    if(std::string::npos != line.find("SPRITE")){
        erase_sprites = true;
    }
}

QString cmd_blackout::getName(){
    return QString("BLACKOUT");
}
