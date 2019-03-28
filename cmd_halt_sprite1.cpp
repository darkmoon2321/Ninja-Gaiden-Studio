#include "cmd_halt_sprite1.h"

cmd_halt_sprite1::cmd_halt_sprite1()
{
    command = 0x15;
}

bool cmd_halt_sprite1::killSignal(){
    return false;
}

uint32_t cmd_halt_sprite1::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    speed = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_halt_sprite1::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = speed;
    return location;
}

std::string cmd_halt_sprite1::writeToTXT(){
    std::string result = "SPRITE1 SPEED " + convertByteToHexString(speed) + "\n";
    return result;
}

cmd_halt_sprite1::~cmd_halt_sprite1(){

}

void cmd_halt_sprite1::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset = 0;
    speed = 0;
    if(std::string::npos != line.find("SPEED")){
        line_offset = line.find("SPEED") + 5;
        speed = getIntFromTXT(line,line_offset);
    }
}

QString cmd_halt_sprite1::getName(){
    return QString("SPRITE1_SPEED");
}
