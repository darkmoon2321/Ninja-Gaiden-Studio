#include "cmd_wait.h"
#include <QMessageBox>

cmd_wait::cmd_wait()
{
    command = 0xff;
}

bool cmd_wait::killSignal(){
    return (delay == 0xffff)? true : false;
}

uint32_t cmd_wait::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    delay = data[scene_pointer++];
    delay += ((uint16_t)(data[scene_pointer++]))<<8;
    for(std::vector<sprite *>::iterator sprite_it = current_sprites.begin();sprite_it<current_sprites.end();sprite_it++){
        (*sprite_it)->bg_gfx_page = bg_CHR_page;
    }
    if(image_changed){
        image_changed = false;
        unsigned int i,j;
        for(i=0;i<num_current_strings;i++){
            current_strings[i]->gfx_page = bg_CHR_page;
            if(current_strings[i]->tiles.size()) continue;
            uint32_t pointer = (current_strings[i]->address&0x3fff) + 0x10010;
            int size_byte;
            j=0;
            current_strings[i]->gfx_page = bg_CHR_page;
            while(true){
                current_strings[i]->s += data[pointer + j];
                size_byte = data[pointer + j]&0x7f;
                if(!size_byte) break;
                current_strings[i]->s += data[pointer + j + 1];
                current_strings[i]->s += data[pointer + j + 2];
                j+=3;
                for(int k=0;k<size_byte;k++){
                    current_strings[i]->s += data[pointer + j];
                    current_strings[i]->tiles.push_back(&(CHR_pages[bg_CHR_page].t[data[pointer + j]]));
                    CHR_pages[bg_CHR_page].bg_used[data[pointer + j]] = true;
                    j++;
                }
            }

        }
        //num_current_strings = 0;
    }
    return scene_pointer;
}

uint32_t cmd_wait::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = delay&0xff;
    data[location++] = delay>>8;
    return location;
}

std::string cmd_wait::writeToTXT(){
    std::string result;
    if(delay==0xffff){
        result = "END_SCENE";
    }
    else{
        result = "WAIT\n";
        result += "TIMER = " + convertByteToHexString(delay>>8) + convertByteToHexString(delay) + '\n';
    }
    return result;
}

cmd_wait::~cmd_wait(){

}

void cmd_wait::parseText(std::string text,std::string line,uint32_t & offset){
    if(line.find("END_SCENE") != std::string::npos){
        delay = 0xffff;
        num_current_strings = 0;
        num_current_bgs = 0;
        num_current_sprites = 0;
        current_sprites.clear();
        return;
    }
    if(image_changed){
        image_changed = false;
        unsigned int i;
        for(i=0;i<num_current_strings;i++){
            current_strings[i]->gfx_page = bg_CHR_page;
        }
        for(std::vector<sprite *>::iterator sprite_it = current_sprites.begin();sprite_it<current_sprites.end();sprite_it++){
            (*sprite_it)->bg_gfx_page = bg_CHR_page;
        }
    }

    line = getLine(text,offset);
    lineUpper(line);
    uint32_t line_offset = 0;
    if(std::string::npos != line.find("TIMER")){
        line_offset = line.find("TIMER") + 5;
        delay = getIntFromTXT(line,line_offset);
        return;
    }
    else{
        delay = 0xffff;
        num_current_strings = 0;
        num_current_bgs = 0;
        num_current_sprites = 0;
        current_sprites.clear();
    }
}
