#include "cmd_spriteset.h"
#include <QMessageBox>

cmd_spriteset::cmd_spriteset()
{
    movement_pointers = NULL;
    arrangements = NULL;
    animation = NULL;
    command = 0x80;
}

bool cmd_spriteset::killSignal(){
    return false;
}

uint32_t cmd_spriteset::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    num_arrangements = command&0xf;
    current_sprites.clear();
    if(!num_arrangements){
        return scene_pointer;
    }
    x_values = "";
    y_values = "";
    attribs = "";
    move_types = "";
    movement_pointers = new void * [num_arrangements];
    arrangements = new void * [num_arrangements];
    animation = new bool[num_arrangements];
    pals[0] = getPalette(data,data[scene_pointer++]);
    pals[1] = getPalette(data,data[scene_pointer++]);
    pals[2] = getPalette(data,data[scene_pointer++]);
    pals[3] = getPalette(data,data[scene_pointer++]);
    pals[0]->references |= (1<<scene_number);
    pals[1]->references |= (1<<scene_number);
    pals[2]->references |= (1<<scene_number);
    pals[3]->references |= (1<<scene_number);
    current_palettes[1].p[0] = *pals[0];
    current_palettes[1].p[1] = *pals[1];
    current_palettes[1].p[2] = *pals[2];
    current_palettes[1].p[3] = *pals[3];
    unsigned int i,j,k;
    image_changed = true;
    for(i=0;i<num_arrangements;i++){
        x_values += data[scene_pointer++];
        y_values += data[scene_pointer++];
        attribs += data[scene_pointer++];
        uint8_t temp = data[scene_pointer++];
        if(temp&0x80){
            animation[i] = true;
            arrangements[i] = getSpriteAnimation(data,temp&0x7F);
            ((sprite_animation *)arrangements[i])->references |= (1<<scene_number);
            for(j=0;j<((sprite_animation*)arrangements[i])->num_steps;j++){
                ((sprite_animation*)arrangements[i])->sequence[j]->references |= (1<<scene_number);
                for(k=0;k<((sprite_animation*)arrangements[i])->sequence[j]->pals.size();k++){
                    if(((sprite_animation*)arrangements[i])->sequence[j]->pals[k] == current_palettes[1]) break;
                }
                if(k>=((sprite_animation*)arrangements[i])->sequence[j]->pals.size()){
                    ((sprite_animation*)arrangements[i])->sequence[j]->pals.push_back(current_palettes[1]);
                }
                current_sprites.push_back(((sprite_animation*)arrangements[i])->sequence[j]);
                sprite_CHR_page = ((sprite_animation*)arrangements[i])->sequence[j]->gfx_page & 0x3F;
            }
        }
        else{
            animation[i] = false;
            arrangements[i] = getSprite(data,temp);
            ((sprite *)arrangements[i])->references |= (1<<scene_number);
            for(j=0;j<((sprite*)arrangements[i])->pals.size();j++){
                if(((sprite*)arrangements[i])->pals[j] == current_palettes[1]) break;
            }
            if(j>=((sprite*)arrangements[i])->pals.size()){
                ((sprite*)arrangements[i])->pals.push_back(current_palettes[1]);
            }
            current_sprites.push_back((sprite*)arrangements[i]);
            sprite_CHR_page = ((sprite*)arrangements[i])->gfx_page & 0x3F;
        }
        temp = data[scene_pointer++];
        move_types += temp>>6;
        switch(move_types[i]){
        case 0x00: //fixed
            movement_pointers[i] = NULL;
            break;
        case 0x01: //linear
            movement_pointers[i] = getSpriteLinear(data,temp&0x3F); //************
            ((sprite_linear*)movement_pointers[i])->references|=(1<<scene_number);
            break;
        case 0x02: //shake
            movement_pointers[i] = getSpriteShake(data,temp&0x3F);
            ((sprite_shake*)movement_pointers[i])->references|=(1<<scene_number);
            break;
        case 0x03: //accelerating
            movement_pointers[i] = getSpriteAccel(data,temp&0x3F);
            ((sprite_accel*)movement_pointers[i])->references|=(1<<scene_number);
            break;
        }
    }
    return scene_pointer;
}

uint32_t cmd_spriteset::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    if(!num_arrangements) return location;
    data[location++] = pals[0]->id;
    data[location++] = pals[1]->id;
    data[location++] = pals[2]->id;
    data[location++] = pals[3]->id;
    for(unsigned int i=0;i<num_arrangements;i++){
        data[location++] = x_values[i];
        data[location++] = y_values[i];
        data[location++] = attribs[i];
        if(animation[i]){
            data[location++] = ((sprite_animation*)arrangements[i])->id | 0x80;
        }
        else{
            data[location++] = ((sprite*)arrangements[i])->id;
        }
        switch(move_types[i]&3){
        case 0:
            data[location++] = 0;
            break;
        case 1:
            data[location++] = ((sprite_linear*)movement_pointers[i])->id | (move_types[i]<<6);
            break;
        case 2:
            data[location++] = ((sprite_shake*)movement_pointers[i])->id | (move_types[i]<<6);
            break;
        case 3:
            data[location++] = ((sprite_accel*)movement_pointers[i])->id | (move_types[i]<<6);
            break;
        }
    }
    return location;
}

std::string cmd_spriteset::writeToTXT(){
    std::string result = "";
    if(!num_arrangements){
        result = "SPRITECLEAR\n";
        return result;
    }
    result += "SPRITESET(" + convertByteToHexString(num_arrangements) + " SPRITES)\n";
    unsigned int i,j;
    for(i=0;i<4;i++){
        result += "  PALETTE";
        result += convertToASCII((i&0xf));
        result += ":";
        for(j=0;j<3;j++){
            result += ' ' + convertByteToHexString(pals[i]->nes_colors[j+1]);
        }
        result += '\n';
    }
    for(i=0;i<num_arrangements;i++){
        result += "  X: " + convertByteToHexString(x_values[i]) + "\n";
        result += "  Y: " + convertByteToHexString(y_values[i]) + "\n";
        result += "  ATTRIBUTES: " + convertByteToHexString(attribs[i]) + "\n";
        result += "  SPRITE#: ";
        if(animation[i]){
            result += "ANIMATION, TIMER: " + convertByteToHexString(((sprite_animation*)arrangements[i])->timer) + "\n   ";
            for(unsigned int j=0;j<((sprite_animation*)arrangements[i])->num_steps;j++){
                result += ' ' + convertByteToHexString(((sprite_animation*)arrangements[i])->sequence[j]->id);
            }
            result += '\n';
        }
        else{
            result += convertByteToHexString(((sprite*)arrangements[i])->id) + '\n';
        }
        switch(move_types[i]&3){
        case 0: //fixed
            result += "  FIXED\n";
            break;
        case 1: //linear
            result += "  LINEAR: Vx=" + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vx_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vx_fine);
            result += " Vy=" + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vy_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vy_fine);
            result += '\n';
            break;
        case 2: //shake
            result += "  SHAKE: " + convertByteToHexString(((sprite_shake*)movement_pointers[i])->num_steps) + " STEPS, TIMER:" + convertByteToHexString(((sprite_shake*)movement_pointers[i])->timer) + '\n';
            for(unsigned int j=0;j<((sprite_shake*)movement_pointers[i])->num_steps;j++){
                result += "    STEP " + convertByteToHexString(j) + " X:" + convertByteToHexString(((sprite_shake*)movement_pointers[i])->x[j]);
                result += " Y:" + convertByteToHexString(((sprite_shake*)movement_pointers[i])->y[j]) + '\n';
            }
            break;
        case 3: //accelerating
            result += "  ACCEL: Vx=" + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vx_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vx_fine);
            result += " Vy=" + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vy_coarse) + '.' + convertByteToHexString(((sprite_linear*)movement_pointers[i])->Vy_fine);
            result += " Ax=" +convertByteToHexString(((sprite_accel*)movement_pointers[i])->Ax) + " Ay=" + convertByteToHexString(((sprite_accel*)movement_pointers[i])->Ay);
            result += '\n';
            break;
        }
    }
    return result;
}

cmd_spriteset::~cmd_spriteset(){
    delete [] movement_pointers;
    movement_pointers = NULL;
    delete [] arrangements;
    arrangements = NULL;
    delete [] animation;
    animation = NULL;
}

void cmd_spriteset::parseText(std::string text,std::string line,uint32_t & offset){
    current_sprites.clear();
    if(std::string::npos != line.find("SPRITECLEAR")){
        num_arrangements = 0;
        return;
    }
    int32_t temp_int;
    uint32_t line_offset = line.find("SPRITESET") + 9;
    num_arrangements = getIntFromTXT(line,line_offset);
    command |= num_arrangements;
    unsigned int i,j,k;
    for(i=0;i<4;i++){
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos == line.find("PALETTE")){
            pals[i] = palettes[0];
            continue;
        }
        line_offset = line.find("PALETTE") + 8; //Also skip the palette # character
        temp_int = getIntFromTXT(line,line_offset);
        pals[i] = new NESpalette;
        pals[i]->references = 0;
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
    current_palettes[1].p[0] = *pals[0];
    current_palettes[1].p[1] = *pals[1];
    current_palettes[1].p[2] = *pals[2];
    current_palettes[1].p[3] = *pals[3];
    x_values = "";
    y_values = "";
    attribs = "";
    move_types = "";
    movement_pointers = new void * [num_arrangements];
    arrangements = new void * [num_arrangements];
    animation = new bool[num_arrangements];
    for(i=0;i<num_arrangements;i++){
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        x_values += (uint8_t)getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        y_values += (uint8_t)getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        getWordFromTXT(line,line_offset);
        attribs += (uint8_t)getIntFromTXT(line,line_offset);
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos != line.find("ANIMATION")){
            animation[i] = true;
            arrangements[i] = new sprite_animation;
            ((sprite_animation *)arrangements[i])->id = num_sprite_animations;
            ((sprite_animation *)arrangements[i])->references = 0;
            ((sprite_animation *)arrangements[i])->num_steps = 0;
            ((sprite_animation *)arrangements[i])->timer = 0;
            ((sprite_animation *)arrangements[i])->text_engine = false;
            ((sprite_animation *)arrangements[i])->sequence = NULL;
            if(std::string::npos != line.find("TIMER")){
                line_offset = line.find("TIMER") + 5;
                temp_int = getIntFromTXT(line,line_offset);
                temp_int &= 0xF;
                ((sprite_animation *)arrangements[i])->timer = temp_int | (temp_int<<4);
            }
            line = getLine(text,offset);
            lineUpper(line);
            line_offset = 0;
            j=0;
            temp_int = getIntFromTXT(line,line_offset);
            while(temp_int >=0){
                temp_int = getIntFromTXT(line,line_offset);
                j++;
            }
            ((sprite_animation *)arrangements[i])->num_steps = j;
            ((sprite_animation *)arrangements[i])->sequence = new sprite * [j];
            line_offset = 0;
            for(j=0;j<((sprite_animation *)arrangements[i])->num_steps;j++){
                temp_int = getIntFromTXT(line,line_offset);
                for(k=0;k<num_sprites;k++){
                    if(temp_int == sprites[k]->id){
                        ((sprite_animation *)arrangements[i])->sequence[j] = sprites[k];
                        break;
                    }
                }
                if(k>=num_sprites) ((sprite_animation *)arrangements[i])->sequence[j] = sprites[0];
            }
            for(j=0;j<num_sprite_animations;j++){
                if(*((sprite_animation *)arrangements[i]) == *sprite_animations[j]){
                    delete ((sprite_animation *)arrangements[i]);
                    arrangements[i] = sprite_animations[j];
                    break;
                }
            }
            if(j>=num_sprite_animations) sprite_animations[num_sprite_animations++] = ((sprite_animation *)arrangements[i]);

            for(j=0;j<((sprite_animation*)arrangements[i])->num_steps;j++){
                for(k=0;k<((sprite_animation*)arrangements[i])->sequence[j]->pals.size();k++){
                    if(((sprite_animation*)arrangements[i])->sequence[j]->pals[k] == current_palettes[1]) break;
                }
                if(k>=((sprite_animation*)arrangements[i])->sequence[j]->pals.size()){
                    ((sprite_animation*)arrangements[i])->sequence[j]->pals.push_back(current_palettes[1]);
                }
                current_sprites.push_back(((sprite_animation*)arrangements[i])->sequence[j]);
                sprite_CHR_page = ((sprite_animation*)arrangements[i])->sequence[j]->gfx_page & 0x3F;
            }
        }
        else{
            animation[i] = false;
            getWordFromTXT(line,line_offset);
            temp_int = (uint8_t)getIntFromTXT(line,line_offset);
            if(temp_int<0){
                arrangements[i] = sprites[0];
            }
            else{
                for(j=0;j<num_sprites;j++){
                    if(sprites[j]->id == temp_int){
                        arrangements[i] = sprites[j];
                        break;
                    }
                }
                if(j>=num_sprites){
                    arrangements[i] = sprites[0];
                }
            }
            for(j=0;j<((sprite*)arrangements[i])->pals.size();j++){
                if(((sprite*)arrangements[i])->pals[j] == current_palettes[1]) break;
            }
            if(j>=((sprite*)arrangements[i])->pals.size()){
                ((sprite*)arrangements[i])->pals.push_back(current_palettes[1]);
            }
            current_sprites.push_back((sprite*)arrangements[i]);
            sprite_CHR_page = ((sprite*)arrangements[i])->gfx_page & 0x3F;
        }
        line = getLine(text,offset);
        lineUpper(line);
        line_offset = 0;
        if(std::string::npos != line.find("ACCEL")){
            move_types += 3;
            movement_pointers[i] = new sprite_accel;
            line_offset = line.find("ACCEL") + 5;
            ((sprite_accel *)movement_pointers[i])->id = num_sprite_accels;
            ((sprite_accel *)movement_pointers[i])->references = 0;
            ((sprite_accel *)movement_pointers[i])->Vx_coarse = getIntFromTXT(line,line_offset);
            ((sprite_accel *)movement_pointers[i])->Vx_fine = getIntFromTXT(line,line_offset);
            ((sprite_accel *)movement_pointers[i])->Vy_coarse = getIntFromTXT(line,line_offset);
            ((sprite_accel *)movement_pointers[i])->Vy_fine = getIntFromTXT(line,line_offset);
            if(line.find("AX") != std::string::npos){
                line_offset = line.find("AX") + 2;
                ((sprite_accel *)movement_pointers[i])->Ax = getIntFromTXT(line,line_offset);
            }
            else{
                ((sprite_accel *)movement_pointers[i])->Ax = 0;
            }
            if(line.find("AY") != std::string::npos){
                line_offset = line.find("AY") + 2;
                ((sprite_accel *)movement_pointers[i])->Ay = getIntFromTXT(line,line_offset);
            }
            else{
                ((sprite_accel *)movement_pointers[i])->Ay = 0;
            }
            for(j=0;j<num_sprite_accels;j++){
                if(*((sprite_accel *)movement_pointers[i]) == *sprite_accels[j]){
                    delete ((sprite_accel *)movement_pointers[i]);
                    movement_pointers[i] = sprite_accels[j];
                    break;
                }
            }
            if(j>=num_sprite_accels){
                sprite_accels[num_sprite_accels++] = (sprite_accel *)movement_pointers[i];
            }
        }
        else if(std::string::npos != line.find("LINEAR")){
            move_types += 1;
            movement_pointers[i] = new sprite_linear;
            line_offset = line.find("LINEAR") + 6;
            ((sprite_linear *)movement_pointers[i])->id = num_sprite_linears;
            ((sprite_linear *)movement_pointers[i])->references = 0;
            ((sprite_linear *)movement_pointers[i])->Vx_coarse = getIntFromTXT(line,line_offset);
            ((sprite_linear *)movement_pointers[i])->Vx_fine = getIntFromTXT(line,line_offset);
            ((sprite_linear *)movement_pointers[i])->Vy_coarse = getIntFromTXT(line,line_offset);
            ((sprite_linear *)movement_pointers[i])->Vy_fine = getIntFromTXT(line,line_offset);
            for(j=0;j<num_sprite_linears;j++){
                if(*((sprite_linear *)movement_pointers[i]) == *sprite_linears[j]){
                    delete ((sprite_linear *)movement_pointers[i]);
                    movement_pointers[i] = sprite_linears[j];
                    break;
                }
            }
            if(j>=num_sprite_linears){
                sprite_linears[num_sprite_linears++] = (sprite_linear *)movement_pointers[i];
            }
        }
        else if(std::string::npos != line.find("SHAKE")){
            move_types += 2;
            movement_pointers[i] = new sprite_shake;
            if(movement_pointers[i] == NULL){
                QMessageBox::critical(NULL,QString("OUT OF MEMORY"),QString("Failed to allocate enough memory for movement pointers."));
            }
            line_offset = line.find("SHAKE") + 5;
            ((sprite_shake *)movement_pointers[i])->id = num_sprite_shakes;
            ((sprite_shake *)movement_pointers[i])->references = 0;
            ((sprite_shake *)movement_pointers[i])->num_steps = getIntFromTXT(line,line_offset);
            ((sprite_shake *)movement_pointers[i])->x = "";
            ((sprite_shake *)movement_pointers[i])->y = "";
            if(std::string::npos != line.find("TIMER")){
                line_offset = line.find("TIMER") + 5;
                ((sprite_shake *)movement_pointers[i])->timer = getIntFromTXT(line,line_offset);
            }
            else{
                ((sprite_shake *)movement_pointers[i])->timer = 0;
            }
            for(j=0;j<((sprite_shake *)movement_pointers[i])->num_steps;j++){
                line = getLine(text,offset);
                lineUpper(line);
                if(std::string::npos != line.find("STEP")){
                    line_offset = line.find("STEP") + 4;
                    getIntFromTXT(line,line_offset);
                    ((sprite_shake *)movement_pointers[i])->x += (uint8_t)getIntFromTXT(line,line_offset);
                    ((sprite_shake *)movement_pointers[i])->y += (uint8_t)getIntFromTXT(line,line_offset);
                }
                else{
                    ((sprite_shake *)movement_pointers[i])->x += ((char)0);
                    ((sprite_shake *)movement_pointers[i])->y += ((char)0);
                }
            }
            for(j=0;j<num_sprite_shakes;j++){
                if(*((sprite_shake *)movement_pointers[i]) == *sprite_shakes[j]){
                    delete ((sprite_shake *)movement_pointers[i]);
                    movement_pointers[i] = sprite_shakes[j];
                    break;
                }
            }
            if(j>=num_sprite_shakes){
                sprite_shakes[num_sprite_shakes++] = (sprite_shake *)movement_pointers[i];
            }
        }
        else{   //Default to Fixed movement
            move_types += ((char)0);
            movement_pointers[i] = NULL;
        }
    }
}

void cmd_spriteset::addRef(uint8_t scene_num){
    pals[0]->references |= (1<<scene_num);
    pals[1]->references |= (1<<scene_num);
    pals[2]->references |= (1<<scene_num);
    pals[3]->references |= (1<<scene_num);
    for(unsigned int i=0;i<num_arrangements;i++){
        if(animation[i]){
            ((sprite_animation *)arrangements[i])->references |= (1<<scene_num);
            for(unsigned int j=0;j<((sprite_animation *)arrangements[i])->num_steps;j++){
                ((sprite_animation *)arrangements[i])->sequence[j]->references |= (1<<scene_num);
            }
        }
        else{
            ((sprite *)arrangements[i])->references |= (1<<scene_num);
        }
        switch(((uint8_t)move_types[i])&3){
        case 0:
            break;
        case 1:
            ((sprite_linear *)movement_pointers[i])->references |= (1<<scene_num);
            break;
        case 2:
            ((sprite_shake *)movement_pointers[i])->references |= (1<<scene_num);
            break;
        case 3:
            ((sprite_accel *)movement_pointers[i])->references |= (1<<scene_num);
            break;
        }
    }
}

QString cmd_spriteset::getName(){
    return (num_arrangements) ? QString("SPRITESET") : QString("SPRITECLEAR");
}
