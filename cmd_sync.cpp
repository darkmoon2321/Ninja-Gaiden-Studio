#include "cmd_sync.h"

cmd_sync::cmd_sync()
{
    command = 0x02;
}

bool cmd_sync::killSignal(){
    return false;
}

uint32_t cmd_sync::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_sync::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    return location;
}

std::string cmd_sync::writeToTXT(){
    std::string result = "SYNC\n";
    return result;
}

cmd_sync::~cmd_sync(){

}

void cmd_sync::parseText(std::string text,std::string line,uint32_t & offset){

}

QString cmd_sync::getName(){
    return QString("SYNC");
}
