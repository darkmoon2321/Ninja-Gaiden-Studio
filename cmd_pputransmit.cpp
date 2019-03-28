#include "cmd_pputransmit.h"

cmd_PPUtransmit::cmd_PPUtransmit()
{
    command = 0x07;
}

bool cmd_PPUtransmit::killSignal(){
    return false;
}

uint32_t cmd_PPUtransmit::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint32_t temp = data[scene_pointer] + (data[scene_pointer+1]<<8);
    scene_pointer+=2;
    //temp = (temp&0x3FFF) + 0x10010;
    to_transmit = getPPUString(data,temp);
    to_transmit->references |= (1<<scene_number);
    image_changed = true;
    ppu_address = data[scene_pointer] + (data[scene_pointer+1]<<8);
    current_strings[num_current_strings++] = to_transmit;
    scene_pointer+=2;
    return scene_pointer;
}

uint32_t cmd_PPUtransmit::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = (to_transmit->address)&0xff;
    data[location++] = (to_transmit->address)>>8;
    data[location++] = ppu_address&0xff;
    data[location++] = ppu_address>>8;
    return location;
}

std::string cmd_PPUtransmit::writeToTXT(){
    std::string result = "PPU_TRANSMIT " + convertByteToHexString(to_transmit->id) + "@" + convertByteToHexString(ppu_address>>8) + convertByteToHexString(ppu_address&0xff) + '\n';
    return result;
}

cmd_PPUtransmit::~cmd_PPUtransmit(){

}

void cmd_PPUtransmit::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset = line.find("PPU_TRANSMIT") + 12;
    int32_t temp_int = getIntFromTXT(line,line_offset);
    unsigned int i;
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->id == temp_int){
            to_transmit = ppu_strings[i];
            break;
        }
    }
    if(i>=num_ppu_strings) to_transmit = ppu_strings[0];
    ppu_address = getIntFromTXT(line,line_offset);
    image_changed = true;
    current_strings[num_current_strings++] = to_transmit;

}

void cmd_PPUtransmit::addRef(uint8_t scene_num){
    to_transmit->references |= scene_num;
}

QString cmd_PPUtransmit::getName(){
    return QString("PPU_TRANSMIT");
}
