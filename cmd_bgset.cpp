#include "cmd_bgset.h"

cmd_BGSET::cmd_BGSET()
{
    arrangements = NULL;
    bases = NULL;
    command = 0x40;
}

bool cmd_BGSET::killSignal(){
    return false;
}

uint32_t cmd_BGSET::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    num_arrangements = command&0xf;
    num_current_strings = 0;
    unsigned int i;
    if(!num_arrangements){
        num_current_bgs = 0;
        return scene_pointer;
    }
    //image_changed = true;
    bases = new ppu_base_type*[num_arrangements];
    arrangements = new bg_arrangement*[num_arrangements];
    nes_ubg = data[scene_pointer++];
    pals[0] = getPalette(data,data[scene_pointer++]);
    pals[1] = getPalette(data,data[scene_pointer++]);
    pals[2] = getPalette(data,data[scene_pointer++]);
    pals[0]->references |= (1<<scene_number);
    pals[1]->references |= (1<<scene_number);
    pals[2]->references |= (1<<scene_number);
    current_palettes[0].nes_ubg = nes_ubg;
    current_palettes[1].nes_ubg = nes_ubg;
    current_palettes[0].p[1] = *pals[0];
    current_palettes[0].p[2] = *pals[1];
    current_palettes[0].p[3] = *pals[2];
    num_current_bgs = num_arrangements;
    for(i=0;i<num_arrangements;i++){
        bases[i] = getPPUBase(data,data[scene_pointer++]);
        bases[i]->references |= (1<<scene_number);
        arrangements[i] = getBGArrangement(data,data[scene_pointer++]);
        arrangements[i]->references |= (1<<scene_number);
        current_bgs[i] = arrangements[i];
        bg_CHR_page = arrangements[i]->gfx_page & 0x3F;
        unsigned int k;
        for(k=0;k<current_bgs[i]->pals.size();k++){
            if(current_bgs[i]->pals[k] == current_palettes[0]) break;
        }
        if(k>=current_bgs[i]->pals.size()){
            current_bgs[i]->pals.push_back(current_palettes[0]);
        }
    }
    return scene_pointer;
}

uint32_t cmd_BGSET::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    if(!num_arrangements){
        return location;
    }
    data[location++] = nes_ubg;
    data[location++] = pals[0]->id;
    data[location++] = pals[1]->id;
    data[location++] = pals[2]->id;
    for(unsigned int i=0;i<num_arrangements;i++){
        data[location++] = bases[i]->id;
        data[location++] = arrangements[i]->id;
    }
    return location;
}

std::string cmd_BGSET::writeToTXT(){
    std::string result;
    if(!num_arrangements){
        result = "BGCLEAR\n";
        return result;
    }
    result = "BGSET(" + convertByteToHexString(num_arrangements) + " ARRANGEMENTS)\n  ";
    result += "BGCOLOR: " + convertByteToHexString(nes_ubg) + '\n';
    unsigned int i,j;
    for(i=0;i<3;i++){
        result += "  PALETTE";
        result += convertToASCII((i&0xf)+1);
        result += ":";
        for(j=0;j<3;j++){
            result += ' ' + convertByteToHexString(pals[i]->nes_colors[j+1]);
        }
        result += '\n';
    }
    for(i=0;i<num_arrangements;i++){
        result += "  PPUADDRESS:" + convertByteToHexString((bases[i]->a)>>8) + convertByteToHexString((bases[i]->a)&0xff) + '\n';
        result += "  BG#:" + convertByteToHexString(arrangements[i]->id) + '\n';
    }
    return result;
}

cmd_BGSET::~cmd_BGSET(){
    delete [] arrangements;
    arrangements = NULL;
    delete [] bases;
    bases = NULL;
}

void cmd_BGSET::parseText(std::string text,std::string line,uint32_t & offset){
    num_current_strings = 0;

    int32_t temp_int;
    if(std::string::npos != line.find("BGCLEAR")){
        num_arrangements = 0;
        num_current_bgs = 0;
        arrangements = NULL;
        bases = NULL;
        return;
    }
    uint32_t line_offset = line.find("BGSET") + 5;
    temp_int = getIntFromTXT(line,line_offset);
    if(temp_int<0){
        num_arrangements = 0;
        arrangements = NULL;
        bases = NULL;
        return;
    }
    num_arrangements = temp_int;
    command|=num_arrangements;
    bases = new ppu_base_type*[num_arrangements];
    arrangements = new bg_arrangement*[num_arrangements];
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    if(std::string::npos == line.find("BGCOLOR")){
        nes_ubg = 0x0F;
    }
    line_offset = line.find("BGCOLOR") + 7;
    temp_int = getIntFromTXT(line,line_offset);
    nes_ubg = (temp_int>=0) ? temp_int&0x3F : 0x0F;

    unsigned int i,j;
    for(i=0;i<3;i++){
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos == line.find("PALETTE")){
            pals[i] = palettes[0];
            continue;
        }
        line_offset = line.find("PALETTE") + 8; //Also skip the palette # character
        pals[i] = new NESpalette;
        pals[i]->references = 0;
        temp_int = getIntFromTXT(line,line_offset);
        pals[i]->nes_colors[1] = (temp_int>=0) ? temp_int&0x3F : 0x0F;
        temp_int = getIntFromTXT(line,line_offset);
        pals[i]->nes_colors[2] = (temp_int>=0) ? temp_int&0x3F : 0x0F;
        temp_int = getIntFromTXT(line,line_offset);
        pals[i]->nes_colors[3] = (temp_int>=0) ? temp_int&0x3F : 0x0F;
        for(j=0;j<num_palettes;j++){
            if(*pals[i] == *palettes[j]){
                delete pals[i];
                pals[i] = palettes[j];
                break;
            }
        }
        if(j>=num_palettes){
            pals[i]->id = num_palettes;
            palettes[num_palettes++] = pals[i];
        }
    }
    current_palettes[0].nes_ubg = nes_ubg;
    current_palettes[1].nes_ubg = nes_ubg;
    current_palettes[0].p[1] = *pals[0];
    current_palettes[0].p[2] = *pals[1];
    current_palettes[0].p[3] = *pals[2];
    num_current_bgs = num_arrangements;

    for(i=0;i<num_arrangements;i++){
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos == line.find("PPUADDRESS")){
            bases[i] = ppu_bases[0];
        }
        else{
            line_offset = line.find("PPUADDRESS") + 9;
            temp_int = getIntFromTXT(line,line_offset);
            bases[i] = new ppu_base_type;
            bases[i]->references = 0;
            bases[i]->a = (temp_int>=0) ? temp_int : 0x2100;
            for(j=0;j<num_ppu_bases;j++){
                if(bases[i]->a == ppu_bases[j]->a){
                    delete bases[i];
                    bases[i] = ppu_bases[j];
                    break;
                }
            }
            if(j>=num_ppu_bases){
                bases[i]->id = num_ppu_bases;
                ppu_bases[num_ppu_bases++] = bases[i];
            }
        }
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos == line.find("BG")){
            arrangements[i] = bgs[0];
        }
        else{
            line_offset = line.find("BG") + 2;
            temp_int = getIntFromTXT(line,line_offset);
            for(j=0;j<num_bgs;j++){
                if(bgs[j]->id == temp_int){
                    arrangements[i] = bgs[j];
                    break;
                }
            }
            if(j>=num_bgs){
                arrangements[i] = bgs[0];
            }
        }
        current_bgs[i] = arrangements[i];
        bg_CHR_page = arrangements[i]->gfx_page & 0x3F;
        unsigned int k;
        for(k=0;k<current_bgs[i]->pals.size();k++){
            if(current_bgs[i]->pals[k] == current_palettes[0]) break;
        }
        if(k>=current_bgs[i]->pals.size()){
            current_bgs[i]->pals.push_back(current_palettes[0]);
        }
    }
}

void cmd_BGSET::addRef(uint8_t scene_num){
    pals[0]->references |= (1<<scene_num);
    pals[1]->references |= (1<<scene_num);
    pals[2]->references |= (1<<scene_num);
    for(unsigned int i=0;i<num_arrangements;i++){
        arrangements[i]->references |= (1<<scene_num);
        bases[i]->references |= (1<<scene_num);
    }
}
