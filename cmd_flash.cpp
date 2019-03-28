#include "cmd_flash.h"

cmd_flash::cmd_flash()
{
    command = 0x01;
}

bool cmd_flash::killSignal(){
    return false;
}

uint32_t cmd_flash::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_flash::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    return location;
}

std::string cmd_flash::writeToTXT(){
    std::string result = "FLASH\n";
    return result;
}

cmd_flash::~cmd_flash(){

}

void cmd_flash::parseText(std::string text,std::string line,uint32_t & offset){

}

QString cmd_flash::getName(){
    return QString("FLASH");
}
