#include "data_handler.h"
#include <QMessageBox>
#include <QString>
#include <fstream>
#include <QDir>

data_handler::data_handler()
{
    rom_data = NULL;
    start_timers = NULL;
    //scenes = NULL;
    num_scenes = 0;
}

void data_handler::importAll(uint8_t * rom){
    clearData();
    rom_data = rom;

    //extern const int16_t NES_COLORS_RGB[];
    num_scenes = 0;
    image_changed = false;

    sprite_timer_address = rom_data[ASM_ANIM_SPRITE_TIMER] + (rom_data[ASM_ANIM_SPRITE_TIMER + 1]<<8);
    sprite_timer_address = (sprite_timer_address&0x3FFF) + 0x10010;
    sprite_shake_timer_address = rom_data[ASM_FIXED_SPRITE_MOVEMENT_TIMER] + (rom_data[ASM_FIXED_SPRITE_MOVEMENT_TIMER+1]<<8);
    sprite_shake_timer_address = (sprite_shake_timer_address&0x3fff) + 0x10010;
    earthquake_timer_address = rom_data[ASM_EARTHQUAKE_TIMER] + (rom_data[ASM_EARTHQUAKE_TIMER+1]<<8);
    earthquake_timer_address = (earthquake_timer_address&0x3fff) + 0x10010;
    earthquake_ptr_address = rom_data[ASM_EARTHQUAKE_PTR_LO] + (rom_data[ASM_EARTHQUAKE_PTR_LO+1]<<8);
    earthquake_ptr_address = (earthquake_ptr_address&0x3fff) + 0x10010;
    scroll_setup_address = rom_data[ASM_SCROLL_SETUP_PTR_LO] + (rom_data[ASM_SCROLL_SETUP_PTR_LO+1]<<8);
    scroll_setup_address = (scroll_setup_address&0x3fff) + 0x10010;
    sprite_linear_address = rom_data[ASM_SPRITE_LINEAR_MOTION_0] + (rom_data[ASM_SPRITE_LINEAR_MOTION_0+1]<<8);
    sprite_linear_address = (sprite_linear_address&0x3fff) + 0x10010;
    sprite_accel_address = rom_data[ASM_SPRITE_ACCEL_MOTION_0] + (rom_data[ASM_SPRITE_ACCEL_MOTION_0+1]<<8);
    sprite_accel_address = (sprite_accel_address&0x3fff) + 0x10010;
    ppu_base_address = rom_data[ASM_PPU_BASE_ADDRESS_0] + (rom_data[ASM_PPU_BASE_ADDRESS_0+1]<<8);
    ppu_base_address = (ppu_base_address&0x3fff) + 0x10010;
    scene_ptr_address = rom_data[ASM_SCENE_PTR_LO] + (rom_data[ASM_SCENE_PTR_LO+1]<<8);
    scene_ptr_address = (scene_ptr_address&0x3fff) + 0x10010;
    spritexy_address = rom_data[ASM_SPRITEXY] + (rom_data[ASM_SPRITEXY+1]<<8);
    spritexy_address = (spritexy_address&0x3fff) + 0x14010;
    animated_sprite_ptr_address = rom_data[ASM_ANIM_SPRITE_PTR_LO] + (rom_data[ASM_ANIM_SPRITE_PTR_LO+1]<<8);
    animated_sprite_ptr_address = (animated_sprite_ptr_address&0x3fff) + 0x14010;
    sprite_shake_address = rom_data[ASM_SPRITE_SHAKE_LO] + (rom_data[ASM_SPRITE_SHAKE_LO+1]<<8);
    sprite_shake_address = (sprite_shake_address&0x3fff) + 0x14010;
    sprite_ptr_address = rom_data[ASM_SPRITE_PTR_LO] + (rom_data[ASM_SPRITE_PTR_LO+1]<<8);
    sprite_ptr_address = (sprite_ptr_address&0x3fff) + 0x14010;
    sprite_timer_txt_address = rom_data[ASM_ANIM_SPRITE_TIMER_TXT_ENGINE] + (rom_data[ASM_ANIM_SPRITE_TIMER_TXT_ENGINE+1]<<8);
    sprite_timer_txt_address = (sprite_timer_txt_address&0x3fff) + 0x14010;
    text_ptr_address = rom_data[ASM_TXT_PTR_LO] + (rom_data[ASM_TXT_PTR_LO+1]<<8);
    text_ptr_address = (text_ptr_address&0x3fff) + 0x14010;
    bg_arrangement_address = rom_data[ASM_BG_ARRANGEMENT_PTR_LO] + (rom_data[ASM_BG_ARRANGEMENT_PTR_LO+1]<<8);
    bg_arrangement_address = (bg_arrangement_address&0x3fff) + 0x18010;
    bg_tileset_address = rom_data[ASM_BG_TILESET_PTR_LO] + (rom_data[ASM_BG_TILESET_PTR_LO+1]<<8);
    bg_tileset_address = (bg_tileset_address&0x3fff) + 0x18010;
    bg_attrib_address = rom_data[ASM_BG_ATTRIB_PTR_LO] + (rom_data[ASM_BG_ATTRIB_PTR_LO+1]<<8);
    bg_attrib_address = (bg_attrib_address&0x3fff) + 0x18010;
    palette_ptr_address = rom_data[ASM_PALLETE_PTR_ADDRESS_LO] + (rom_data[ASM_PALLETE_PTR_ADDRESS_LO + 1]<<8);
    palette_ptr_address = (palette_ptr_address&0x3fff) + 0x10010;

    uint32_t scene_selector_limit_address = 0xffffff;
    uint32_t scene_pointer = 0;
    start_timers = new uint16_t[0x100];
    unsigned int i,j,k;

    for(i=0;i<0x29;i++) spritexy_values[i] = rom_data[spritexy_address + i];
    for(i=0;i<0x20;i++){
        if(spritexy_values[i] == 0xC8) break;
    }
    if(i>=0x20){  //If not present, we want to add 0xC8 to the original NG values for spritexy position.
        for(i=0;i<0x20;i++){
            for(j=i+1;j<0x20;j++){
                if(spritexy_values[j] == spritexy_values[i]){   //overwrite the first duplicate value
                    spritexy_values[j] = 0xC8;
                    rom_data[spritexy_address + j] = 0xC8;
                    break;
                }
            }
            if(j<0x20) break;
        }
    }

    uint32_t chr_offset = rom_data[4]<<0x0E;
    chr_offset+=0x10;
    //*********PUT CODE HERE TO INITIALIZE CHR DATA
    num_chr_pages = rom_data[5]<<1;
    CHR_pages = new CHR_page[num_chr_pages];
    for(i=0;i<num_chr_pages;i++){
        CHR_pages[i].address = chr_offset;
        CHR_pages[i].id = i;
        CHR_pages[i].num_used = 0;
        for(j=0;j<0x100;j++){
            CHR_pages[i].bg_used[j] = false;
            CHR_pages[i].sprite_used[j] = false;
            CHR_pages[i].t[j].checksum = 0;
            CHR_pages[i].t[j].id = j;
            for(k=0;k<0x10;k++){
                CHR_pages[i].t[j].checksum += rom_data[chr_offset];
                CHR_pages[i].t[j].t[k] = rom_data[chr_offset++];
            }
        }
    }

    script_command * temp_command;
    while((scene_ptr_address + (num_scenes<<1)) < scene_selector_limit_address){
        for(k=0;k<2;k++){
            for(i=0;i<4;i++){
                for(j=0;j<4;j++){
                    current_palettes[k].p[i].nes_colors[j] = 0x0F;
                }
            }
        }
        current_palettes[0].p[0] = *getPalette(rom_data,0);
        current_palettes[0].p[0].references = -1;
        current_sprites.clear();
        num_current_bgs=0;
        num_current_strings=0;

        scene_pointer = rom_data[scene_ptr_address + (num_scenes<<1)] + (rom_data[scene_ptr_address + 1 + (num_scenes<<1)]<<8);
        scene_pointer = (scene_pointer&0x3fff) + 0x10010;
        if(scene_pointer < scene_selector_limit_address) scene_selector_limit_address = scene_pointer;
        //if(scene_pointer == 0 || scene_pointer == 0xffff)
        start_timers[num_scenes] = rom_data[scene_pointer++];
        start_timers[num_scenes] += rom_data[scene_pointer++]<<8;
        if(start_timers[num_scenes] == 0xffff){
            num_scenes++;
            continue;
        }
        do{
            uint8_t byte_command = rom_data[scene_pointer];
            if(byte_command == 0xff){
                temp_command = new cmd_wait;
            }
            else if((byte_command&0xC0)==0x40){
                temp_command = new cmd_BGSET;
            }
            else if((byte_command&0xC0)==0x80){
                temp_command = new cmd_spriteset;
            }
            else if((byte_command&0xC0)==0xC0){
                temp_command = new cmd_text;
            }
            else{
                switch(byte_command){
                case 1:
                    temp_command = new cmd_flash;
                    break;
                case 2:
                    temp_command = new cmd_sync;
                    break;
                case 3:
                    temp_command = new cmd_scrollsetup;
                    break;
                case 4:
                    temp_command = new cmd_fade;
                    break;
                case 5:
                    temp_command = new cmd_mirroring;
                    break;
                case 6:
                    temp_command = new cmd_music;
                    break;
                case 7:
                    temp_command = new cmd_PPUtransmit;
                    break;
                case 8:
                    temp_command = new cmd_BGGFXpage;
                    break;
                case 9:
                    temp_command = new cmd_movesprite;
                    break;
                case 0xa:
                    temp_command = new cmd_BGCOL_MASK;
                    break;
                case 0xb:
                    temp_command = new cmd_scrollspeed;
                    break;
                case 0xc:
                    temp_command = new cmd_paletteset;
                    break;
                case 0xd:
                    temp_command = new cmd_earthquake;
                    break;
                case 0xe:
                case 0xf:
                    temp_command = new cmd_blackout;
                    break;
                case 0x10:
                    temp_command = new cmd_setup_ranking_text;
                    break;
                case 0x11:
                    temp_command = new cmd_bar_or_reset;
                    break;
                case 0x12:
                    temp_command = new cmd_setup_hard_mode;
                    break;
                case 0x13:
                    temp_command = new cmd_setup_boss_rush_text;
                    break;
                case 0x14:
                    temp_command = new cmd_end_scene_during_popcorn_viewer;
                    break;
                case 0x15:
                    temp_command = new cmd_halt_sprite1;
                    break;
                default:
                    temp_command = new cmd_unknown;
                    break;
                }
            }
            //QMessageBox::critical(NULL,QString(convert24BitToHexString(scene_pointer).c_str()),QString("OK"));
            scene_pointer = temp_command->parseData(rom_data,scene_pointer,num_scenes);
            scenes[num_scenes].push_back(temp_command);
        }while(!temp_command->killSignal());
        num_scenes++;
    }
    compactImages();
    for(i=0;i<num_bgs;i++){
        bgs[i]->generateImage();
    }
    for(i=0;i<num_sprites;i++){
        sprites[i]->generateImage();
    }
    for(i=0;i<num_ppu_strings;i++){
        ppu_strings[i]->generateImage();
    }
}

void data_handler::clearData(){
    rom_data=NULL;
    unsigned int i;
    for(i=0;i<0x100;i++){
        delete ppu_strings[i];
        ppu_strings[i] = NULL;
        delete ppu_bases[i];
        ppu_bases[i] = NULL;
        delete sprite_linears[i];
        sprite_linears[i] = NULL;
        delete sprite_accels[i];
        sprite_accels[i] = NULL;
        delete earthquakes[i];
        earthquakes[i] = NULL;
        delete sprite_shakes[i];
        sprite_shakes[i] = NULL;
        delete palettes[i];
        palettes[i] = NULL;
        delete bgs[i];
        bgs[i] = NULL;
        delete all_dialog[i];
        all_dialog[i] = NULL;
    }
    num_ppu_strings = 0;
    num_earthquakes = 0;
    num_ppu_bases = 0;
    num_sprite_linears = 0;
    num_sprite_accels = 0;
    num_sprite_shakes = 0;
    num_palettes = 0;
    num_bgs = 0;
    num_dialogs = 0;
    for(i=0;i<0x80;i++){
        delete sprite_animations[i];
        sprite_animations[i] = NULL;
        delete sprites[i];
        sprites[i] = NULL;
    }
    num_sprite_animations = 0;
    num_sprites = 0;
    for(i=0;i<0x40;i++){
        delete scroll_params_list[i];
        scroll_params_list[i] = NULL;
    }
    num_params = 0;
    num_current_bgs = 0;
    current_sprites.clear();
    num_current_strings = 0;
    delete [] CHR_pages;
    num_chr_pages = 0;

    for(unsigned int i=0;i<num_scenes;i++){
        for(unsigned int j=0;j<scenes[i].size();j++){
            delete scenes[i].at(j);      //Virtual destructors
        }
        scenes[i].clear();
    }
    delete [] start_timers;
    num_scenes=0;
}

uint8_t * data_handler::exportAll(){
    unsigned int i,j,k,l;
    std::string to_write;
    uint32_t pointer_start;
    uint8_t * data = rom_data;
    j=0;
    for(i=0;i<num_sprite_animations;i++){       //Order sprite animations
        if(sprite_animations[i]->references == 0){
            delete sprite_animations[i];
            sprite_animations[i] = NULL;
            continue;
        }
        if(sprite_animations[i]->text_engine){
            sprite_animations[i]->id = j++;
        }
    }
    for(i=0;i<num_sprite_animations;i++){
        if(!sprite_animations[i]) continue;
        if(!(sprite_animations[i]->text_engine)){
            sprite_animations[i]->id = j++;
        }
    }
    uint16_t final_animation_number = j;
    sprite_animation * temp_animation;
    for(j=0;j<final_animation_number;j++){
        for(i=j;i<num_sprite_animations;i++){
            if(sprite_animations[i] && (sprite_animations[i]->id == j)){
                temp_animation = sprite_animations[j];
                sprite_animations[j] = sprite_animations[i];
                sprite_animations[i] = temp_animation;
            }
        }
    }
        /*if(!sprite_animations[i]){
            k=i+1;
            while(k<num_sprite_animations && !sprite_animations[k]) k++;
            if(k>=num_sprite_animations) break;
            sprite_animations[i] = sprite_animations[k];
            sprite_animations[k] = NULL;
        }
    }
    num_sprite_animations = j;*/
    num_sprite_animations = final_animation_number;
    //order sprite shakes
    j=0;
    for(i=0;i<num_sprite_shakes;i++){
        if(sprite_shakes[i]->references == 0){
            delete sprite_shakes[i];
            sprite_shakes[i] = NULL;
            continue;
        }
        sprite_shakes[i]->id = j++;
    }
    for(i=0;i<num_sprite_shakes;i++){
        if(!sprite_shakes[i]){
            k=i+1;
            while(k<num_sprite_shakes && !sprite_shakes[k]) k++;
            if(k>=num_sprite_shakes) break;
            sprite_shakes[i] = sprite_shakes[k];
            sprite_shakes[k] = NULL;
        }
    }
    num_sprite_shakes = j;

    //order earthquakes
    j=0;
    for(i=0;i<num_earthquakes;i++){
        if(earthquakes[i]->references == 0){
            delete earthquakes[i];
            earthquakes[i] = NULL;
            continue;
        }
        earthquakes[i]->id = j++;
    }
    for(i=0;i<num_earthquakes;i++){
        if(!earthquakes[i]){
            k=i+1;
            while(k<num_earthquakes && !earthquakes[k]) k++;
            if(k>=num_earthquakes) break;
            earthquakes[i] = earthquakes[k];
            earthquakes[k] = NULL;
        }
    }
    num_earthquakes = j;

    //order scroll setups
    j=0;
    for(i=0;i<num_params;i++){
        if(scroll_params_list[i]->references == 0){
            delete scroll_params_list[i];
            scroll_params_list[i] = NULL;
            continue;
        }
        scroll_params_list[i]->id = j++;
    }
    for(i=0;i<num_params;i++){
        if(!scroll_params_list[i]){
            k=i+1;
            while(k<num_params && !scroll_params_list[k]) k++;
            if(k>=num_params) break;
            scroll_params_list[i] = scroll_params_list[k];
            scroll_params_list[k] = NULL;
        }
    }
    num_params = j;

    //order sprite linears
    j=0;
    for(i=0;i<num_sprite_linears;i++){
        if(sprite_linears[i]->references == 0){
            delete sprite_linears[i];
            sprite_linears[i] = NULL;
            continue;
        }
        sprite_linears[i]->id = j++;
    }
    for(i=0;i<num_sprite_linears;i++){
        if(!sprite_linears[i]){
            k=i+1;
            while(k<num_sprite_linears && !sprite_linears[k]) k++;
            if(k>=num_sprite_linears) break;
            sprite_linears[i] = sprite_linears[k];
            sprite_linears[k] = NULL;
        }
    }
    num_sprite_linears = j;

    //order sprite accels
    j=0;
    for(i=0;i<num_sprite_accels;i++){
        if(sprite_accels[i]->references == 0){
            delete sprite_accels[i];
            sprite_accels[i] = NULL;
            continue;
        }
        sprite_accels[i]->id = j++;
    }
    for(i=0;i<num_sprite_accels;i++){
        if(!sprite_accels[i]){
            k=i+1;
            while(k<num_sprite_accels && !sprite_accels[k]) k++;
            if(k>=num_sprite_accels) break;
            sprite_accels[i] = sprite_accels[k];
            sprite_accels[k] = NULL;
        }
    }
    num_sprite_accels = j;

    //order palettes
    j=0;
    for(i=0;i<num_palettes;i++){
        if(palettes[i]->references == 0){
            delete palettes[i];
            palettes[i] = NULL;
            continue;
        }
        palettes[i]->id = j++;
    }
    for(i=0;i<num_palettes;i++){
        if(!palettes[i]){
            k=i+1;
            while(k<num_palettes && !palettes[k]) k++;
            if(k>=num_palettes) break;
            palettes[i] = palettes[k];
            palettes[k] = NULL;
        }
    }
    num_palettes = j;

    //order PPU base addresses
    j=0;
    for(i=0;i<num_ppu_bases;i++){
        if(ppu_bases[i]->references == 0){
            delete ppu_bases[i];
            ppu_bases[i] = NULL;
            continue;
        }
        ppu_bases[i]->id = j++;
    }
    for(i=0;i<num_ppu_bases;i++){
        if(!ppu_bases[i]){
            k=i+1;
            while(k<num_ppu_bases && !ppu_bases[k]) k++;
            if(k>=num_ppu_bases) break;
            ppu_bases[i] = ppu_bases[k];
            ppu_bases[k] = NULL;
        }
    }
    num_ppu_bases = j;

    //order dialog sets
    j=0;
    for(i=0;i<num_dialogs;i++){
        if(all_dialog[i]->references == 0){
            delete all_dialog[i];
            all_dialog[i] = NULL;
            continue;
        }
        all_dialog[i]->id = j++;
    }
    for(i=0;i<num_dialogs;i++){
        if(!all_dialog[i]){
            k=i+1;
            while(k<num_dialogs && !all_dialog[k]) k++;
            if(k>=num_dialogs) break;
            all_dialog[i] = all_dialog[k];
            all_dialog[k] = NULL;
        }
    }
    num_dialogs = j;
    for(i=0;i<num_dialogs;i++){
        if(all_dialog[i]->id != i){
            break;
        }
    }

    if(!compactImages()){    //Allocate CHR data
        //Images could not fit.  Abort writing data to ROM.

        return NULL;
    }



    //Put all images in order
    //bank 4
    //order PPU transmission strings
    j=0;
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->references == 0){
            delete ppu_strings[i];
            ppu_strings[i] = NULL;
            continue;
        }
        ppu_strings[i]->id = j++;
    }
    for(i=0;i<num_ppu_strings;i++){
        if(!ppu_strings[i]){
            k=i+1;
            while(k<num_ppu_strings && !ppu_strings[k]) k++;
            if(k>=num_ppu_strings) break;
            ppu_strings[i] = ppu_strings[k];
            ppu_strings[k] = NULL;
        }
    }
    num_ppu_strings = j;

    //bank 5
    //order sprite pointers
    j=0;
    for(i=0;i<num_sprites;i++){
        if(sprites[i]->references == 0){
            delete sprites[i];
            sprites[i] = NULL;
            continue;
        }
        sprites[i]->id = j++;
    }
    for(i=0;i<num_sprites;i++){
        if(!sprites[i]){
            k=i+1;
            while(k<num_sprites && !sprites[k]) k++;
            if(k>=num_sprites) break;
            sprites[i] = sprites[k];
            sprites[k] = NULL;
        }
    }
    num_sprites = j;

    //bank 6
    //order bg arrangements
    j=0;
    for(i=0;i<num_bgs;i++){
        if(bgs[i]->references == 0){
            delete bgs[i];
            bgs[i] = NULL;
            continue;
        }
        bgs[i]->id = j++;
    }
    for(i=0;i<num_bgs;i++){
        if(!bgs[i]){
            k=i+1;
            while(k<num_bgs && !bgs[k]) k++;
            if(k>=num_bgs) break;
            bgs[i] = bgs[k];
            bgs[k] = NULL;
        }
    }
    num_bgs = j;


    std::vector <metatile_set> metatiles;
    metatile_set current_set;
    current_set.id = 0;
    current_set.num_metatiles = 0;
    bool skip_arrangement[0x100];
    for(i=0;i<0x100;i++){
        skip_arrangement[i] = false;
    }
    //Create metatiles for BG arrangements

    while(true){
        for(i=0;i<num_bgs;i++){
            if(bgs[i]->references && !skip_arrangement[i]) break;
        }
        if(i>=num_bgs) break;
        for(i=0;i<num_bgs;i++){
            if(skip_arrangement[i] || !bgs[i]->references) continue;
            uint16_t metatile_index;
            uint8_t arrangement_size = (bgs[i]->metatilesX) * (bgs[i]->metatilesY);
            uint16_t old_num_metatiles = current_set.num_metatiles;
            for(j=0;j<arrangement_size;j++){
                metatile_index = 0xffff;
                bgs[i]->metatiles[j].checksum = 0;
                for(k=0;k<8;k++){
                    bgs[i]->metatiles[j].tiles[k] = bgs[i]->tiles[(j<<3) + k];
                    bgs[i]->metatiles[j].checksum += bgs[i]->metatiles[j].tiles[k]->id;
                }
                bgs[i]->metatiles[j].attrib_bits = (j&1) ? bgs[i]->attrib_bits[j>>1] & 0xf : (bgs[i]->attrib_bits[j>>1]>>4)&0xf;
                metatile_index = current_set.add(bgs[i]->metatiles[j]);
                if(metatile_index == 0xffff){
                    //too many metatiles added to this set.  Restore the previous number of metatiles in the set and skip the rest of this arrangement
                    current_set.num_metatiles = old_num_metatiles;
                    break;
                }
                else{
                    bgs[i]->metatiles[j].id = metatile_index;   //Store the metatile index within the bg_arrangement for later use.
                }
            }
            if(j<arrangement_size) continue;
            skip_arrangement[i] = true;
            bgs[i]->tileset = current_set.id;
            if(current_set.num_metatiles == 0x100){
                metatiles.push_back(current_set);
                current_set.num_metatiles = 0;
                current_set.id++;
            }
        }
        metatiles.push_back(current_set);
        current_set.num_metatiles = 0;
        current_set.id++;
    }

    uint32_t write_position = BANK4_WRITE_START;
    uint32_t nes_pointer;
    //write animated sprite timers
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_ANIM_SPRITE_TIMER] = nes_pointer;
    data[ASM_ANIM_SPRITE_TIMER + 1] = nes_pointer>>8;

    for(i=0;i<num_sprite_animations;i++){
        rom_data[write_position++] = sprite_animations[i]->timer;
    }
    //write sprite shake timers
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_FIXED_SPRITE_MOVEMENT_TIMER] = nes_pointer;
    data[ASM_FIXED_SPRITE_MOVEMENT_TIMER + 1] = nes_pointer>>8;
    for(i=0;i<num_sprite_shakes;i++){
        rom_data[write_position++] = sprite_shakes[i]->timer;
    }
    //write earthquake timers
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_EARTHQUAKE_TIMER] = nes_pointer;
    data[ASM_EARTHQUAKE_TIMER + 1] = nes_pointer>>8;
    for(i=0;i<num_earthquakes;i++){
        rom_data[write_position++] = earthquakes[i]->timer;
    }
    //write earthquake data and pointers
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_EARTHQUAKE_PTR_LO] = nes_pointer;
    data[ASM_EARTHQUAKE_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0f;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_EARTHQUAKE_PTR_HI] = nes_pointer;
    data[ASM_EARTHQUAKE_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(num_earthquakes<<1);
    for(k=0;k<num_earthquakes;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = earthquakes[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write scroll setup pointers
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCROLL_SETUP_PTR_LO] = nes_pointer;
    data[ASM_SCROLL_SETUP_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0f;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCROLL_SETUP_PTR_HI] = nes_pointer;
    data[ASM_SCROLL_SETUP_PTR_HI + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCROLL_SETUP_PTR_LO_ALT] = nes_pointer;
    data[ASM_SCROLL_SETUP_PTR_LO_ALT + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0f;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCROLL_SETUP_PTR_HI_ALT] = nes_pointer;
    data[ASM_SCROLL_SETUP_PTR_HI_ALT + 1] = nes_pointer>>8;
    write_position+=(num_params<<1);
    for(k=0;k<num_params;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = scroll_params_list[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write sprite linears
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_LINEAR_MOTION_0] = nes_pointer;
    data[ASM_SPRITE_LINEAR_MOTION_0 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_LINEAR_MOTION_1] = nes_pointer;
    data[ASM_SPRITE_LINEAR_MOTION_1 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0E;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_LINEAR_MOTION_2] = nes_pointer;
    data[ASM_SPRITE_LINEAR_MOTION_2 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0D;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_LINEAR_MOTION_3] = nes_pointer;
    data[ASM_SPRITE_LINEAR_MOTION_3 + 1] = nes_pointer>>8;
    for(k=0;k<num_sprite_linears;k++){
        to_write = sprite_linears[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write sprite accels
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_0] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_0 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_1] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_1 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0E;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_2] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_2 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0D;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_3] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_3 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0C;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_4] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_4 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0B;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_ACCEL_MOTION_5] = nes_pointer;
    data[ASM_SPRITE_ACCEL_MOTION_5 + 1] = nes_pointer>>8;
    for(k=0;k<num_sprite_accels;k++){
        to_write = sprite_accels[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write palette pointers and data
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_PALLETE_PTR_ADDRESS_LO] = nes_pointer;
    data[ASM_PALLETE_PTR_ADDRESS_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_PALLETE_PTR_ADDRESS_HI] = nes_pointer;
    data[ASM_PALLETE_PTR_ADDRESS_HI + 1] = nes_pointer>>8;
    pointer_start = write_position;
    write_position+=4;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[pointer_start] = nes_pointer;
    data[pointer_start + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x10 + 0xFF;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[pointer_start + 2] = nes_pointer;
    data[pointer_start + 3] = nes_pointer>>8;
    for(k=0;k<num_palettes;k++){
        to_write = palettes[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //Re-order palette IDs to account for the 2 different pointer sets.
    for(i=0;i<num_palettes;i++){
        if(i>=0x55){
            palettes[i]->id = ((i - 0x55)|0x80);
        }
        else{
            palettes[i]->id = i;
        }
    }
    //write PPU transmission strings
    for(k=0;k<num_ppu_strings;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        ppu_strings[k]->address = nes_pointer;
        to_write = ppu_strings[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write PPU base addresses
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_PPU_BASE_ADDRESS_0] = nes_pointer;
    data[ASM_PPU_BASE_ADDRESS_0 + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_PPU_BASE_ADDRESS_1] = nes_pointer;
    data[ASM_PPU_BASE_ADDRESS_1 + 1] = nes_pointer>>8;
    for(k=0;k<num_ppu_bases;k++){
        to_write = ppu_bases[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }

    //WRITE SCENE DATA HERE
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCENE_PTR_LO] = nes_pointer;
    data[ASM_SCENE_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SCENE_PTR_HI] = nes_pointer;
    data[ASM_SCENE_PTR_HI + 1] = nes_pointer>>8;
    write_position += (num_scenes<<1);
    for(i=0;i<num_scenes;i++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (i<<1)] = nes_pointer;
        data[pointer_start + 1 + (i<<1)] = nes_pointer>>8;
        data[write_position++] = start_timers[i];
        data[write_position++] = start_timers[i]>>8;
        for(j=0;j<scenes[i].size();j++){
            write_position = scenes[i].at(j)->writeToScene(data,write_position);
        }
    }



    //bank 5
    write_position = BANK5_WRITE_START;
    //write pointers and data for animated sprite sequences
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_ANIM_SPRITE_PTR_LO] = nes_pointer;
    data[ASM_ANIM_SPRITE_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_ANIM_SPRITE_PTR_HI] = nes_pointer;
    data[ASM_ANIM_SPRITE_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(num_sprite_animations<<1);
    for(k=0;k<num_sprite_animations;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = sprite_animations[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write sprite shake pointers and data
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_SHAKE_LO] = nes_pointer;
    data[ASM_SPRITE_SHAKE_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_SHAKE_HI] = nes_pointer;
    data[ASM_SPRITE_SHAKE_HI + 1] = nes_pointer>>8;
    write_position+=(num_sprite_shakes<<1);
    for(k=0;k<num_sprite_shakes;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = sprite_shakes[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write sprite pointers and data
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_PTR_LO] = nes_pointer;
    data[ASM_SPRITE_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_SPRITE_PTR_HI] = nes_pointer;
    data[ASM_SPRITE_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(num_sprites<<1);
    for(k=0;k<num_sprites;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = sprites[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    //write txt engine animated sprite timers
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_ANIM_SPRITE_TIMER_TXT_ENGINE] = nes_pointer;
    data[ASM_ANIM_SPRITE_TIMER_TXT_ENGINE + 1] = nes_pointer>>8;
    for(k=0;k<num_sprite_animations;k++){
        if(!(sprite_animations[k]->text_engine)) continue;
        rom_data[write_position++] = sprite_animations[k]->timer;
    }
    //write text pointers and data
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_TXT_PTR_LO] = nes_pointer;
    data[ASM_TXT_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_TXT_PTR_HI] = nes_pointer;
    data[ASM_TXT_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(num_dialogs<<1);
    for(k=0;k<num_dialogs;k++){
        if(!all_dialog[k]->text.length()){
            data[pointer_start + (k<<1)] = (char)0;
            data[pointer_start + 1 + (k<<1)] = (char)7;
            continue;
        }
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = all_dialog[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }

    //bank 6
    write_position = BANK6_WRITE_START;
    //write bg arrangement pointers and data
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_ARRANGEMENT_PTR_LO] = nes_pointer;
    data[ASM_BG_ARRANGEMENT_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_ARRANGEMENT_PTR_HI] = nes_pointer;
    data[ASM_BG_ARRANGEMENT_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(num_bgs<<1);
    for(k=0;k<num_bgs;k++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start + (k<<1)] = nes_pointer;
        data[pointer_start + 1 + (k<<1)] = nes_pointer>>8;
        to_write = bgs[k]->write();
        for(l=0;l<to_write.length();l++){
            rom_data[write_position++] = to_write[l];
        }
    }
    pointer_start = write_position;
    nes_pointer = write_position - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_TILESET_PTR_LO] = nes_pointer;
    data[ASM_BG_TILESET_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = write_position - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_TILESET_PTR_HI] = nes_pointer;
    data[ASM_BG_TILESET_PTR_HI + 1] = nes_pointer>>8;
    write_position+=(metatiles.size()<<2);
    for(std::vector<metatile_set>::iterator meta_it = metatiles.begin();meta_it < metatiles.end();meta_it++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start++] = nes_pointer;
        data[pointer_start++] = nes_pointer>>8;
        for(j=0;j<(*meta_it).num_metatiles;j++){
            to_write = (*meta_it).set[j].write();
            for(k=0;k<to_write.length();k++) rom_data[write_position++] = to_write[k];
        }
    }
    nes_pointer = pointer_start - 0x10;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_ATTRIB_PTR_LO] = nes_pointer;
    data[ASM_BG_ATTRIB_PTR_LO + 1] = nes_pointer>>8;
    nes_pointer = pointer_start - 0x0F;
    nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
    data[ASM_BG_ATTRIB_PTR_HI] = nes_pointer;
    data[ASM_BG_ATTRIB_PTR_HI + 1] = nes_pointer>>8;
    for(std::vector<metatile_set>::iterator meta_it = metatiles.begin();meta_it < metatiles.end();meta_it++){
        nes_pointer = write_position - 0x10;
        nes_pointer = (nes_pointer&0x3FFF) | 0x8000;
        data[pointer_start++] = nes_pointer;
        data[pointer_start++] = nes_pointer>>8;
        uint8_t temp_attrib = 0;
        for(j=0;j<(*meta_it).num_metatiles;j++){
            if(j&1){
                temp_attrib |= (*meta_it).set[j].attrib_bits & 0xf;
                rom_data[write_position++] = temp_attrib;
            }
            else{
                temp_attrib = (*meta_it).set[j].attrib_bits <<4;
            }
        }
        if(j&1) rom_data[write_position++] = temp_attrib;
        temp_attrib = 0;
    }
    for(i=0x11;i<num_chr_pages;i++){
        for(j=0;j<0x100;j++){
            if(CHR_pages[i].bg_used[j] || CHR_pages[i].sprite_used[j]){
                for(k=0;k<0x10;k++){
                    rom_data[CHR_pages[i].address + (j<<4) + k] = CHR_pages[i].t[j].t[k];
                }
            }
            else if(i<0x20){        //Mark unused tiles with an easy to identify striped pattern, but only in CHR regions used solely for scene data
                for(k=0;k<0x10;k+=2){
                    rom_data[CHR_pages[i].address + (j<<4) + k] = 0xFF;
                    rom_data[CHR_pages[i].address + (j<<4) + k+1] = 0;
                }
            }
        }
    }
    return rom_data;
}

void data_handler::writeToTXT(std::string file_base){
    unsigned int i,j;
    std::ofstream outfile;
    std::string f_name;

    f_name = file_base + "/Scenes";
    QDir dirscene(f_name.c_str());
    if(!dirscene.exists()) dirscene.mkpath(f_name.c_str());
    for(i=0;i<num_scenes;i++){
        f_name = file_base + "/Scenes/SCENE" + convertByteToHexString(i) + ".txt";
        //QMessageBox::critical(NULL,QString("File Name"),QString(f_name.c_str()));
        outfile.open(f_name.c_str());
        if(!outfile){
            //ERROR HANDLER
            QMessageBox::critical(NULL,QString("ERROR!"),QString("File could not be opened."));
            return;
        }
        outfile.clear();
        outfile<<"TIMER = ";
        outfile<<convertByteToHexString(start_timers[i]>>8);
        outfile<<convertByteToHexString(start_timers[i]&0xff);
        outfile<<'\n';
        for(j=0;j<scenes[i].size();j++){
            outfile<<scenes[i].at(j)->writeToTXT();
        }
        outfile.close();
    }
    f_name = file_base + "/BGs";
    QDir dirbg(f_name.c_str());
    if(!dirbg.exists()) dirbg.mkpath(f_name.c_str());
    for(i=0;i<num_bgs;i++){
        f_name = file_base + "/BGs/BG" + convertByteToHexString(bgs[i]->id) + ".png";
        bgs[i]->image.save(QString(f_name.c_str()),0,-1);
    }
    f_name = file_base + "/Sprites";
    QDir dirsprites(f_name.c_str());
    if(!dirsprites.exists()) dirsprites.mkpath(f_name.c_str());
    for(i=0;i<num_sprites;i++){
        f_name = file_base + "/Sprites/SPRITE" + convertByteToHexString(sprites[i]->id) + ".png";
        sprites[i]->image.save(QString(f_name.c_str()),0,-1);
    }
    f_name = file_base + "/DynamicImages";
    QDir dirdynamic(f_name.c_str());
    if(!dirdynamic.exists()) dirdynamic.mkpath(f_name.c_str());
    for(i=0;i<num_ppu_strings;i++){
        f_name = file_base + "/DynamicImages/DYNAMIC" + convertByteToHexString(ppu_strings[i]->id) + ".png";
        ppu_strings[i]->image.save(QString(f_name.c_str()),0,-1);
    }
}

bool data_handler::compactImages(){
    unsigned int i,j,k;

    //Clear the "used" boolean for all cut-scene CHR pages
    for(i=0x11;i<num_chr_pages;i++){    //Do not re-order CHR pages below x11
        for(j=0;j<0x100;j++){
            CHR_pages[i].bg_used[j] = (j>=0xF8);
            CHR_pages[i].sprite_used[j] = (j<=0xF8);
            CHR_pages[i].t[j].attrib_changes = 0;
        }
    }

    //Mark BG and sprite tiles that are currently used in each CHR page
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->references && !ppu_strings[i]->modified){
            for(j=0;j<ppu_strings[i]->tiles.size();j++){
                CHR_pages[ppu_strings[i]->gfx_page].bg_used[ppu_strings[i]->tiles[j]->id] = true;
            }
        }
    }

    for(i=0;i<num_bgs;i++){
        if(bgs[i]->references && !bgs[i]->modified){
            for(j=0;j<bgs[i]->tiles.size();j++){
                CHR_pages[bgs[i]->gfx_page].bg_used[bgs[i]->tiles[j]->id] = true;
            }
        }
    }

    for(i=0;i<num_sprites;i++){
        uint8_t tile_count = 0;
        if(sprites[i]->references && !sprites[i]->modified){
            for(j=0;j<sprites[i]->arrangement.length();){
                if(sprites[i]->arrangement[j]&0x80) break;
                j++;    //Move past X position byte of column
                uint8_t column_size = sprites[i]->arrangement[j++];    //Get the number of tiles in the column
                for(k=0;k<column_size;k++){
                    if(sprites[i]->arrangement[j++]&1){ //Tile value from the arrangement.  Only use the gfx page selector bit.
                        //CHR_pages[sprites[i]->gfx_page].used[sprites[i]->tiles[tile_count]->id] = true;
                        //CHR_pages[sprites[i]->gfx_page].used[sprites[i]->tiles[tile_count + 1]->id] = true;
                        CHR_pages[sprites[i]->gfx_page].sprite_used[sprites[i]->tiles[tile_count]->id] = true;
                        CHR_pages[sprites[i]->gfx_page].sprite_used[sprites[i]->tiles[tile_count + 1]->id] = true;

                        tile_count += 2;
                    }
                    else{
                        //CHR_pages[sprites[i]->bg_gfx_page].used[sprites[i]->tiles[tile_count]->id] = true;
                        //CHR_pages[sprites[i]->bg_gfx_page].used[sprites[i]->tiles[tile_count + 1]->id] = true;
                        CHR_pages[sprites[i]->bg_gfx_page].sprite_used[sprites[i]->tiles[tile_count]->id] = true;
                        CHR_pages[sprites[i]->bg_gfx_page].sprite_used[sprites[i]->tiles[tile_count + 1]->id] = true;
                        tile_count += 2;
                    }
                    j++; //move past the tile y position and attrib bits byte
                }
            }
        }
    }

    //SAVE SPACE HERE, MAKE WAY FOR MORE GRAPHICS
    bool skip_tile[0x100];
    //Check for duplicate sprite tiles
    for(i=0x11;i<num_chr_pages;i++){    //Do not re-order CHR pages below x11
        NEStile noflip[2];
        NEStile hflip[2];
        for(j=0;j<0xF8;j++) skip_tile[j] = false;
        for(;j<0x100;j++) skip_tile[j] = true;
        for(j=0xfe;((int)j)>=0;j-=2){
            noflip[0] = CHR_pages[i].t[j];
            noflip[1] = CHR_pages[i].t[j+1];
            hflip[0] = noflip[0];
            hflip[1] = noflip[1];
/*            vflip[0] = noflip[1];
            vflip[1] = noflip[0];
            hvflip[0] = noflip[1];
            hvflip[1] = noflip[0];*/
            hflip[0].hflip();
            hflip[1].hflip();
/*            vflip[0].vflip();
            vflip[1].vflip();
            hvflip[0].hvflip();
            hvflip[1].hvflip();*/
            for(k=j-2;((int)k)>=0;k-=2){
                if(skip_tile[k] || !CHR_pages[i].sprite_used[k]) continue;
                if((noflip[0] == CHR_pages[i].t[k]) && (noflip[1] == CHR_pages[i].t[k+1])){
                    CHR_pages[i].sprite_used[j] = true;
                    CHR_pages[i].sprite_used[j+1] = true;
                    CHR_pages[i].sprite_used[k] = false;
                    CHR_pages[i].sprite_used[k+1] = false;
                    CHR_pages[i].bg_used[j] |= CHR_pages[i].bg_used[k];
                    CHR_pages[i].bg_used[j+1] |= CHR_pages[i].bg_used[k+1];
                    CHR_pages[i].bg_used[k] = false;
                    CHR_pages[i].bg_used[k+1] = false;
                    CHR_pages[i].t[k].id = j;                   //All references to this tile will now point to the original instead of the duplicate
                    CHR_pages[i].t[k+1].id = j+1;
                    skip_tile[k] = true;
                    skip_tile[k+1] = true;
                }
                else if(!CHR_pages[i].bg_used[k] && !CHR_pages[i].bg_used[k+1]){
                    if((hflip[0] == CHR_pages[i].t[k]) && (hflip[1] == CHR_pages[i].t[k+1])){
                        CHR_pages[i].sprite_used[j] = true;
                        CHR_pages[i].sprite_used[j+1] = true;
                        CHR_pages[i].sprite_used[k] = false;
                        CHR_pages[i].sprite_used[k+1] = false;
                        CHR_pages[i].t[k].attrib_changes = 1;
                        CHR_pages[i].t[k+1].attrib_changes = 1;
                        CHR_pages[i].t[k].id = j;                   //All references to this tile will now point to the original instead of the duplicate
                        CHR_pages[i].t[k+1].id = j+1;
                        skip_tile[k] = true;
                        skip_tile[k+1] = true;
                        /*std::string temp_string = "Page ";
                        temp_string += convertByteToHexString(i);
                        temp_string += ", Base tile: ";
                        temp_string += convertByteToHexString(j);
                        temp_string += ", Dupe tile: ";
                        temp_string += convertByteToHexString(k);
                        QMessageBox::critical(NULL,QString("HFlip Duplicate"),QString(temp_string.c_str()));*/
                    }
/*                    else if((vflip[1] == CHR_pages[i].t[k]) && (vflip[0] == CHR_pages[i].t[k+1])  || \
                            (hvflip[1] == CHR_pages[i].t[k]) && (hvflip[0] == CHR_pages[i].t[k+1])){
                        CHR_pages[i].sprite_used[j] = true;
                        CHR_pages[i].sprite_used[j+1] = true;
                        CHR_pages[i].sprite_used[k] = false;
                        CHR_pages[i].sprite_used[k+1] = false;
                        CHR_pages[i].t[k].id = j+1;                   //All references to this tile will now point to the original instead of the duplicate
                        CHR_pages[i].t[k+1].id = j;
                        skip_tile[k] = true;
                        skip_tile[k+1] = true;
                    }*/
                }
            }
        }
        //Check for duplicate bg tiles within a graphics page
        for(j=0xff;((int)j)>=0;j--){
            for(k=j-1;((int)k)>=0;k--){
                if(skip_tile[k]) continue;
                if((CHR_pages[i].t[j] == CHR_pages[i].t[k]) && !CHR_pages[i].sprite_used[k]){
                    if(CHR_pages[i].bg_used[k]){
                        CHR_pages[i].bg_used[k] = false;
                        CHR_pages[i].bg_used[j] = true;
                    }
                    CHR_pages[i].t[k].id = j;                   //All references to this tile will now point to the original instead of the duplicate
                    skip_tile[k] = true;
                }
            }
        }
    }

    //Update tile pointers for PPU Strings
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->modified || !ppu_strings[i]->references) continue;
        for(j=0;j<ppu_strings[i]->tiles.size();j++){
            ppu_strings[i]->tiles[j] = &(CHR_pages[ppu_strings[i]->gfx_page].t[ppu_strings[i]->tiles[j]->id]);
        }
    }
    //Update tile pointers for BGs
    for(i=0;i<num_bgs;i++){
        if(bgs[i]->modified || !bgs[i]->references) continue;
        for(j=0;j<bgs[i]->tiles.size();j++){
            bgs[i]->tiles[j] = &(CHR_pages[bgs[i]->gfx_page].t[bgs[i]->tiles[j]->id]);
        }
    }
    //Update tile pointers for sprites
    //for sprites, also need to correct attribute data in the event of flips
    for(i=0;i<num_sprites;i++){
        if(sprites[i]->modified || !sprites[i]->references) continue;
        uint8_t tile_count = 0;
        for(j=0;j<sprites[i]->arrangement.length();){
            uint8_t tiles_in_column;
            j++;
            tiles_in_column = sprites[i]->arrangement[j++];
            for(k=0;k<tiles_in_column;k++){
                /*uint8_t arrangement_tile = sprites[i]->arrangement[j];
                uint8_t pointer_tile = sprites[i]->tiles[tile_count]->id;
                uint8_t attribs = sprites[i]->arrangement[j+1];
                if((arrangement_tile&0xFE)!=pointer_tile){
                    attribs ^= CHR_pages[(arrangement_tile&1)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].attrib_changes[arrangement_tile&0xFE];
                    sprites[i]->tiles[tile_count++] = &(CHR_pages[(arrangement_tile&1)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].t[pointer_tile]);
                    sprites[i]->tiles[tile_count++] = &(CHR_pages[(arrangement_tile&1)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].t[pointer_tile+1]);
                    sprites[i]->arrangement[j] = pointer_tile | (arrangement_tile&1);
                    sprites[i]->arrangement[j+1] = attribs;
                }*/
                uint8_t arrangement_tile = sprites[i]->arrangement[j]&0x01;
                uint8_t pointer_tile = sprites[i]->tiles[tile_count]->id;
                uint8_t attribs = sprites[i]->arrangement[j+1];
                //attribs ^= CHR_pages[(arrangement_tile)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].attrib_changes[pointer_tile];
                attribs ^= sprites[i]->tiles[tile_count]->attrib_changes;
                sprites[i]->tiles[tile_count++] = &(CHR_pages[(arrangement_tile)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].t[pointer_tile]);
                sprites[i]->tiles[tile_count++] = &(CHR_pages[(arrangement_tile)?sprites[i]->gfx_page : sprites[i]->bg_gfx_page].t[pointer_tile+1]);
                sprites[i]->arrangement[j] = pointer_tile | (arrangement_tile);
                sprites[i]->arrangement[j+1] = attribs;
                j+=2;
            }
        }
    }
    //Restore tile ID#'s.
    for(i=0;i<num_chr_pages;i++){
        for(j=0;j<0x100;j++){
            CHR_pages[i].t[j].id = j;
        }
    }
    for(i=0;i<num_ppu_strings;i++) ppu_strings[i]->modified = false;
    for(i=0;i<num_bgs;i++) bgs[i]->modified = false;
    for(i=0;i<num_sprites;i++) sprites[i]->modified = false;
    return true;
}

void data_handler::importScene(uint8_t scene_num,std::string scene_data){
    uint32_t offset = 0;
    unsigned int i;
    std::string line;
    uint32_t line_offset = 0;
    int32_t temp_int;

//    for(offset = 0;offset < scene_data.length();offset++) scene_data[offset] = toupper(scene_data[offset]);
    offset = 0;
    for(i=0;i<num_bgs;i++){
        bgs[i]->references = bgs[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_dialogs;i++){
        all_dialog[i]->references = all_dialog[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_earthquakes;i++){
        earthquakes[i]->references = earthquakes[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_palettes;i++){
        palettes[i]->references = palettes[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_params;i++){
        scroll_params_list[i]->references = scroll_params_list[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_ppu_bases;i++){
        ppu_bases[i]->references = ppu_bases[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_ppu_strings;i++){
        ppu_strings[i]->references = ppu_strings[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_sprites;i++){
        sprites[i]->references = sprites[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_sprite_accels;i++){
        sprite_accels[i]->references = sprite_accels[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_sprite_animations;i++){
        sprite_animations[i]->references = sprite_animations[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_sprite_linears;i++){
        sprite_linears[i]->references = sprite_linears[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<num_sprite_shakes;i++){
        sprite_shakes[i]->references = sprite_shakes[i]->references&(0xffffffffffffffff^(1<<scene_num));
    }
    for(i=0;i<scenes[scene_num].size();i++){
        delete scenes[scene_num].at(i);
    }
    scenes[scene_num].clear();

    do{
        line = getLine(scene_data,offset);
        lineUpper(line);
    }while(std::string::npos == line.find("TIMER"));
    line_offset = line.find("TIMER") + 5;
    temp_int = getIntFromTXT(line,line_offset);
    start_timers[scene_num] = (temp_int>=0)? temp_int : 0xffff;

    script_command * temp_command;
    while(offset < scene_data.length()){
        line = getLine(scene_data,offset);
        if(std::string::npos != line.find("FLASH")){
            temp_command = new cmd_flash;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SYNC")){
            temp_command = new cmd_sync;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SCROLL_SETUP")){
            temp_command = new cmd_scrollsetup;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_scrollsetup *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("CANCEL_SPLITS")){
            temp_command = new cmd_scrollsetup;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("FADE")){
            temp_command = new cmd_fade;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("MIRRORING")){
            temp_command = new cmd_mirroring;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("MUSIC")){
            temp_command = new cmd_music;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("PPU_TRANSMIT")){
            temp_command = new cmd_PPUtransmit;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_PPUtransmit *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("BG_GFXPAGE")){
            temp_command = new cmd_BGGFXpage;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("MOVE_SPRITE")){
            temp_command = new cmd_movesprite;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_movesprite *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("BGCOLOR")){
            temp_command = new cmd_BGCOL_MASK;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SCROLL")){
            temp_command = new cmd_scrollspeed;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_scrollspeed *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("PALETTE_SET")){
            temp_command = new cmd_paletteset;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_paletteset *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("EARTHQUAKE")){
            temp_command = new cmd_earthquake;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_earthquake *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("BLACKOUT")){
            temp_command = new cmd_blackout;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SETUP_RANKING_TEXT")){
            temp_command = new cmd_setup_ranking_text;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("BAR_OR_RESET")){
            temp_command = new cmd_bar_or_reset;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SETUP_HARD_MODE")){
            temp_command = new cmd_setup_hard_mode;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SETUP_BOSS_RUSH_TEXT")){
            temp_command = new cmd_setup_boss_rush_text;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("END_SCENE_DURING_POPCORN_VIEWER")){
            temp_command = new cmd_end_scene_during_popcorn_viewer;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SPRITE1")){
            temp_command = new cmd_halt_sprite1;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("SPRITESET")){
            temp_command = new cmd_spriteset;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_spriteset *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("SPRITECLEAR")){
            temp_command = new cmd_spriteset;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("BGSET")){
            temp_command = new cmd_BGSET;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_BGSET *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("BGCLEAR")){
            temp_command = new cmd_BGSET;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("TEXT")){
            temp_command = new cmd_text;
            temp_command->parseText(scene_data,line,offset);
            ((cmd_text *)temp_command)->addRef(scene_num);
        }
        else if(std::string::npos != line.find("WAIT")){
            temp_command = new cmd_wait;
            temp_command->parseText(scene_data,line,offset);
        }
        else if(std::string::npos != line.find("END_SCENE")){
            temp_command = new cmd_wait;
            temp_command->parseText(scene_data,line,offset);
        }
        else{   //else just continue until we find a line with one of these.
            continue;
        }
        scenes[scene_num].push_back(temp_command);
        if(temp_command->killSignal()) break;
    }
}

uint8_t data_handler::getNumScenes(){
    return num_scenes;
}
