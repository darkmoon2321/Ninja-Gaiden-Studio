#include "cmd_paletteset.h"

cmd_paletteset::cmd_paletteset()
{
    command = 0x0C;
    changed_palettes = NULL;
}

bool cmd_paletteset::killSignal(){
    return false;
}

uint32_t cmd_paletteset::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    num_to_change = data[scene_pointer]&0x0F;
    start_sprites = data[scene_pointer++]&0x80;
    changed_palettes = new NESpalette * [num_to_change];
    unsigned int i,j;
    for(i=0;i<num_to_change;i++){
        changed_palettes[i] = getPalette(data,data[scene_pointer++]);
        changed_palettes[i]->references |= (1<<scene_number);
        if(start_sprites){
            if(i>=4) break;
            current_palettes[1].p[i] = *(changed_palettes[i]);
        }
        else{
            if(i>=8) break;
            if(i>=4){
                current_palettes[1].p[i - 4] = *(changed_palettes[i]);
            }
            else{
                current_palettes[0].p[i] = *(changed_palettes[i]);
            }
        }
    }
    for(i=0;i<num_current_bgs;i++){
        for(j=0;j<current_bgs[i]->pals.size();j++){
            if(current_bgs[i]->pals[j] == current_palettes[0]) break;
        }
        if(j>=current_bgs[i]->pals.size()){
            current_bgs[i]->pals.push_back(current_palettes[0]);
        }
    }
    for(i=0;i<num_current_sprites;i++){
        for(j=0;j<current_sprites[i]->pals.size();j++){
            if(current_sprites[i]->pals[j] == current_palettes[1]) break;
        }
        if(j>=current_sprites[i]->pals.size()){
            current_sprites[i]->pals.push_back(current_palettes[1]);
        }
    }
    return scene_pointer;
}

uint32_t cmd_paletteset::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    uint8_t temp = num_to_change;
    if(start_sprites) temp|=0x80;
    data[location++] = temp;
    for(int i=0;i<num_to_change;i++){
        data[location++] = changed_palettes[i]->id;
    }
    return location;
}

std::string cmd_paletteset::writeToTXT(){
    std::string result = "PALETTE_SET";
    int i = (start_sprites)? 4:0;
    for(int j=0;j<num_to_change;j++){
        result += "  ";
        result += convertToASCII((i++)&0xf);
        result += ":";
        for(int k=0;k<3;k++){
            result+= ' ' + convertByteToHexString(changed_palettes[j]->nes_colors[k + 1]);
        }
    }
    result += '\n';
    return result;
}

cmd_paletteset::~cmd_paletteset(){
    delete [] changed_palettes;
    changed_palettes = NULL;
}

void cmd_paletteset::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset = line.find("PALETTE_SET") + 11;
    uint32_t temp_offset = line_offset;
    int32_t temp_int = getIntFromTXT(line,temp_offset);
    start_sprites = false;
    num_to_change = 0;
    changed_palettes = NULL;
    if(temp_int == 0x04) start_sprites = true;
    unsigned int i,j;
    i=0;
    while(temp_int>=0){
        temp_int = getIntFromTXT(line,temp_offset);
        i++;
    }
    num_to_change = i>>2;
    if(start_sprites){
        num_to_change = (num_to_change>4)? 4 : num_to_change;
    }
    else{
        num_to_change = (num_to_change>8)? 8 : num_to_change;
    }
    changed_palettes = new NESpalette *[num_to_change];
    for(i=0;i<num_to_change;i++){
        getIntFromTXT(line,line_offset);    //this will skip past the unnecessary palette #.
        changed_palettes[i] = new NESpalette;
        changed_palettes[i]->id = num_palettes;
        changed_palettes[i]->references = 0;
        changed_palettes[i]->nes_colors[1] = getIntFromTXT(line,line_offset)&0x3f;
        changed_palettes[i]->nes_colors[2] = getIntFromTXT(line,line_offset)&0x3f;
        changed_palettes[i]->nes_colors[3] = getIntFromTXT(line,line_offset)&0x3f;
        for(j=0;j<num_palettes;j++){
            if(*changed_palettes[i] == *palettes[j]){
                delete changed_palettes[i];
                changed_palettes[i] = palettes[j];
                break;
            }
        }
        if(j>=num_palettes){
            palettes[num_palettes++] = changed_palettes[i];
        }

        if(start_sprites){
            current_palettes[1].p[i] = *(changed_palettes[i]);
        }
        else{
            if(i>=4){
                current_palettes[1].p[i - 4] = *(changed_palettes[i]);
            }
            else{
                current_palettes[0].p[i] = *(changed_palettes[i]);
            }
        }
    }
    for(i=0;i<num_current_bgs;i++){
        for(j=0;j<current_bgs[i]->pals.size();j++){
            if(current_bgs[i]->pals[j] == current_palettes[0]) break;
        }
        if(j>=current_bgs[i]->pals.size()){
            current_bgs[i]->pals.push_back(current_palettes[0]);
        }
    }
    for(i=0;i<num_current_sprites;i++){
        for(j=0;j<current_sprites[i]->pals.size();j++){
            if(current_sprites[i]->pals[j] == current_palettes[1]) break;
        }
        if(j>=current_sprites[i]->pals.size()){
            current_sprites[i]->pals.push_back(current_palettes[1]);
        }
    }
}

void cmd_paletteset::addRef(uint8_t scene_num){
    for(unsigned int i=0;i<num_to_change;i++){
        changed_palettes[i]->references |= (1<<scene_num);
    }
}

QString cmd_paletteset::getName(){
    return QString("PALETTE_SET");
}
