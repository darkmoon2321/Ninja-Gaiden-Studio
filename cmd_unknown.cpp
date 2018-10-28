#include "cmd_unknown.h"

cmd_unknown::cmd_unknown()
{

}

bool cmd_unknown::killSignal(){
    return false;
}

uint32_t cmd_unknown::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    var = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_unknown::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = var;
    return location;
}

std::string cmd_unknown::writeToTXT(){
    std::string result = "UNKNOWN " + convertByteToHexString(var) + '\n';
    return result;
}

cmd_unknown::~cmd_unknown(){

}

void cmd_unknown::parseText(std::string text,std::string line,uint32_t & offset){

}
