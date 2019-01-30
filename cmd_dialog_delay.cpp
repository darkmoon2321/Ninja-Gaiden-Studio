#include "cmd_dialog_delay.h"

cmd_dialog_delay::cmd_dialog_delay()
{
    command = 0x16;
}


bool cmd_dialog_delay::killSignal(){
    return false;
}

uint32_t cmd_dialog_delay::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    delay = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_dialog_delay::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = delay;
    return location;
}

std::string cmd_dialog_delay::writeToTXT(){
    std::string result = "DIALOG_DELAY ";
    result += convertByteToHexString(delay);
    result += '\n';
    return result;
}

cmd_dialog_delay::~cmd_dialog_delay(){

}

void cmd_dialog_delay::parseText(std::string text,std::string line,uint32_t & offset){
    delay = 0;
    uint32_t line_offset;
    line_offset = line.find("DIALOG_DELAY ") + 13;
    delay = getIntFromTXT(line,line_offset);
}
