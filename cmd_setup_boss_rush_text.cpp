#include "cmd_setup_boss_rush_text.h"

cmd_setup_boss_rush_text::cmd_setup_boss_rush_text()
{
    command = 0x13;
    unused_var = 0xff;
}

bool cmd_setup_boss_rush_text::killSignal(){
    return false;
}

uint32_t cmd_setup_boss_rush_text::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    unused_var = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_setup_boss_rush_text::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = unused_var;
    return location;
}

std::string cmd_setup_boss_rush_text::writeToTXT(){
    std::string result = "SETUP_BOSS_RUSH_TEXT\n";
    return result;
}

cmd_setup_boss_rush_text::~cmd_setup_boss_rush_text(){

}

void cmd_setup_boss_rush_text::parseText(std::string text,std::string line,uint32_t & offset){

}
