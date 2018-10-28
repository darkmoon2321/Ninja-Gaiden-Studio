#include "cmd_scrollsetup.h"

cmd_scrollsetup::cmd_scrollsetup()
{
    command = 0x03;
}

bool cmd_scrollsetup::killSignal(){
    return false;
}

uint32_t cmd_scrollsetup::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint32_t temp = data[scene_pointer++];
    enable_text = (temp&0x80);
    bit6 = (temp&0x40);
    if(temp==0){
        params = NULL;
        return scene_pointer;
    }
    params = getScrollParams(data,temp&0x3F);
    params->references |= (1<<scene_number);
    return scene_pointer;
}

uint32_t cmd_scrollsetup::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    if(!params){
        data[location++] = (char)0;
        return location;
    }
    uint8_t temp = params->id;
    if(!temp) bit6 = true;
    if(bit6) temp|=0x40;
    if(enable_text) temp|=0x80;
    data[location++] = temp;
    return location;
}

std::string cmd_scrollsetup::writeToTXT(){
    std::string result = "";
    if((!bit6) && (!enable_text) && (params == NULL)){
        result = "CANCEL_SPLITS\n";
        return result;
    }
    result = "SCROLL_SETUP";
    if(enable_text) result += " -ENABLE_TEXT";
    result += '\n';
    result += "  " + convertByteToHexString(params->num_splits) + " SPLITS\n";
    result += "  " + convertByteToHexString(params->ppu_ctrl_mod) + " PPU_CTRL_BITS\n";
    result += "  " + convertByteToHexString(params->sprite_0_Y) + " Sprite0 Y\n";
    result += "  " + convertByteToHexString(params->X_coarse) + " X\n";
    result += "  " + convertByteToHexString(params->Y_coarse) + " Y\n";
    result += "  " + convertByteToHexString(params->Vx_fine) + " Vx Fine\n";
    result += "  " + convertByteToHexString(params->Vx_coarse) + " Vx Coarse\n";
    result += "  " + convertByteToHexString(params->Vy_fine) + " Vy Fine\n";
    result += "  " + convertByteToHexString(params->Vy_coarse) + " Vy Coarse\n";
    for(int i=0;i<params->num_splits;i++){
        result += "  SPLIT " + convertByteToHexString(i) + '\n';
        result += "    " + convertByteToHexString(params->splits[i].X_coarse) + " X coarse\n";
        result += "    " + convertByteToHexString(params->splits[i].split_height_fine) + " Height, fine\n";
        result += "    " + convertByteToHexString(params->splits[i].split_height_coarse) + " Height, coarse\n";
        result += "    " + convertByteToHexString(params->splits[i].Vx_fine) + " Vx fine\n";
        result += "    " + convertByteToHexString(params->splits[i].Vx_coarse) + " Vx coarse\n";
    }
    return result;
}

cmd_scrollsetup::~cmd_scrollsetup(){

}

void cmd_scrollsetup::parseText(std::string text,std::string line,uint32_t & offset){
    enable_text = false;
    bit6 = false;
    params = NULL;
    if(std::string::npos != line.find("CANCEL_SPLITS")) return;
    if(std::string::npos != line.find("ENABLE_TEXT")) enable_text = true;
    params = new scroll_params;

    line = getLine(text,offset);
    lineUpper(line);
    uint32_t line_offset = 0;
    unsigned int i;
    params->num_splits = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->ppu_ctrl_mod = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->sprite_0_Y = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->X_coarse = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->Y_coarse = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->Vx_fine = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->Vx_coarse = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->Vy_fine = getIntFromTXT(line,line_offset);
    line = getLine(text,offset);
    lineUpper(line);
    line_offset = 0;
    params->Vy_coarse = getIntFromTXT(line,line_offset);
    for(i=0;i<params->num_splits;i++){
        getLine(text,offset);   //skip  line containing the split number
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->splits[i].X_coarse = getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->splits[i].split_height_fine = getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->splits[i].split_height_coarse = getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->splits[i].Vx_fine = getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->splits[i].Vx_coarse = getIntFromTXT(line,line_offset);
    }
    for(i=0;i<num_params;i++){
        if(*params == *scroll_params_list[i]){
            delete params;
            params = scroll_params_list[i];
            break;
        }
    }
    if(i>=num_params){
        scroll_params_list[num_params++] = params;
    }
}

void cmd_scrollsetup::addRef(uint8_t scene_num){
    params->references |= (1<<scene_num);
}
