#include "cmd_earthquake.h"

cmd_earthquake::cmd_earthquake()
{
    command = 0x0D;
}

bool cmd_earthquake::killSignal(){
    return false;
}

uint32_t cmd_earthquake::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    quake = getEarthquake(data,data[scene_pointer++]);
    quake->references |= (1<<scene_number);
    return scene_pointer;
}

uint32_t cmd_earthquake::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = quake->id;
    return location;
}

std::string cmd_earthquake::writeToTXT(){
    std::string result = "EARTHQUAKE TIMER " + convertByteToHexString(quake->timer) +'\n';
    for(int i=0;i<quake->num_steps;i++){
        result += "  STEP " + convertByteToHexString(i) + '\n';
        result += "   " + convertByteToHexString(quake->ppu_ctrl[i]) + "  PPUBITS\n";
        result += "   " + convertByteToHexString(quake->x[i]) + " X\n";
        result += "   " + convertByteToHexString(quake->y[i]) + " Y\n";
    }
    return result;
}

cmd_earthquake::~cmd_earthquake(){

}

void cmd_earthquake::parseText(std::string text,std::string line,uint32_t & offset){
    int32_t temp_int;
    if(std::string::npos == line.find("TIMER")){
        quake = earthquakes[0];
        return;
    }
    uint32_t line_offset = line.find("TIMER") + 5;
    quake = new earthquake;
    temp_int = getIntFromTXT(line,line_offset);
    temp_int&=0xf;
    temp_int |= (temp_int<<4);
    quake->timer = temp_int;
    quake->num_steps = 0;
    quake->references = 0;
    quake->id = num_earthquakes;
    uint32_t old_offset = offset;
    line = getLine(text,offset);
    lineUpper(line);
    while(std::string::npos != line.find("STEP")){  //Could cause a problem if the following command has "STEP" in its name
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        temp_int = getIntFromTXT(line,line_offset);
        quake->ppu_ctrl += (uint8_t) temp_int;
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        temp_int = getIntFromTXT(line,line_offset);
        quake->x += (uint8_t) temp_int;
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        temp_int = getIntFromTXT(line,line_offset);
        quake->y += (uint8_t) temp_int;
        old_offset = offset;
        line = getLine(text,offset);
        lineUpper(line);
        quake->num_steps++;
    }
    offset = old_offset;
    unsigned int i;
    for(i=0;i<num_earthquakes;i++){
        if(*quake == *earthquakes[i]){
            delete quake;
            quake = earthquakes[i];
        }
    }
    if(i>=num_earthquakes){
        earthquakes[num_earthquakes++] = quake;
    }
}

void cmd_earthquake::addRef(uint8_t scene_num){
    quake->references |= (1<<scene_num);
}

QString cmd_earthquake::getName(){
    return QString("EARTHQUAKE");
}
