#include "cmd_music.h"

cmd_music::cmd_music()
{
    command = 0x06;
}


bool cmd_music::killSignal(){
    return false;
}

uint32_t cmd_music::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    track_number = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_music::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = track_number;
    return location;
}

std::string cmd_music::writeToTXT(){
    std::string result = "MUSIC " + convertByteToHexString(track_number) + '\n';
    return result;
}

cmd_music::~cmd_music(){

}

void cmd_music::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset;
    int32_t temp_int;
    line_offset = line.find("MUSIC") + 5;
    temp_int = getIntFromTXT(line,line_offset);
    track_number = (temp_int>=0) ? temp_int : 2;
}
