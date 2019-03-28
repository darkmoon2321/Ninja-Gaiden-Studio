#include "cmd_movesprite.h"

cmd_movesprite::cmd_movesprite()
{
    command = 0x09;
}

bool cmd_movesprite::killSignal(){
    return false;
}

uint32_t cmd_movesprite::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    uint8_t temp = data[scene_pointer++];
    flipv = temp&0x80;
    fliph = temp&0x40;
    priority = temp&0x20;
    sprite_num = temp&0xf;
    temp = data[scene_pointer++];
    movement_type = temp>>6;
    switch(movement_type){
    case 0x00: //fixed
        movement_pointer = NULL;
        break;
    case 0x01: //linear
        movement_pointer = getSpriteLinear(data,temp&0x3F); //************
        ((sprite_linear*)movement_pointer)->references|=(1<<scene_number);
        break;
    case 0x02: //shake
        movement_pointer = getSpriteShake(data,temp&0x3F);
        ((sprite_shake*)movement_pointer)->references|=(1<<scene_number);
        break;
    case 0x03: //accelerating
        movement_pointer = getSpriteAccel(data,temp&0x3F);
        ((sprite_accel*)movement_pointer)->references|=(1<<scene_number);
        break;
    default:
        //ERROR HANDLER
        break;
    }
    return scene_pointer;
}

uint32_t cmd_movesprite::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    uint8_t temp = sprite_num;
    if(flipv) temp|=0x80;
    if(fliph) temp|=0x40;
    if(priority) temp|=0x20;
    data[location++] = temp;
    temp = movement_type<<6;
    switch(movement_type&3){
    case 0: //fixed
        data[location++] = 0;
        break;
    case 1: //linear
        temp|= ((sprite_linear*)movement_pointer)->id;
        data[location++] = temp;
        break;
    case 2: //shake
        temp|= ((sprite_shake*)movement_pointer)->id;
        data[location++] = temp;
        break;
    case 3: //accelerating
        temp|= ((sprite_accel*)movement_pointer)->id;
        data[location++] = temp;
        break;
    }

    return location;
}

std::string cmd_movesprite::writeToTXT(){
    std::string result = "MOVE_SPRITE ";
    result += convertToASCII(sprite_num&0xf);
    if(flipv) result += " FLIPV";
    if(fliph) result += " FLIPH";
    if(priority) result += " PRIORITY";
    result += '\n';
    switch(movement_type&3){
    case 0: //fixed
        result += "  FIXED\n";
        break;
    case 1: //linear
        result += "  LINEAR: Vx=" + convertByteToHexString(((sprite_linear*)movement_pointer)->Vx_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointer)->Vx_fine);
        result += " Vy=" + convertByteToHexString(((sprite_linear*)movement_pointer)->Vy_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointer)->Vy_fine);
        result += '\n';
        break;
    case 2: //shake
        result += "  SHAKE: " + convertByteToHexString(((sprite_shake*)movement_pointer)->num_steps) + " STEPS, TIMER:" + convertByteToHexString(((sprite_shake*)movement_pointer)->timer) + '\n';
        for(int i=0;i<((sprite_shake*)movement_pointer)->num_steps;i++){
            result += "  STEP " + convertByteToHexString(i) + " X:" + convertByteToHexString(((sprite_shake*)movement_pointer)->x[i]);
            result += " Y:" + convertByteToHexString(((sprite_shake*)movement_pointer)->y[i]) + '\n';
        }
        break;
    case 3: //accelerating
        result += "  ACCEL: Vx=" + convertByteToHexString(((sprite_linear*)movement_pointer)->Vx_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointer)->Vx_fine);
        result += " Vy=" + convertByteToHexString(((sprite_linear*)movement_pointer)->Vy_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointer)->Vy_fine);
        result += " Ax=" +convertByteToHexString(((sprite_accel*)movement_pointer)->Ax) + " Ay=" + convertByteToHexString(((sprite_accel*)movement_pointer)->Ay);
        result += '\n';
        break;
    }
    return result;
}

cmd_movesprite::~cmd_movesprite(){

}

void cmd_movesprite::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset;
    int32_t temp_int;
    unsigned int i;
    priority = false;
    fliph = false;
    flipv = false;
    line_offset = line.find("MOVE_SPRITE") + 11;
    temp_int = getIntFromTXT(line,line_offset);
    if(temp_int<0 || temp_int>3) temp_int = 0;
    sprite_num = temp_int;
    if(std::string::npos != line.find("FLIPV")) flipv = true;
    if(std::string::npos != line.find("FLIPH")) fliph = true;
    if(std::string::npos != line.find("PRIORITY")) priority = true;
    line = getLine(text,offset);
    lineUpper(line);
    if(std::string::npos != line.find("ACCEL")){
        movement_type = 3;
        movement_pointer = new sprite_accel;
        line_offset = line.find("ACCEL") + 5;
        ((sprite_accel *)movement_pointer)->id = num_sprite_accels;
        ((sprite_accel *)movement_pointer)->references = 0;
        ((sprite_accel *)movement_pointer)->Vx_coarse = getIntFromTXT(line,line_offset);
        ((sprite_accel *)movement_pointer)->Vx_fine = getIntFromTXT(line,line_offset);
        ((sprite_accel *)movement_pointer)->Vy_coarse = getIntFromTXT(line,line_offset);
        ((sprite_accel *)movement_pointer)->Vy_fine = getIntFromTXT(line,line_offset);
        if(line.find("AX") != std::string::npos){
            line_offset = line.find("AX") + 2;
            ((sprite_accel *)movement_pointer)->Ax = getIntFromTXT(line,line_offset);
        }
        else{
            ((sprite_accel *)movement_pointer)->Ax = 0;
        }
        if(line.find("AY") != std::string::npos){
            line_offset = line.find("AY") + 2;
            ((sprite_accel *)movement_pointer)->Ay = getIntFromTXT(line,line_offset);
        }
        else{
            ((sprite_accel *)movement_pointer)->Ay = 0;
        }
        for(i=0;i<num_sprite_accels;i++){
            if(*((sprite_accel *)movement_pointer) == *sprite_accels[i]){
                delete ((sprite_accel *)movement_pointer);
                movement_pointer = sprite_accels[i];
                break;
            }
        }
        if(i>=num_sprite_accels){
            sprite_accels[num_sprite_accels++] = (sprite_accel *)movement_pointer;
        }
    }
    else if(std::string::npos != line.find("LINEAR")){
        movement_type = 1;
        movement_pointer = new sprite_linear;
        line_offset = line.find("LINEAR") + 6;
        ((sprite_linear *)movement_pointer)->id = num_sprite_linears;
        ((sprite_linear *)movement_pointer)->references = 0;
        ((sprite_linear *)movement_pointer)->Vx_coarse = getIntFromTXT(line,line_offset);
        ((sprite_linear *)movement_pointer)->Vx_fine = getIntFromTXT(line,line_offset);
        ((sprite_linear *)movement_pointer)->Vy_coarse = getIntFromTXT(line,line_offset);
        ((sprite_linear *)movement_pointer)->Vy_fine = getIntFromTXT(line,line_offset);
        for(i=0;i<num_sprite_linears;i++){
            if(*((sprite_linear *)movement_pointer) == *sprite_linears[i]){
                delete ((sprite_linear *)movement_pointer);
                movement_pointer = sprite_linears[i];
                break;
            }
        }
        if(i>=num_sprite_linears){
            sprite_linears[num_sprite_linears++] = (sprite_linear *)movement_pointer;
        }
    }
    else if(std::string::npos != line.find("SHAKE")){
        movement_type = 2;
        movement_pointer = new sprite_shake;
        line_offset = line.find("SHAKE") + 5;
        ((sprite_shake *)movement_pointer)->id = num_sprite_shakes;
        ((sprite_shake *)movement_pointer)->references = 0;
        ((sprite_shake *)movement_pointer)->num_steps = getIntFromTXT(line,line_offset);
        ((sprite_shake *)movement_pointer)->x = "";
        ((sprite_shake *)movement_pointer)->y = "";
        if(std::string::npos != line.find("TIMER")){
            line_offset = line.find("TIMER") + 5;
            ((sprite_shake *)movement_pointer)->timer = getIntFromTXT(line,line_offset);
        }
        else{
            ((sprite_shake *)movement_pointer)->timer = 0;
        }
        for(i=0;i<((sprite_shake *)movement_pointer)->num_steps;i++){
            line = getLine(text,offset);
            lineUpper(line);
            if(std::string::npos != line.find("STEP")){
                line_offset = line.find("STEP") + 4;
                getIntFromTXT(line,line_offset);
                ((sprite_shake *)movement_pointer)->x += (uint8_t)getIntFromTXT(line,line_offset);
                ((sprite_shake *)movement_pointer)->y += (uint8_t)getIntFromTXT(line,line_offset);
            }
            else{
                ((sprite_shake *)movement_pointer)->x += ((char)0);
                ((sprite_shake *)movement_pointer)->y += ((char)0);
            }
        }
        for(i=0;i<num_sprite_shakes;i++){
            if(*((sprite_shake *)movement_pointer) == *sprite_shakes[i]){
                delete ((sprite_shake *)movement_pointer);
                movement_pointer = sprite_shakes[i];
                break;
            }
        }
        if(i>=num_sprite_shakes){
            sprite_shakes[num_sprite_shakes++] = (sprite_shake *)movement_pointer;
        }
    }
    else{   //Default to Fixed movement
        movement_type = 0;
        movement_pointer = NULL;
    }
}

void cmd_movesprite::addRef(uint8_t scene_num){
    switch(movement_type&3){
    case 0:
        break;
    case 1:
        ((sprite_linear *)movement_pointer)->references |= (1<<scene_num);
        break;
    case 2:
        ((sprite_shake *)movement_pointer)->references |= (1<<scene_num);
        break;
    case 3:
        ((sprite_accel *)movement_pointer)->references |= (1<<scene_num);
        break;
    }
}

QString cmd_movesprite::getName(){
    return QString("MOVE_SPRITE");
}
