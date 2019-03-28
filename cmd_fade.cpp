#include "cmd_fade.h"

cmd_fade::cmd_fade()
{
    command = 0x04;
}

bool cmd_fade::killSignal(){
    return false;
}

uint32_t cmd_fade::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint32_t temp = data[scene_pointer++];
    fade_in = temp&0x80;
    fade_sprites = temp&0x01;
    fade_bg = temp&0x02;
    fade_timer = (temp&0x7F)>>2;
    return scene_pointer;
}

uint32_t cmd_fade::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    uint8_t temp = fade_timer<<2;
    if(fade_sprites) temp |= 0x01;
    if(fade_bg) temp |= 0x02;
    if(fade_in) temp |= 0x80;
    data[location++] = temp;
    return location;
}

std::string cmd_fade::writeToTXT(){
    std::string result = "FADE ";
    result += (fade_in)? "IN " : "OUT ";
    if(fade_bg){
        if(fade_sprites){
            result+="ALL";
        }
        else{
            result+="BG";
        }
    }
    else{
        if(fade_sprites){
            result+="SPRITES";
        }
        else{
            result+="ALLGLITCH";
        }
    }
    result += ", RATE:" + convertByteToHexString(fade_timer) + '\n';
    return result;
}

cmd_fade::~cmd_fade(){

}

void cmd_fade::parseText(std::string text,std::string line,uint32_t & offset){
    //FADE OUT ALL, RATE:05
    fade_in = true;
    fade_bg = false;
    fade_sprites = false;
    fade_timer = 0;
    if(std::string::npos != line.find("OUT")){
        fade_in = false;
    }
    if(std::string::npos != line.find("GLITCH")){

    }
    else if(std::string::npos != line.find("ALL")){
        fade_bg = true;
        fade_sprites = true;
    }
    else if(std::string::npos != line.find("BG")){
        fade_bg = true;
    }
    else if(std::string::npos != line.find("SPRITES")){
        fade_sprites = true;
    }
    if(std::string::npos != line.find("RATE")){
        uint32_t line_offset = line.find("RATE") + 5;
        int32_t temp_int = getIntFromTXT(line,line_offset);
        fade_timer = temp_int & 0x1F;
    }
}

QString cmd_fade::getName(){
    return QString("FADE");
}
