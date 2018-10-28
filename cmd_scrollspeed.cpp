#include "cmd_scrollspeed.h"

cmd_scrollspeed::cmd_scrollspeed()
{
    command = 0x0B;
}

bool cmd_scrollspeed::killSignal(){
    return false;
}

uint32_t cmd_scrollspeed::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint8_t temp = data[scene_pointer++] + 1;
    if(temp == 1){  //stop y
        stopY = true;
        stopX = false;
        stop_splits = 0;
        params = NULL;
    }
    else if(!temp){ //stop X
        stopX = true;
        stopY = false;
        stop_splits = 0;
        params = NULL;
    }
    else if(temp&0x80){
        stopX = false;
        stopY = false;
        stop_splits = 0;
        temp--;
        params = getScrollParams(data,temp&0x7F);
        params->references |= (1<<scene_number);
    }
    else{
        stopX = false;
        stopY = false;
        params = NULL;
        stop_splits = temp - 1;
    }
    return scene_pointer;
}

uint32_t cmd_scrollspeed::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    if(stopY){
        data[location++] = 0;
    }
    else if(stopX){
        data[location++] = 0xff;
    }
    else if(params == NULL){
        data[location++] = stop_splits;
    }
    else{
        data[location++] = ((params->id)|0x80);
    }
    return location;
}

std::string cmd_scrollspeed::writeToTXT(){
    std::string result = "SCROLL ";
    if(stopY){
        result += "STOP Y\n";
    }
    else if(stopX){
        result += "STOP X\n";
    }
    else if(params==NULL){
        result += "STOP SPLIT ";
        uint8_t temp = stop_splits;
        for(int i=0;i<5;i++){
            if(temp&1) result += convertToASCII(i);
            temp>>=1;
        }
        result += '\n';
    }
    else{
        result += '\n';
        result += "  " + convertByteToHexString(params->num_splits) + " SPLITS\n";
        result += "  " + convertByteToHexString(params->Vx_fine) + " Vx Fine\n";
        result += "  " + convertByteToHexString(params->Vx_coarse) + " Vx Coarse\n";
        result += "  " + convertByteToHexString(params->Vy_fine) + " Vy Fine\n";
        result += "  " + convertByteToHexString(params->Vy_coarse) + " Vy Coarse\n";
        for(int i=0;i<params->num_splits;i++){
            result += "  SPLIT " + convertByteToHexString(i) + '\n';
            result += "    " + convertByteToHexString(params->splits[i].Vx_fine) + " Vx fine\n";
            result += "    " + convertByteToHexString(params->splits[i].Vx_coarse) + " Vx coarse\n";
        }
    }
    return result;
}

cmd_scrollspeed::~cmd_scrollspeed(){

}

void cmd_scrollspeed::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset = 0;
    int32_t temp_int;
    std::string temp_string;
    unsigned int i;

    stopX = false;
    stopY = false;
    params = NULL;
    stop_splits = 0;
    if(std::string::npos != line.find("STOP")){
        line_offset = line.find("STOP") + 4;
        temp_string = getWordFromTXT(line,line_offset);
        if(temp_string.length()){
            if(temp_string[0] == 'X'){
                stopX = true;
                return;
            }
            else if(temp_string[0] == 'Y'){
                stopY = true;
                return;
            }
            else if(std::string::npos != line.find("SPLIT")){
                line_offset = line.find("SPLIT") + 5;
                //temp_int = getIntFromTXT(line,line_offset);
                while(line_offset < line.length()){
                    temp_int = convertToHex(line[line_offset++]);
                    if(temp_int < 0) continue;
                    stop_splits |= (1<<(temp_int&0x7));
                }
                if(!stop_splits) stop_splits = 0x1f;
                return;
            }
        }
    }
    else{
/*        SCROLL
          01 SPLITS
          80 Vx Fine
          FF Vx Coarse
          00 Vy Fine
          00 Vy Coarse
          SPLIT 00
            00 Vx fine
            00 Vx coarse    */
        params = new scroll_params;
        params->id = num_params;
        params->references = 0;

        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        params->num_splits = getIntFromTXT(line,line_offset);
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
            getLine(text,offset);   //Skip past split number line
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
            if(params->speedEquals(*scroll_params_list[i])){
                delete params;
                params = scroll_params_list[i];
                break;
            }
        }
        if(i>=num_params){
            scroll_params_list[num_params++] = params;
        }
    }
}

void cmd_scrollspeed::addRef(uint8_t scene_num){
    if(stopX || stopY) return;
    if(!params) return;
    if(stop_splits) return;
    params->references |= (1<<scene_num);
}
