#include "cmd_setup_hard_mode.h"

cmd_setup_hard_mode::cmd_setup_hard_mode()
{
    command = 0x12;
    unused_var = 0xff;
}

bool cmd_setup_hard_mode::killSignal(){
    return false;
}

uint32_t cmd_setup_hard_mode::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    unused_var = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_setup_hard_mode::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = unused_var;
    return location;
}

std::string cmd_setup_hard_mode::writeToTXT(){
    std::string result = "SETUP_HARD_MODE\n";
    return result;
}

cmd_setup_hard_mode::~cmd_setup_hard_mode(){

}

void cmd_setup_hard_mode::parseText(std::string text,std::string line,uint32_t & offset){

}
