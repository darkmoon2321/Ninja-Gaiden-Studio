#include "cmd_mirroring.h"

cmd_mirroring::cmd_mirroring()
{
    command = 0x05;
}

bool cmd_mirroring::killSignal(){
    return false;
}

/*NOTE: THIS COMMAND MAY DIFFER SLIGHTLY FROM ORIGINAL NG.
 * ARGUMENT VALUES USED TO BE EITHER 1E OR 1F, BUT DUE TO MAPPER CHANGE,
 * ONLY BIT 0 IS CONSIDERED, AND ARGUMENTS OF 00 OR 01 ARE VALID.
 * IT IS PROBABLY BEST TO CONTINUE USING 1E OR 1F INSTEAD, OR REVERT
 * TO THE ORIGINAL.  LIKELY COPIED TO PPU_CTRL, BUT NEED TO CHECK
 * ORIGINAL NG.
 * */

uint32_t cmd_mirroring::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    value = data[scene_pointer++];
    horizontal = value&1;
    return scene_pointer;
}

uint32_t cmd_mirroring::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = value;
    return location;
}

std::string cmd_mirroring::writeToTXT(){
    std::string result = "MIRRORING ";
    result += (horizontal)? "HORIZONTAL\n" : "VERTICAL\n";
    return result;
}

cmd_mirroring::~cmd_mirroring(){

}

void cmd_mirroring::parseText(std::string text,std::string line,uint32_t & offset){
    horizontal = false;
    if(std::string::npos != line.find("HORIZONTAL")){
        horizontal = true;
    }
    value = 0x1E | ((horizontal)? 1 : 0);
}

QString cmd_mirroring::getName(){
    return QString("MIRRORING");
}
