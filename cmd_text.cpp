#include "cmd_text.h"
#include <QMessageBox>

cmd_text::cmd_text()
{
    command = 0xC0;
}

bool cmd_text::killSignal(){
    return false;
}

uint32_t cmd_text::parseData(uint8_t * data, uint32_t scene_pointer,uint8_t scene_number){
    command = data[scene_pointer++];
    dialog = getText(data, data[scene_pointer++]);
    dialog->references |= (1<<scene_number);
    unsigned int i,j,k;
    for(i=0;i<dialog->data.size();i++){
        if(dialog->is_animation[i]){
            ((sprite_animation *)dialog->data[i])->references |= (1<<scene_number);
            ((sprite_animation *)dialog->data[i])->text_engine = true;
            for(j=0;j<((sprite_animation*)dialog->data[i])->num_steps;j++){
                ((sprite_animation*)dialog->data[i])->sequence[j]->references |= (1<<scene_number);
                for(k=0;k<((sprite_animation*)dialog->data[i])->sequence[j]->pals.size();k++){
                    if(((sprite_animation*)dialog->data[i])->sequence[j]->pals[k] == current_palettes[1]) break;
                }
                if(k>=((sprite_animation*)dialog->data[i])->sequence[j]->pals.size()){
                    ((sprite_animation*)dialog->data[i])->sequence[j]->pals.push_back(current_palettes[1]);
                }
                current_sprites.push_back(((sprite_animation*)dialog->data[i])->sequence[j]);
                sprite_CHR_page = ((sprite_animation*)dialog->data[i])->sequence[j]->gfx_page & 0x3F;
            }
        }
        else{
            ((sprite *)dialog->data[i])->references |= (1<<scene_number);
            for(j=0;j<((sprite*)dialog->data[i])->pals.size();j++){
                if(((sprite*)dialog->data[i])->pals[j] == current_palettes[1]) break;
            }
            if(j>=((sprite*)dialog->data[i])->pals.size()){
                ((sprite*)dialog->data[i])->pals.push_back(current_palettes[1]);
            }
            current_sprites.push_back((sprite*)dialog->data[i]);
            sprite_CHR_page = ((sprite*)dialog->data[i])->gfx_page & 0x3F;
        }
    }
    return scene_pointer;
}

uint32_t cmd_text::writeToScene(uint8_t * data, uint32_t location){
    data[location++] = command;
    data[location++] = dialog->id;
    return location;
}

std::string cmd_text::writeToTXT(){
    std::string result = "TEXT(";
    result += convertToASCII(command&0xf);
    result += "):\n  ";
    uint8_t value;
    if(dialog->text == ""){
        result += "  VARIABLE_TEXT\n";
        return result;
    }
    unsigned int i,j;
    unsigned int data_index = 0;
    for(i=0;i<dialog->text.length();i++){
        value = dialog->text[i];
        if(value<0xf6){
            if(((value>=0x20 && value<0x5E) || (value>=0x61 && value<0x7E)) && \
                    (value!=0x25) && (value!=0x27)){
                result += value;
            }
            else{
                switch(value){
                case 0x80:
                     result += ' ';
                     break;
                case 0x81:
                     result += '!';
                     break;
                case 0x82:
                     result += '\"';
                     break;
                case 0x83:
                     result += '#';
                     break;
                case 0x84:
                     result += '$';
                     break;
                case 0x85:
                case 0x25:
                     result += '\'';
                     break;
                case 0x86:
                     result += '&';
                     break;
                case 0x87:
                case 0x27:
                     result += '%';
                     break;
                case 0x88:
                     result += '(';
                     break;
                case 0x89:
                     result += ')';
                     break;
                case 0x8c:
                     result += ',';
                     break;
                case 0x8d:
                     result += '-';
                     break;
                case 0x8e:
                     result += '.';
                     break;
                case 0x8f:
                     result += '/';
                     break;
                case 0x90:
                     result += '0';
                     break;
                case 0x91:
                     result += '1';
                     break;
                case 0x92:
                     result += '2';
                     break;
                case 0x93:
                     result += '3';
                     break;
                case 0x94:
                     result += '4';
                     break;
                case 0x95:
                     result += '5';
                     break;
                case 0x96:
                     result += '6';
                     break;
                case 0x97:
                     result += '7';
                     break;
                case 0x98:
                     result += '8';
                     break;
                case 0x99:
                     result += '9';
                     break;
                case 0x9a:
                     result += ':';
                     break;
                case 0x9b:
                     result += ';';
                     break;
                /*case 0x9c:
                     result += '<';
                     break;*/
                case 0x9d:
                     result += '=';
                     break;
                /*case 0x9e:
                     result += '>';
                     break;*/
                case 0x9f:
                     result += '?';
                     break;
                default:
                     result += '<' + convertByteToHexString(value) + '>';
                     break;
                }
            }
        }
        else{
            switch(value){
                case 0xff:
                     result += "<BREAK>\n  ";
                     break;
                case 0xfe:
                     result += "<SPRITESWAP-";
                     result += convertToASCII((dialog->text[i+1]>>6)&0x3);
                     result += ":";
                     result += convertByteToHexString(dialog->data.at(data_index++)->id) + ">";
                     //result += convertByteToHexString(dialog->text[i+1]&0x3F) + ">";
                     i++;
                     break;
                case 0xfd:
                {
                     result += "<ANIMATESPRITE ";
                     result += convertToASCII((dialog->text[i+1]>>6)&0x3);
                     result += ",TIMER=";
                     result += convertByteToHexString(((sprite_animation *)dialog->data.at(data_index))->timer);
                     result += ":";
                     for(j=0;j<((sprite_animation *)dialog->data.at(data_index))->num_steps;j++){
                         result += " " + convertByteToHexString(((sprite_animation *)dialog->data.at(data_index))->sequence[j]->id);
                     }
                     data_index++;
                     result +='>';
                     //result += convertByteToHexString(dialog->text[i+1]&0x3F) + ">";
                     i++;
                     break;
                }
                case 0xfB:
                     result += "<CLEAR>\n\n  ";
                     break;
                case 0xfa:
                     result += "<BREAK&INDENT>\n  ";
                     break;
                case 0xf9:
                     result += "<SET_INDENT " + convertByteToHexString(dialog->text[i+1]) + ">";
                     i++;
                     break;
                case 0xfC:
                     result += "<DELAY>";
                     break;
                case 0xf8:
                     result += "<END>\n\n";
                     break;
                case 0xf6:
                     result += "<DRAWBELOW ";
                     result += dialog->text[i+1];
                     result += dialog->text[i+2];
                     result += ">";
                     i+=2;
                     break;
                case 0xf7:
                     result += "<DRAWABOVE ";
                     result += dialog->text[i+1];
                     result += dialog->text[i+2];
                     result += ">";
                     i+=2;
                     break;
            }
        }
    }
    return result;
}

cmd_text::~cmd_text(){

}

void cmd_text::parseText(std::string text,std::string line,uint32_t & offset){
    uint32_t line_offset,temp_offset;
    int32_t temp_int;
    unsigned int i,j;
    line_offset = line.find("TEXT") + 4;
    temp_int = getIntFromTXT(line,line_offset);
    command |= temp_int&0xf;
    line = getLine(text,offset);
    std::string temp_line = line;
    lineUpper(temp_line);
    if(std::string::npos != temp_line.find("VARIABLE_TEXT")){
        for(i=0;i<num_dialogs;i++){
            if(!(all_dialog[i]->text.length())){
                dialog = all_dialog[i];
            }
        }
        if(i>=num_dialogs){
            dialog = new dialog_string;
            dialog->id = num_dialogs;
            dialog->references = 0;
            dialog->text = "";
            all_dialog[num_dialogs++] = dialog;
        }
        return;
    }
    dialog = new dialog_string;
    dialog->id = num_dialogs;
    dialog->references = 0;
    dialog->text = "";
    uint8_t value;
    std::string temp_string;
    //uint8_t indentation = 2;
    //line_offset = indentation;
    line_offset = 2;
    while(true){
        if(line_offset >= line.length()){
            //Error!

            QMessageBox::critical(NULL,QString("Bad text."),QString(dialog->text.c_str()));
        }
        value = line[line_offset++];
        if(((value>=0x20 && value<0x5E) || (value>=0x61 && value<0x7E)) && \
                (value!=0x25) && (value!=0x27) && (value!=0x3C)){
            dialog->text += value;
        }
        else if(value == 0x25 || value == 0x85){
            dialog->text += 0x27;
        }
        else if(value == 0x27 || value == 0x87){
            dialog->text += 0x25;
        }
        else if((value>=0x80 && value < 0x9C)){
            dialog->text = value - 0x60;
        }
        else if(value == 0x3C){
            temp_offset = line_offset;
            temp_string = getWordFromTXT(line,line_offset);
            lineUpper(temp_string);
            if(temp_string == "BREAK"){
                if(line[line_offset] == '>'){
                    dialog->text += 0xff;
                    line = getLine(text,offset);
                    line_offset = 2;
                }
                else{
                    dialog->text += 0xfa;
                    line = getLine(text,offset);
                    line_offset = 2;
                }
            }
            else if(temp_string == "SPRITESWAP"){
                dialog->text += 0xfe;
                temp_int = getIntFromTXT(line,line_offset);
                temp_int &= 0x3;
                temp_int <<= 6;
                j= temp_int;
                temp_int = getIntFromTXT(line,line_offset);
                dialog->text += j | (temp_int&0x3f);
                for(i=0;i<num_sprites;i++){
                    if(sprites[i]->id == temp_int) break;
                }
                if(i>=num_sprites) i=0;
                dialog->data.push_back(sprites[i]);
                dialog->is_animation.push_back(false);
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
            else if(temp_string == "ANIMATESPRITE"){
                dialog->text += 0xfd;
                std::vector<sprite *> sequence;
                sprite_animation * temp_animation = new sprite_animation;
                temp_animation->id = num_sprite_animations;
                temp_animation->references = 0;
                temp_animation->num_steps = 0;
                temp_animation->text_engine = true;
                temp_animation->sequence = NULL;

                temp_int = getIntFromTXT(line,line_offset);
                temp_int &= 0x3;
                temp_int <<= 6;
                j= temp_int;
                if(std::string::npos != line.find("TIMER",line_offset)){
                    line_offset = line.find("TIMER",line_offset) + 5;
                    temp_int = getIntFromTXT(line,line_offset);
                    temp_int &=0xf;
                    temp_int |= temp_int<<4;
                    temp_animation->timer = temp_int;
                }
                else{
                    temp_animation->timer = 0;
                }
                while(line_offset < line.length() && line[line_offset]!='>'){
                    temp_int = getIntFromTXT(line,line_offset);
                    for(i=0;i<num_sprites;i++){
                        if(sprites[i]->id == temp_int) break;
                    }
                    if(i>=num_sprites) i=0;
                    sequence.push_back(sprites[i]);
                    temp_animation->num_steps++;
                }
                line_offset++;
                temp_animation->sequence = new sprite * [temp_animation->num_steps];
                for(i=0;i<temp_animation->num_steps;i++){
                    temp_animation->sequence[i] = sequence.at(i);
                }
                for(i=0;i<num_sprite_animations;i++){
                    if(*temp_animation == *sprite_animations[i]){
                        delete temp_animation;
                        dialog->data.push_back(sprite_animations[i]);
                        break;
                    }
                }
                if(i>=num_sprite_animations) dialog->data.push_back(temp_animation);
                dialog->text += j | (i&0x3f);
                dialog->is_animation.push_back(true);
            }
            else if(temp_string == "CLEAR"){
                dialog->text += 0xFB;
                //getLine(text,offset);
                line = getLine(text,offset);
                line_offset = 2;
            }
            else if(temp_string == "SET"){
                dialog->text += 0xF9;
                getWordFromTXT(line,line_offset);
                dialog->text += (uint8_t)getIntFromTXT(line,line_offset);
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
            else if(temp_string == "DELAY"){
                dialog->text += 0xFC;
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
            else if(temp_string == "END"){
                dialog->text += 0xF8;
                //getLine(text,offset);
               // line = getLine(text,offset);
                for(i=0;i<num_dialogs;i++){
                    if(*dialog == *all_dialog[i]){
                        delete dialog;
                        dialog = all_dialog[i];
                        break;
                    }
                }
                if(i>=num_dialogs) all_dialog[num_dialogs++] = dialog;
                return;
            }
            else if(temp_string == "DRAWBELOW"){
                dialog->text += 0xF6;
                line_offset++;
                dialog->text += line[line_offset++];
                dialog->text += line[line_offset++];
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
            else if(temp_string == "DRAWABOVE"){
                dialog->text += 0xF7;
                line_offset++;
                dialog->text += line[line_offset++];
                dialog->text += line[line_offset++];
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
            else{
                line_offset = temp_offset;
                temp_int = getIntFromTXT(line,line_offset);
                dialog->text += (uint8_t) temp_int;
                while(line_offset < line.length() && line[line_offset]!='>') line_offset++;
                line_offset++;
            }
        }
    }
}

void cmd_text::addRef(uint8_t scene_num){
    dialog->references |= (1<<scene_num);
    for(unsigned int i=0;i<dialog->data.size();i++){
        dialog->data.at(i)->references |= (1<<scene_num);
        if(dialog->is_animation.at(i)){
            for(unsigned int j=0;j<((sprite_animation *)dialog->data.at(i))->num_steps;j++){
                ((sprite_animation *)dialog->data.at(i))->sequence[j]->references |= (1<<scene_num);
            }
        }
    }
}
