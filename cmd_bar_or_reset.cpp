#include "cmd_bar_or_reset.h"

cmd_bar_or_reset::cmd_bar_or_reset()
{
    command = 0x11;
    unused_var = 0xff;
}

bool cmd_bar_or_reset::killSignal(){
    return false;
}

uint32_t cmd_bar_or_reset::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    unused_var = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_bar_or_reset::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = unused_var;
    return location;
}

std::string cmd_bar_or_reset::writeToTXT(){
    std::string result = "BAR_OR_RESET\n";
    return result;
}

cmd_bar_or_reset::~cmd_bar_or_reset(){

}

void cmd_bar_or_reset::parseText(std::string text,std::string line,uint32_t & offset){

}

QString cmd_bar_or_reset::getName(){
    return QString("BAR_OR_RESET");
}
