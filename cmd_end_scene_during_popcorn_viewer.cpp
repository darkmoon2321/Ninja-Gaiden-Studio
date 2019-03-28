#include "cmd_end_scene_during_popcorn_viewer.h"

cmd_end_scene_during_popcorn_viewer::cmd_end_scene_during_popcorn_viewer()
{
    command = 0x14;
    unused_var = 0xff;
}

bool cmd_end_scene_during_popcorn_viewer::killSignal(){
    return false;
}

uint32_t cmd_end_scene_during_popcorn_viewer::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    unused_var = data[scene_pointer++];
    return scene_pointer;
}

uint32_t cmd_end_scene_during_popcorn_viewer::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = unused_var;
    return location;
}

std::string cmd_end_scene_during_popcorn_viewer::writeToTXT(){
    std::string result = "END_SCENE_DURING_POPCORN_VIEWER\n";
    return result;
}

cmd_end_scene_during_popcorn_viewer::~cmd_end_scene_during_popcorn_viewer(){

}

void cmd_end_scene_during_popcorn_viewer::parseText(std::string text,std::string line,uint32_t & offset){

}

QString cmd_end_scene_during_popcorn_viewer::getName(){
    return QString("END_SCENE_DURING_POPCORN_VIEWWER");
}
