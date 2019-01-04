#include "data_types.h"

char convertToASCII(char c){
     if(c<10)
     {
          c+=48;
     }
     else
     {
          c+=55;
     }
     return c;
}

std::string convertByteToHexString(uint8_t toConvert){
     int i=0;
     uint8_t letters[2];
     std::string s;

     for(i=0;i<2;i++)
     {
          letters[i]=toConvert&0xF;
          letters[i]=convertToASCII(letters[i]);
          toConvert=toConvert>>4;
     }
     for(i=1;i>=0;i--)
     {
          s+=letters[i];
     }
     return s;
}

std::string convert24BitToHexString(uint32_t toConvert){
     int i=0;
     uint8_t letters[6];
     std::string s;

     for(i=0;i<6;i++)
     {
          letters[i]=toConvert&0xF;
          letters[i]=convertToASCII(letters[i]);
          toConvert=toConvert>>4;
     }
     for(i=5;i>=0;i--)
     {
          s+=letters[i];
     }
     return s;
}

scroll_params & scroll_params::operator = (const scroll_params & s){
    num_splits = s.num_splits;
    ppu_ctrl_mod = s.ppu_ctrl_mod;
    sprite_0_Y = s.sprite_0_Y;
    X_coarse = s.X_coarse;
    Y_coarse = s.Y_coarse;
    Vx_fine = s.Vx_fine;
    Vx_coarse = s.Vx_coarse;
    Vy_fine = s.Vy_fine;
    Vy_coarse = s.Vy_coarse;
    for(int i=0;i<num_splits;i++){
        splits[i].X_coarse = s.splits[i].X_coarse;
        splits[i].split_height_fine = s.splits[i].split_height_fine;
        splits[i].split_height_coarse = s.splits[i].split_height_coarse;
        splits[i].Vx_fine = s.splits[i].Vx_fine;
        splits[i].Vx_coarse = s.splits[i].Vx_coarse;
    }
    return *this;
}

bool scroll_params::operator == (const scroll_params & s){
    if(!(num_splits == s.num_splits)) return false;
    if(!(ppu_ctrl_mod == s.ppu_ctrl_mod)) return false;
    if(!(sprite_0_Y == s.sprite_0_Y)) return false;
    if(!(X_coarse == s.X_coarse)) return false;
    if(!(Y_coarse == s.Y_coarse)) return false;
    if(!(Vx_fine == s.Vx_fine)) return false;
    if(!(Vx_coarse == s.Vx_coarse)) return false;
    if(!(Vy_fine == s.Vy_fine)) return false;
    if(!(Vy_coarse == s.Vy_coarse)) return false;
    for(int i=0;i<num_splits;i++){
        if(!(splits[i].X_coarse == s.splits[i].X_coarse)) return false;
        if(!(splits[i].split_height_fine == s.splits[i].split_height_fine)) return false;
        if(!(splits[i].split_height_coarse == s.splits[i].split_height_coarse)) return false;
        if(!(splits[i].Vx_fine == s.splits[i].Vx_fine)) return false;
        if(!(splits[i].Vx_coarse == s.splits[i].Vx_coarse)) return false;
    }
    return true;
}

scroll_params * getScrollParams(uint8_t * rom_data, uint8_t index){
    scroll_params * result = new scroll_params;
    uint32_t pointer = rom_data[scroll_setup_address + (index<<1)] + (rom_data[scroll_setup_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x10010;
    result->id = index;
    result->references = 0;
    result->num_splits = rom_data[pointer++];
    result->ppu_ctrl_mod = rom_data[pointer++];
    result->sprite_0_Y = rom_data[pointer++];
    result->X_coarse = rom_data[pointer++];
    result->Y_coarse = rom_data[pointer++];
    result->Vx_fine = rom_data[pointer++];
    result->Vx_coarse = rom_data[pointer++];
    result->Vy_fine = rom_data[pointer++];
    result->Vy_coarse = rom_data[pointer++];
    for(int i=0;i<result->num_splits;i++){
        result->splits[i].X_coarse = rom_data[pointer++];
        result->splits[i].split_height_fine = rom_data[pointer++];
        result->splits[i].split_height_coarse = rom_data[pointer++];
        result->splits[i].Vx_fine = rom_data[pointer++];
        result->splits[i].Vx_coarse = rom_data[pointer++];
    }
    for(int i=0;i<num_params;i++){
        if(*result == *scroll_params_list[i]){
            delete result;
            return scroll_params_list[i];
        }
    }
    if(num_params>=0x40){
        //ERROR HANDLER
    }
    scroll_params_list[num_params++] = result;
    return result;
}

ppu_string & ppu_string::operator = (const ppu_string & to_copy){
    address = to_copy.address;
    id = to_copy.id;
    s = to_copy.s;
    gfx_page = to_copy.gfx_page;
    image = to_copy.image;
    modified = to_copy.modified;
    references = to_copy.references;
    tiles = to_copy.tiles;
    return *this;
}

bool ppu_string::operator == (const ppu_string & to_check){
    if(s != to_check.s) return false;
    if(tiles.size() != to_check.tiles.size()) return false;
    for(unsigned int i=0;i<tiles.size();i++){
        if(tiles[i] != to_check.tiles[i]) return false;
    }
    return true;
}

ppu_string * getPPUString(uint8_t * rom_data,uint16_t address){
    int i;
    for(i=0;i<num_ppu_strings;i++){
        if(ppu_strings[i]->address == address){
            return ppu_strings[i];
        }
    }
    if(num_ppu_strings>=0x100){
        //ERROR HANDLER
    }
    ppu_string * result = new ppu_string;
    result->references = 0;
    result->id = num_ppu_strings;
    result->modified = false;
    result->address = address;
    result->s = "";
    //Don't set the arrangement or tile data until cmd_wait is reached, as bg_CHR_page is sometimes set after the ppu transmission
    ppu_strings[num_ppu_strings++] = result;
    return result;
}

sprite_linear & sprite_linear::operator = (const sprite_linear &s){
    Vx_fine = s.Vx_fine;
    Vx_coarse = s.Vx_coarse;
    Vy_fine = s.Vy_fine;
    Vy_coarse = s.Vy_coarse;
    return *this;
}

bool sprite_linear::operator ==(const sprite_linear & s){
    if(Vx_fine!=s.Vx_fine) return false;
    if(Vx_coarse!=s.Vx_coarse) return false;
    if(Vy_fine!=s.Vy_fine) return false;
    if(Vy_coarse!=s.Vy_coarse) return false;
    return true;
}

sprite_linear * getSpriteLinear(uint8_t * rom_data,uint8_t index){
    sprite_linear * result = new sprite_linear;
    result->Vx_fine = rom_data[sprite_linear_address + (index<<2)];
    result->Vx_coarse = rom_data[sprite_linear_address + 1 + (index<<2)];
    result->Vy_fine = rom_data[sprite_linear_address + 2 + (index<<2)];
    result->Vy_coarse = rom_data[sprite_linear_address + 3 + (index<<2)];
    result->id = index;
    result->references=0;
    for(int i=0;i<num_sprite_linears;i++){
        if(*result == *sprite_linears[i]){
            delete result;
            return sprite_linears[i];
        }
    }
    if(num_sprite_linears>=0x100){
        //ERROR HANDLER
    }
    sprite_linears[num_sprite_linears++] = result;
    return result;
}

earthquake & earthquake::operator = (const earthquake & e){
    ppu_ctrl = e.ppu_ctrl;
    x = e.x;
    y = e.y;
    num_steps = e.num_steps;
    timer = e.timer;

    return *this;
}

bool earthquake::operator ==(const earthquake & e){
    if(num_steps!=e.num_steps) return false;
    if(timer != e.timer) return false;
    if(x.length()<num_steps || e.x.length()<num_steps) return false;
    if(y.length()<num_steps || e.y.length()<num_steps) return false;
    if(ppu_ctrl.length()<num_steps || e.ppu_ctrl.length()<num_steps) return false;
    for(int i=0;i<num_steps;i++){
        if(x[i]!=e.x[i]) return false;
        if(y[i]!=e.y[i]) return false;
        if(ppu_ctrl[i]!=e.ppu_ctrl[i]) return false;
    }
    return true;
}

earthquake * getEarthquake(uint8_t * rom_data,uint8_t index){
    earthquake * result = new earthquake;
    result->id = index;
    result->references = 0;
    result->timer = rom_data[earthquake_timer_address + index];
    result->num_steps = 0;
    result->ppu_ctrl = "";
    result->x="";
    result->y="";
    uint32_t pointer = rom_data[earthquake_ptr_address + (index<<1)] + (rom_data[earthquake_ptr_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x10010;
    while(!(rom_data[pointer]&0x80)){
        result->ppu_ctrl+=rom_data[pointer++];
        result->x+=rom_data[pointer++];
        result->y+=rom_data[pointer++];
        result->num_steps++;
    }
    for(int i=0;i<num_earthquakes;i++){
        if(*result == *earthquakes[i]){
            delete result;
            return earthquakes[i];
        }
    }
    if(num_earthquakes>=0x100){
        //ERROR HANDLER
    }
    earthquakes[num_earthquakes++] = result;
    return result;
}

sprite_shake & sprite_shake::operator = (const sprite_shake & s){
    x=s.x;
    y=s.y;
    num_steps = s.num_steps;
    timer = s.timer;

    return *this;
}

bool sprite_shake::operator ==(const sprite_shake & s){
    if(num_steps!=s.num_steps) return false;
    if(timer != s.timer) return false;
    if(x.length()<num_steps || s.x.length()<num_steps) return false;
    if(y.length()<num_steps || s.y.length()<num_steps) return false;
    for(int i=0;i<num_steps;i++){
        if(x[i]!=s.x[i]) return false;
        if(y[i]!=s.y[i]) return false;
    }
    return true;
}

sprite_shake * getSpriteShake(uint8_t * rom_data, uint8_t index){
    sprite_shake * result = new sprite_shake;
    result->id = index;
    result->references=0;
    result->timer = rom_data[sprite_shake_timer_address + index];
    result->num_steps = 0;
    result->x = "";
    result->y = "";
    uint32_t pointer = rom_data[sprite_shake_address + (index<<1)] + (rom_data[sprite_shake_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x14010;
    while(!(rom_data[pointer]&0x80)){
        result->num_steps++;
        result->x += rom_data[pointer++];
        result->y += rom_data[pointer++];
    }
    for(int i=0;i<num_sprite_shakes;i++){
        if(*result == *sprite_shakes[i]){
            delete result;
            return sprite_shakes[i];
        }
    }
    if(num_sprite_shakes>=0x100){
        //ERROR HANDLER
    }
    sprite_shakes[num_sprite_shakes++] = result;
    return result;
}

sprite_accel & sprite_accel::operator = (const sprite_accel &a){
    Vx_fine = a.Vx_fine;
    Vx_coarse = a.Vx_coarse;
    Vy_fine = a.Vy_fine;
    Vy_coarse = a.Vy_coarse;
    Ax = a.Ax;
    Ay = a.Ay;
    return *this;
}

bool sprite_accel::operator ==(const sprite_accel & a){
    if(Vx_fine!=a.Vx_fine) return false;
    if(Vx_coarse!=a.Vx_coarse) return false;
    if(Vy_fine!=a.Vy_fine) return false;
    if(Vy_coarse!=a.Vy_coarse) return false;
    if(Ax!=a.Ax) return false;
    if(Ay!=a.Ay) return false;
    return true;
}

sprite_accel * getSpriteAccel(uint8_t * rom_data,uint8_t index){
    sprite_accel * result = new sprite_accel;
    result->Vx_fine = rom_data[sprite_accel_address + (index<<2) + (index<<1)];
    result->Vx_coarse = rom_data[sprite_accel_address + 1 + (index<<2) + (index<<1)];
    result->Vy_fine = rom_data[sprite_accel_address + 2 + (index<<2) + (index<<1)];
    result->Vy_coarse = rom_data[sprite_accel_address + 3 + (index<<2) + (index<<1)];
    result->Ax = rom_data[sprite_accel_address + 4 + (index<<2) + (index<<1)];
    result->Ay = rom_data[sprite_accel_address + 5 + (index<<2) + (index<<1)];
    result->id = index;
    result->references=0;
    for(int i=0;i<num_sprite_accels;i++){
        if(*result == *sprite_accels[i]){
            delete result;
            return sprite_accels[i];
        }
    }
    if(num_sprite_accels>=0x100){
        //ERROR HANDLER
    }
    sprite_accels[num_sprite_accels++] = result;
    return result;
}

QColor getColor(uint8_t palette_num){
    palette_num&=0x3F;
    const uint8_t red = NES_COLORS_RGB[(palette_num<<1)+palette_num];
    const uint8_t green = NES_COLORS_RGB[(palette_num<<1)+palette_num+1];
    const uint8_t blue = NES_COLORS_RGB[(palette_num<<1)+palette_num+2];
    return QColor(red,green,blue);
}

NESpalette & NESpalette::operator = (const NESpalette & pal){
    for(int i=1;i<4;i++){
        nes_colors[i] = pal.nes_colors[i];
        p[i] = pal.p[i];
    }
    return *this;
}

bool NESpalette::operator == (const NESpalette & pal){
    for(int i=1;i<4;i++){
        if(nes_colors[i] != pal.nes_colors[i]) return false;    //only check NES colors, not RGB values
        //if(p[i] != pal.p[i]) return false;
    }
    return true;
}

palette_set & palette_set::operator = (const palette_set & pal){
    ubg_color = pal.ubg_color;
    nes_ubg = pal.nes_ubg;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            p[i].nes_colors[j] = pal.p[i].nes_colors[j];
            p[i].p[j] = pal.p[i].p[j];
        }
    }
    return *this;
}

bool palette_set::operator == (const palette_set & pal){
    if(nes_ubg != pal.nes_ubg) return false;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(!j) continue;    //Don't compare the universal background color again.
            if(p[i].nes_colors[j] != pal.p[i].nes_colors[j]) return false;  //Only compare NES colors, not RGB values
        }
    }
    return true;
}

uint8_t palette_set::duplicates(){
    uint8_t num_duplicates=0;
    uint16_t duplicate_bits=0;
    uint8_t linear_palettes[16];
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(!j){
                linear_palettes[(i<<2)] = nes_ubg;
            }
            else{
                linear_palettes[(i<<2) + j] = p[i].nes_colors[j];
            }

        }
    }
    for(int i=0;i<16;i++){
        if((duplicate_bits>>i)&1) continue;
        for(int j=i+1;j<16;j++){
            if((duplicate_bits>>j)&1) continue;
            if(linear_palettes[i] == linear_palettes[j]){
                num_duplicates++;
                duplicate_bits|= (1<<j);
            }
        }
    }
    return num_duplicates;
}

NESpalette * getPalette(uint8_t * rom_data, uint8_t index){
    NESpalette * result = new NESpalette;
    result->references = 0;
    result->id = index;
    uint32_t pointer;
    if(index&0x80){
        pointer = rom_data[palette_ptr_address + 2] + (rom_data[palette_ptr_address + 3]<<8);
        index&=0x7f;
    }
    else{
        pointer = rom_data[palette_ptr_address] + (rom_data[palette_ptr_address + 1]<<8);
    }
    pointer = (pointer&0x3fff) + 0x10010;
    for(int i=1;i<4;i++){
        result->nes_colors[i] = rom_data[pointer + ((3*index)&0xff) + i - 1];
    }
    for(int i=0;i<num_palettes;i++){
        if(*result == *palettes[i]){
            delete result;
            return palettes[i];
        }
    }
    if(num_palettes>=0x100){
        //ERROR HANDLER
    }
    palettes[num_palettes++] = result;
    return result;
}

dialog_string * getText(uint8_t * rom_data, uint8_t index){
    for(unsigned int i=0;i<num_dialogs;i++){
        if(all_dialog[i]->id == index) return all_dialog[i];
    }
    dialog_string * result = new dialog_string;
    if(!result){
        return NULL;
    }
    result->id = index;
    result->references = 0;
    result->text = "";
    uint8_t temp;
    uint32_t pointer = rom_data[text_ptr_address + (index<<1)] + (rom_data[text_ptr_address + 1 + (index<<1)]<<8);
    if(pointer&0x8000){
        pointer = (pointer&0x3fff) + 0x14010;
        while(rom_data[pointer]!=0xF8){
            temp = rom_data[pointer++];
            result->text += temp;
            switch(temp){
            case 0xFD:  //ANIMATESPRITE
                result->data.push_back(getSpriteAnimation(rom_data,rom_data[pointer]&0x3F));
                result->is_animation.push_back(true);
                result->text += rom_data[pointer++];
                break;
            case 0xFE:  //SPRITESWAP
                result->data.push_back(getSprite(rom_data,rom_data[pointer]&0x3F));
                result->is_animation.push_back(false);
                result->text += rom_data[pointer++];
                break;
            case 0xF9:  //SET_INDENT
                result->text += rom_data[pointer++];
                break;
            case 0xF6:  //drawBelow, Deadpool specific command
            case 0xF7:  //drawAbove, Deadpool specific command
                result->text += rom_data[pointer++];
                result->text += rom_data[pointer++];
                break;
            default:
                break;
            }
        }
        result->text+= 0xF8;
    }

    if(num_dialogs>=0x100){
        //ERROR HANDLER
    }
    all_dialog[num_dialogs++] = result;
    return result;
}

ppu_base_type * getPPUBase(uint8_t * rom_data, uint8_t index){
    ppu_base_type * result = new ppu_base_type;
    result->id = index;
    result->references = 0;
    result->a = rom_data[ppu_base_address + (index<<1)] + (rom_data[ppu_base_address + 1 + (index<<1)]<<8);
    for(unsigned int i=0;i<num_ppu_bases;i++){
        if(result->a == ppu_bases[i]->a){
            delete result;
            return ppu_bases[i];
        }
    }
    if(num_ppu_bases>=0x100){
        //ERROR HANDLER
    }
    ppu_bases[num_ppu_bases++] = result;
    return result;
}

bg_arrangement & bg_arrangement::operator = (const bg_arrangement & b){
    arrangement = b.arrangement;
    gfx_page = b.gfx_page;
    tileset = b.tileset;
    metatilesX = b.metatilesX;
    metatilesY = b.metatilesY;
    attrib_bits = b.attrib_bits;
    id = b.id;
    references = b.references;
    image = b.image;
    delete [] metatiles;
    metatiles = new metatile[b.tiles.size()>>3];
    for(unsigned int i=0;i<(b.tiles.size()>>3);i++){
        metatiles[i] = b.metatiles[i];
    }
    modified = b.modified;
    tiles = b.tiles;
    pals = b.pals;
    return *this;
}

bool bg_arrangement::operator == (const bg_arrangement & b){
    if(arrangement != b.arrangement) return false;
    if(gfx_page != b.gfx_page) return false;
    if(tileset != b.tileset) return false;
    if(metatilesX != b.metatilesX) return false;
    if(metatilesY != b.metatilesY) return false;
    return true;
}

bg_arrangement * getBGArrangement(uint8_t * rom_data, uint8_t index){
    for(unsigned int i=0;i<num_bgs;i++){
        if(index == bgs[i]->id){
            return bgs[i];
        }
    }
    bg_arrangement * result = new bg_arrangement;
    result->id = index;
    result->references = 0;
    result->pals.clear();
    result->modified = false;
    uint32_t pointer = rom_data[bg_arrangement_address + (index<<1)] + (rom_data[bg_arrangement_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x18010;
    result->arrangement = "";
    uint8_t temp = rom_data[pointer++];
    result->gfx_page = temp&0x1F;
    result->tileset = (temp>>5)&0x7;
    temp = rom_data[pointer++];
    result->metatilesX = (temp>>4)&0xf;
    result->metatilesY = temp&0xf;
    result->attrib_bits = "";
    uint32_t tileset_ptr,attrib_ptr;
    tileset_ptr = rom_data[bg_tileset_address + (result->tileset<<1)] + (rom_data[bg_tileset_address + 1 + (result->tileset<<1)]<<8);
    tileset_ptr = (tileset_ptr&0x3FFF) + 0x18010;
    attrib_ptr = rom_data[bg_attrib_address + (result->tileset<<1)] + (rom_data[bg_attrib_address + 1 + (result->tileset<<1)]<<8);
    attrib_ptr = (attrib_ptr&0x3FFF) + 0x18010;
    uint8_t temp_attrib,temp_prev_attrib = 0;
    unsigned int i;
    uint16_t arrangement_size = ((result->metatilesX) * (result->metatilesY));
    for(i=0;i<arrangement_size;i++){
        uint32_t metatile_offset = tileset_ptr + (rom_data[pointer]<<3);
        for(unsigned int j=0;j<8;j++){
            result->tiles.push_back(&(CHR_pages[result->gfx_page].t[rom_data[metatile_offset + j]]));
            CHR_pages[result->gfx_page].bg_used[rom_data[metatile_offset + j]] = true;
        }
        temp_attrib = rom_data[attrib_ptr + (rom_data[pointer]>>1)];
        if(!(rom_data[pointer]&1)) temp_attrib = (temp_attrib >> 4) &0xF;
        temp_attrib&=0xF;
        if(i&1){
            result->attrib_bits += temp_attrib | (temp_prev_attrib<<4);
        }
        else{
            temp_prev_attrib = temp_attrib;
        }
        result->arrangement += rom_data[pointer++];
    }
    if(i&1) result->attrib_bits += temp_prev_attrib<<4;
    result->metatiles = new metatile[arrangement_size];
    if(num_bgs>=0x100){
        //ERROR HANDLER
    }
    bgs[num_bgs++] = result;
    return result;
}

sprite & sprite::operator = (const sprite &s){
    gfx_page = s.gfx_page;
    arrangement = s.arrangement;
    bg_gfx_page = s.bg_gfx_page;
    id = s.id;
    references = s.references;
    image = s.image;
    modified = s.modified;
    pals = s.pals;
    tiles = s.tiles;
    return *this;
}

bool sprite::operator == (const sprite & s){
    if(gfx_page != s.gfx_page) return false;
    if(arrangement != s.arrangement) return false;
    return true;
}

sprite * getSprite(uint8_t * rom_data,uint8_t index){
    unsigned int i;
    for(i=0;i<num_sprites;i++){
        if(sprites[i]->id == index){
            return sprites[i];
        }
    }
    if(num_sprites>=0x80){
        //ERROR HANDLER
    }
    sprite * result = new sprite;
    result->id = index;
    result->references = 0;
    uint32_t pointer = rom_data[sprite_ptr_address + (index<<1)] + (rom_data[sprite_ptr_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x14010;
    result->gfx_page = rom_data[pointer++];
    result->arrangement = "";
    result->modified = false;
    i=0;
    while(!(rom_data[pointer]&0x80)){
        unsigned int j;
        result->arrangement += rom_data[pointer++];     //column X position
        result->arrangement += rom_data[pointer];       //Num tiles in column
        j= rom_data[pointer++];
        for(i=0;i<j;i++){
            uint8_t tile_value = rom_data[pointer++];
            result->arrangement += tile_value; //Tile value
            if(tile_value&1){
                result->tiles.push_back(&(CHR_pages[result->gfx_page].t[tile_value&0xFE]));
                CHR_pages[result->gfx_page].sprite_used[tile_value&0xFE] = true;
                result->tiles.push_back(&(CHR_pages[result->gfx_page].t[tile_value|0x01]));
                CHR_pages[result->gfx_page].sprite_used[tile_value|0x01] = true;
            }
            else{
                result->tiles.push_back(&(CHR_pages[bg_CHR_page].t[tile_value&0xFE]));
                CHR_pages[bg_CHR_page].sprite_used[tile_value&0xFE] = true;
                result->tiles.push_back(&(CHR_pages[bg_CHR_page].t[tile_value|0x01]));
                CHR_pages[bg_CHR_page].sprite_used[tile_value|0x01] = true;
            }
            result->arrangement += rom_data[pointer++]; //Y position and attribs
        }
    }

    sprites[num_sprites++] = result;
    return result;
}

sprite_animation * getSpriteAnimation(uint8_t * rom_data,uint8_t index){
    unsigned int i=0;
    /*for(i=0;i<num_sprite_animations;i++){
        if(sprite_animations[i]->id == index) return sprite_animations[i];
    }*/
    sprite_animation * result = new sprite_animation;
    result->id = index;
    result->references = 0;
    uint32_t pointer = rom_data[animated_sprite_ptr_address + (index<<1)] + (rom_data[animated_sprite_ptr_address + 1 + (index<<1)]<<8);
    pointer = (pointer&0x3fff) + 0x14010;
    result->text_engine = false;
    result->timer = rom_data[sprite_timer_address + index];
    while(!(rom_data[pointer + i]&0x80)) i++;
    result->sequence = new sprite * [i];
    result->num_steps = i;
    for(i=0;i<result->num_steps;i++){
        result->sequence[i] = getSprite(rom_data,rom_data[pointer + i]);
    }
    for(i=0;i<num_sprite_animations;i++){
        if(*result == *sprite_animations[i]){
            //delete result;
            return sprite_animations[i];
        }
    }
    if(num_sprite_animations>=0x80){
        //ERROR HANDLER
    }
    sprite_animations[num_sprite_animations++] = result;
    return result;
}

bool sprite_animation::operator == (const sprite_animation & a){
    if(timer != a.timer) return false;
    if(num_steps != a.num_steps) return false;
    for(unsigned int i=0;i<num_steps;i++){
        if(sequence[i] != a.sequence[i]) return false;
    }
    return true;
}

sprite_animation &sprite_animation::operator = (const sprite_animation &a){
    timer = a.timer;
    num_steps = a.num_steps;
    sequence = new sprite * [num_steps];
    id = a.id;
    references = a.references;
    text_engine = a.text_engine;
    for(unsigned int i=0;i<num_steps;i++){
        sequence[i] = a.sequence[i];
    }
    return *this;
}

std::string sprite::write(){
    std::string result = "";
    result += gfx_page;
    unsigned int i,j,k;
    uint8_t tiles_in_column;
    k=0;
    uint8_t tile_value;
    for(i=0;i<arrangement.length();){
        result += arrangement[i++];         //Column X position
        tiles_in_column = arrangement[i];
        result += arrangement[i++];         //Tiles in column
        for(j=0;j<tiles_in_column;j++){
            tile_value = this->tiles[k]->id;
            k+=2;
            tile_value &=0xFE;
            tile_value |= ((arrangement[i++])&1);   //Tile value
            result += tile_value;
            result += arrangement[i++];             //Attrib bits and Y position of tile
        }
    }
    result += 0x80;
    return result;
}

std::string sprite_accel::write(){
    std::string result = "";
    result += Vx_fine;
    result += Vx_coarse;
    result += Vy_fine;
    result += Vy_coarse;
    result += Ax;
    result += Ay;
    return result;
}

std::string sprite_animation::write(){
    std::string result = "";
    for(int i=0;i<num_steps;i++){
        result += sequence[i]->id;
    }
    result += 0xff;
    return result;
}

std::string sprite_linear::write(){
    std::string result = "";
    result += Vx_fine;
    result += Vx_coarse;
    result += Vy_fine;
    result += Vy_coarse;
    return result;
}

std::string sprite_shake::write(){
    std::string result = "";
    for(int i=0;i<num_steps;i++){
        result += x[i];
        result += y[i];
    }
    result += 0xff;
    return result;
}

std::string earthquake::write(){
    std::string result = "";
    for(int i=0;i<num_steps;i++){
        result += ppu_ctrl[i];
        result += x[i];
        result += y[i];
    }
    result += 0xff;
    return result;
}
/*
 * typedef struct scroll_splits{
    uint8_t X_coarse;
    uint8_t split_height_fine;
    uint8_t split_height_coarse;
    uint8_t Vx_fine;
    uint8_t Vx_coarse;
} scroll_splits;


 *
 * */
std::string scroll_params::write(){
    std::string result = "";
    result += num_splits;
    result += ppu_ctrl_mod;
    result += sprite_0_Y;
    result += X_coarse;
    result += Y_coarse;
    result += Vx_fine;
    result += Vx_coarse;
    result += Vy_fine;
    result += Vy_coarse;
    for(unsigned int i=0;i<num_splits;i++){
        result += splits[i].X_coarse;
        result += splits[i].split_height_fine;
        result += splits[i].split_height_coarse;
        result += splits[i].Vx_fine;
        result += splits[i].Vx_coarse;
    }
    return result;
}

std::string NESpalette::write(){
    std::string result = "";
    result += nes_colors[1];
    result += nes_colors[2];
    result += nes_colors[3];
    return result;
}

std::string bg_arrangement::write(){
    std::string result = "";
    uint8_t temp = gfx_page;
    temp |= (tileset<<5);
    result += temp;
    temp = metatilesX<<4;
    temp |= metatilesY;
    result += temp;
    uint16_t arrangement_size = metatilesX * metatilesY;
    //********INSERT CODE TO ADD METATILE ARRANGEMENT HERE.  DON'T JUST USE THE DEFAULT ARRANGEMENT
    for(unsigned int i=0;i<arrangement_size;i++){
        result += metatiles[i].id;
    }
    //result += arrangement;
    return result;
}

std::string ppu_string::write(){
    std::string result = "";
    unsigned int i,j,k;
    uint8_t tile_count;
    k=0;
    for(i=0;i<s.length();){
        tile_count = s[i]&0x7F;
        if(!tile_count) break;
        result += s[i++];
        result += s[i++];
        result += s[i++];
        for(j=0;j<tile_count;j++){
            result += tiles[k++]->id;
            i++;
        }
    }
    result += (char)0;
    return result;
}

std::string ppu_base_type::write(){
    std::string result = "";
    result += a&0xff;
    result += (a>>8)&0xff;
    return result;
}

std::string dialog_string::write(){
    unsigned int i,j;
    j=0;
    std::string result = "";
    for(i=0;i<text.length();i++){
        result += text[i];
        switch((uint8_t)(text[i])){
        case 0xFE:  //spriteswap
            i++;
            result += (text[i]&0xC0) | data.at(j++)->id;
            break;
        case 0xFD:  //animatesprite
            i++;
            result += (text[i]&0xC0) | data.at(j++)->id;
            break;
        case 0xF9:  //setindent
            i++;
            result += text[i];
            break;
        default:
            break;
        }
    }
    return result;
}

NEStile & NEStile::operator = (const NEStile & to_copy){
    for(unsigned int i=0;i<0x10;i++){
        t[i] = to_copy.t[i];
    }
    checksum = to_copy.checksum;

    return *this;
}

bool NEStile::operator == (const NEStile & to_compare){
    if(checksum != to_compare.checksum) return false;
    for(unsigned int i=0;i<0x10;i++){
        if(t[i] != to_compare.t[i]) return false;
    }
    return true;
}

std::string metatile::write(){
    std::string result = "";
    for(unsigned int i=0;i<8;i++){
        result += (char)(tiles[i]->id);
    }
    return result;
}

metatile & metatile::operator = (const metatile &m){
    checksum = m.checksum;
    attrib_bits = m.attrib_bits;
    for(unsigned int i=0;i<8;i++){
        tiles[i] = m.tiles[i];
    }

    return *this;
}

bool metatile::operator == (const metatile & m){
    if(checksum != m.checksum) return false;
    if(attrib_bits != m.attrib_bits) return false;
    for(unsigned int i=0;i<8;i++){
        if(m.tiles[i]->id != tiles[i]->id) return false;
    }
    return true;
}

uint16_t metatile_set::add(metatile m){
    if(num_metatiles>=0x100) return 0xffff;
    unsigned int i;
    for(i=0;i<num_metatiles;i++){
        if(set[i] == m) return i;
    }
    set[num_metatiles++] = m;
    return i;
}

void NEStile::hflip(){
    unsigned int i;
    uint8_t temp_array[0x10];
    for(i=0;i<0x10;i++){
        temp_array[i] = t[i];
        t[i] = 0;
    }
    for(i=0;i<0x10;i++){
        t[i] |= (temp_array[i] & 0x01)<<7;
        t[i] |= (temp_array[i] & 0x02)<<5;
        t[i] |= (temp_array[i] & 0x04)<<3;
        t[i] |= (temp_array[i] & 0x08)<<1;
        t[i] |= (temp_array[i] & 0x10)>>1;
        t[i] |= (temp_array[i] & 0x20)>>3;
        t[i] |= (temp_array[i] & 0x40)>>5;
        t[i] |= (temp_array[i] & 0x80)>>7;
    }
    checksum = 0;
    for(i=0;i<0x10;i++) checksum+=t[i];
}

void NEStile::vflip(){
    unsigned int i;
    uint8_t temp_array[0x10];
    for(i=0;i<0x10;i++){
        temp_array[i] = t[i];
        t[i] = 0;
    }
    for(i=0;i<8;i++){
        t[i] = temp_array[7 - i];
        t[8+i] = temp_array[15-i];
    }
    //Checksum won't change for a vlip
}

void NEStile::hvflip(){
    unsigned int i;
    uint8_t temp_array[0x10];
    for(i=0;i<0x10;i++){
        temp_array[i] = t[i];
        t[i] = 0;
    }
    for(i=0;i<0x10;i++){
        t[i] |= (temp_array[0x0F - i] & 0x01)<<7;
        t[i] |= (temp_array[0x0F - i] & 0x02)<<5;
        t[i] |= (temp_array[0x0F - i] & 0x04)<<3;
        t[i] |= (temp_array[0x0F - i] & 0x08)<<1;
        t[i] |= (temp_array[0x0F - i] & 0x10)>>1;
        t[i] |= (temp_array[0x0F - i] & 0x20)>>3;
        t[i] |= (temp_array[0x0F - i] & 0x40)>>5;
        t[i] |= (temp_array[0x0F - i] & 0x80)>>7;
    }
    for(i=0;i<0x10;i++) checksum+=t[i];
}

palette_set bg_arrangement::bestPalette(){
    uint16_t result = 0;
    unsigned int i,j;
    uint8_t least_duplicates = 0xff;
    for(i=0;i<pals.size();i++){
        j = pals.at(i).duplicates();
        if(j < least_duplicates){
            least_duplicates = j;
            result = i;
        }
    }
    return pals.at(result);
}

palette_set sprite::bestPalette(){
    uint16_t result = 0;
    unsigned int i,j;
    uint8_t least_duplicates = 0xff;
    for(i=0;i<pals.size();i++){
        j = pals.at(i).duplicates();
        if(j < least_duplicates){
            least_duplicates = j;
            result = i;
        }
    }
    return pals.at(result);
}

void palette_set::convertColors(){
    unsigned int i,j;
    ubg_color = getColor(nes_ubg);
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            p[i].p[j] = (j)?getColor(p[i].nes_colors[j]):ubg_color;
        }
    }
}

bg_arrangement::bg_arrangement(){
    metatiles = NULL;
}

bg_arrangement::~bg_arrangement(){
    delete [] metatiles;
    metatiles = NULL;
}

sprite_animation::sprite_animation(){
    sequence = NULL;
}

sprite_animation::~sprite_animation(){
    delete [] sequence;
    sequence = NULL;
}

std::string getLine(const std::string & s,uint32_t & offset){
    std::string result = "";

    while((offset< s.length()) && !(s[offset] == '\n' || s[offset] == '\r')){
        if(isValidText(s[offset])) result += s[offset];
        offset++;
    }
    if(offset>=s.length()) return result;
    while((offset < s.length()) && !isValidText(s[offset])) offset++;
    line_number++;
    return result;
}

bool isValidText(uint8_t to_test){
    if(to_test<0x20) return false;
    if(to_test<0x7F) return true;
    return false;
}

bool isWhitespace(uint8_t to_test){
    if(to_test == ' ') return true;
    if(to_test == '\t') return true;
    if(to_test == '\n') return true;
    return false;
}

void skipWhitespace(const std::string & s,uint32_t & offset){
    while(offset < s.length() && isWhitespace(s[offset])) offset++;
}

int8_t convertToHex(char c){
    if(c>=48 && c<58) return (c-48);
    if(c>=65 && c<71) return (c-55);
    if(c>=97 && c<103) return (c-87);
    return -1;
}

int32_t getIntFromTXT(const std::string & s,uint32_t & offset){
    while(offset<s.length()){
        if(s[offset] >= 0x30 && s[offset] < 0x3A) break;
        if(s[offset] >= 0x41 && s[offset] < 0x47) break;
        offset++;
    }
    if(offset>=s.length()) return -1;
    uint32_t result=0;
    uint8_t digit = 0;
    int8_t temp;
    while(offset < s.length() && digit < 8){
        temp = convertToHex(s[offset]);
        if(temp<0) break;
        result<<=4;
        result|=temp;
        offset++;
    }
    return result;
}

std::string getWordFromTXT(const std::string & s,uint32_t & offset){
    std::string result = "";
    while(offset < s.length()){
        if(s[offset] >= 0x41 && s[offset] < 0x5B) break;
        if(s[offset] >= 0x61 && s[offset] < 0x7B) break;
        offset++;
    }
    if(offset >= s.length()) return result;
    while(offset < s.length()){
        if(s[offset] >= 0x41 && s[offset] < 0x5B){
            result += s[offset++];
            continue;
        }
        if(s[offset] >= 0x61 && s[offset] < 0x7B){
            result += s[offset++];
            continue;
        }
        break;
    }
    return result;
}

bool scroll_params::speedEquals(const scroll_params & s){
    if(num_splits != s.num_splits) return false;
    if(Vx_fine != s.Vx_fine) return false;
    if(Vx_coarse != s.Vx_coarse) return false;
    if(Vy_fine != s.Vy_fine) return false;
    if(Vy_coarse != s.Vy_coarse) return false;
    for(unsigned int i=0;i<num_splits;i++){
        if(splits[i].Vx_fine != s.splits[i].Vx_fine) return false;
        if(splits[i].Vx_coarse != s.splits[i].Vx_coarse) return false;
    }
    return true;
}

bool dialog_string::operator == (const dialog_string & s){
    if(text != s.text) return false;
    unsigned int i=0;
    for(i=0;i<data.size();i++){
        if(data.at(i) != s.data.at(i)) return false;
        if(is_animation.at(i) != s.is_animation.at(i)) return false;
    }
    return true;

}

void lineUpper(std::string & s){
    for(unsigned int i=0;i<s.length();i++) s[i] = toupper(s[i]);
}

CHR_page & CHR_page::operator = (const CHR_page & to_copy){
    unsigned int i=0;
    address = to_copy.address;
    id = to_copy.id;
    for(i=0;i<0x100;i++){
        t[i] = to_copy.t[i];
        t[i].shared = to_copy.t[i].shared;
        bg_used[i] = to_copy.bg_used[i];
        sprite_used[i] = to_copy.sprite_used[i];
    }
    return *this;
}

void bg_arrangement::generateImage(){
    uint16_t pixel_width = (metatilesX)<<5;
    uint16_t pixel_height = (metatilesY)<<4;
    image = QImage(pixel_width,pixel_height,QImage::Format_ARGB32);
    unsigned int i,j,k;
    uint8_t color_index;
    QRgb * edit_line;
    palette_set chosen_palettes = bestPalette();
    chosen_palettes.convertColors();

    for(i=0;i<tiles.size();i++){
        //8 tiles per metatile, 4 on top row, 4 on bottom row.
        //base_row = metatile_num/metatilesX * 0x10		(0x10 rows per metatile)
        //if tile_num&4, add 8 to base_row
        uint16_t metatile_num = i>>3;               //metatile_num = tile_num/8
        uint16_t base_row = (metatile_num/metatilesX) <<4;
        if(i&4) base_row +=8;
        uint16_t base_column = (metatile_num%metatilesX) <<5;
        base_column += (i&3)<<3;
        uint8_t attribs = attrib_bits[metatile_num>>1];
        if(!(metatile_num&1)) attribs >>= 4;
        if(!(i&2)) attribs >>= 2;
        attribs&=0x3;
        for(j=0;j<8;j++){
            edit_line = (QRgb*)image.scanLine(base_row + j);
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= ((tiles[i]->t[j]) >> (7-k))&0x01;
                color_index |= (((tiles[i]->t[j+8]) >> (7-k))&0x01)<<1;
                edit_line[base_column + k] = chosen_palettes.p[attribs].p[color_index].rgb();
            }
        }
    }
}

void sprite::generateImage(){
    typedef struct sprite_tile{
        NEStile * tile;
        uint8_t x;
        uint8_t y;
        bool hflip;
        uint8_t palette;
    }sprite_tile;
    if(!tiles.size()) return;
    sprite_tile * to_draw = new sprite_tile[tiles.size()];
    palette_set chosen_palettes = bestPalette();
    chosen_palettes.convertColors();
    uint16_t current_x;
    int i,j,k;
    uint8_t tiles_in_column;
    int16_t tile_count = 0;
    for(i=0;i<arrangement.length();){
        if(arrangement[i]&0x80) break;
        current_x = spritexy_values[((uint8_t)(arrangement[i++]))];
        tiles_in_column = arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            i++;    //skip over tile value in the arrangement.  Actual pointers take precedence
            to_draw[tile_count].tile = tiles[tile_count];
            to_draw[tile_count+1].tile = tiles[tile_count+1];
            to_draw[tile_count].x = current_x;
            to_draw[tile_count+1].x = current_x;
            to_draw[tile_count].y = spritexy_values[((((uint8_t)(arrangement[i]))>>3)&0x1F)];
            to_draw[tile_count+1].y = to_draw[tile_count].y + 8;
            to_draw[tile_count].palette = ((arrangement[i])>>1)&0x03;
            to_draw[tile_count+1].palette = to_draw[tile_count].palette;
            to_draw[tile_count].hflip = (arrangement[i++])&0x01; //At this point we advance to next byte in arrangement
            to_draw[tile_count+1].hflip = to_draw[tile_count].hflip;
            tile_count+=2;
        }
    }
    int8_t min_x,min_y,max_x,max_y;
    min_x = to_draw[0].x;
    min_y = to_draw[0].y;
    max_x = to_draw[0].x;
    max_y = to_draw[0].y;

    for(i=1;i<tile_count;i++){
        if(((int8_t)to_draw[i].x) > max_x) max_x = to_draw[i].x;
        if(((int8_t)to_draw[i].x) < min_x) min_x = to_draw[i].x;
        if(((int8_t)to_draw[i].y) > max_y) max_y = to_draw[i].y;
        if(((int8_t)to_draw[i].y) < min_y) min_y = to_draw[i].y;
    }
    min_x = 0x100-min_x;
    min_y = 0x100-min_y;
    max_x += min_x + 8;
    max_y += min_y + 8;
    for(i=0;i<tile_count;i++){
        to_draw[i].x += min_x;
        to_draw[i].y += min_y;
    }
    image = QImage((uint8_t)max_x,(uint8_t)max_y,QImage::Format_ARGB32);
    QColor green(0,0xff,0);
    uint8_t color_index;
    QRgb * edit_line;
    for(i=0;i<(uint8_t)max_y;i++){
        edit_line = (QRgb*)image.scanLine(i);
        for(j=0;j<(uint8_t)max_x;j++){
            edit_line[j] = green.rgb();
        }
    }
    for(i=tile_count-1;i>=0;i--){
        for(j=0;j<8;j++){
            edit_line = (QRgb*)image.scanLine(to_draw[i].y + j);
            if(to_draw[i].hflip){
                for(k=0;k<8;k++){
                    color_index = 0;
                    color_index |= ((tiles[i]->t[j]) >> (7-k))&0x01;
                    color_index |= (((tiles[i]->t[j+8]) >> (7-k))&0x01)<<1;
                    if(!color_index){
                        if(edit_line[to_draw[i].x + 7 - k] != green.rgb()) continue;
                    }
                    edit_line[to_draw[i].x + 7 - k] = chosen_palettes.p[to_draw[i].palette].p[color_index].rgb();
                }
            }
            else{
                for(k=0;k<8;k++){
                    color_index = 0;
                    color_index |= ((tiles[i]->t[j]) >> (7-k))&0x01;
                    color_index |= (((tiles[i]->t[j+8]) >> (7-k))&0x01)<<1;
                    if(!color_index){
                        if(edit_line[to_draw[i].x + k] != green.rgb()) continue;
                    }
                    edit_line[to_draw[i].x + k] = chosen_palettes.p[to_draw[i].palette].p[color_index].rgb();
                }
            }
        }
    }
    delete [] to_draw;
}

void ppu_string::generateImage(){
    typedef struct string_tile{
        NEStile * tile;
        uint8_t x;
        uint8_t y;
    }string_tile;
    if(!tiles.size()) return;
    string_tile * to_draw = new string_tile[tiles.size()];
    //palette_set chosen_palettes = bestPalette();
    //chosen_palettes.convertColors();
    unsigned int i,j,k;
    unsigned int tile_count = 0;
    unsigned int tiles_in_section;
    uint8_t current_x;
    uint8_t current_y;
    bool vertical;
    for(i=0;i<s.size();){
        if(!s[i]) break;
        vertical = s[i]&0x80;
        tiles_in_section = s[i++]&0x7F;
        uint16_t address = s[i++];
        address += (s[i++])<<8;
        current_x = (address&0x1F)<<3;
        current_y = (address>>5)<<3;
        for(j=0;j<tiles_in_section;j++){
            i++;
            to_draw[tile_count].tile = tiles[tile_count];
            to_draw[tile_count].x = current_x;
            to_draw[tile_count].y = current_y;
            tile_count++;
            if(vertical){
                current_y+=8;
            }
            else{
                current_x+=8;
            }
        }
    }
    uint8_t max_x=0;
    uint8_t max_y=0;
    for(i=0;i<tile_count;i++){
        if(to_draw[i].x > max_x) max_x = to_draw[i].x;
        if(to_draw[i].y > max_y) max_y = to_draw[i].y;
    }
    max_x+=8;
    max_y+=8;
    image = QImage(max_x,max_y,QImage::Format_ARGB32);
    QColor green(0,0xff,0);
    uint8_t color_index;
    QRgb * edit_line;
    NESpalette pal;
    for(i=0;i<4;i++){
        pal.p[i] = QColor(i<<6,i<<6,i<<6);      //Create greyscale palette
    }
    for(i=0;i<max_y;i++){
        edit_line = (QRgb*)image.scanLine(i);
        for(j=0;j<max_x;j++){
            edit_line[j] = green.rgb();
        }
    }
    for(i=0;i<tile_count;i++){
        for(j=0;j<8;j++){
            edit_line = (QRgb*)image.scanLine(to_draw[i].y + j);
            for(k=0;k<8;k++){
                color_index = 0;
                color_index |= ((tiles[i]->t[j]) >> (7-k))&0x01;
                color_index |= (((tiles[i]->t[j+8]) >> (7-k))&0x01)<<1;
                edit_line[to_draw[i].x + k] = pal.p[color_index].rgb();
            }
        }
    }
    delete [] to_draw;
}

uint32_t NESpalette::colorDiff(QRgb col){
    unsigned int i;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    for(i=0;i<4;i++){
        if(p[i].red() >= qRed(col)){
            total_diff = p[i].red() - qRed(col);
        }
        else{
            total_diff = qRed(col) - p[i].red();
        }
        if(p[i].green() >= qGreen(col)){
            total_diff += p[i].green() - qGreen(col);
        }
        else{
            total_diff += qGreen(col) - p[i].green();
        }
        if(p[i].blue() >= qBlue(col)){
            total_diff += p[i].blue() - qBlue(col);
        }
        else{
            total_diff += qBlue(col) - p[i].blue();
        }
        if(total_diff < min_diff){
            min_diff = total_diff;
        }
    }
    return min_diff;
}

uint32_t NESpalette::colorDiffSprite(QRgb col){
    unsigned int i;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    for(i=1;i<4;i++){
        if(p[i].red() >= qRed(col)){
            total_diff = p[i].red() - qRed(col);
        }
        else{
            total_diff = qRed(col) - p[i].red();
        }
        if(p[i].green() >= qGreen(col)){
            total_diff += p[i].green() - qGreen(col);
        }
        else{
            total_diff += qGreen(col) - p[i].green();
        }
        if(p[i].blue() >= qBlue(col)){
            total_diff += p[i].blue() - qBlue(col);
        }
        else{
            total_diff += qBlue(col) - p[i].blue();
        }
        if(total_diff < min_diff){
            min_diff = total_diff;
        }
    }
    return min_diff;
}

uint8_t NESpalette::bestColor(QRgb col){
    unsigned int i;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    uint8_t best_color = 0;
    for(i=0;i<4;i++){
        if(p[i].red() >= qRed(col)){
            total_diff = p[i].red() - qRed(col);
        }
        else{
            total_diff = qRed(col) - p[i].red();
        }
        if(p[i].green() >= qGreen(col)){
            total_diff += p[i].green() - qGreen(col);
        }
        else{
            total_diff += qGreen(col) - p[i].green();
        }
        if(p[i].blue() >= qBlue(col)){
            total_diff += p[i].blue() - qBlue(col);
        }
        else{
            total_diff += qBlue(col) - p[i].blue();
        }
        if(total_diff < min_diff){
            min_diff = total_diff;
            best_color = i;
        }
    }
    return best_color;
}

uint8_t NESpalette::bestSpriteColor(QRgb col){
    unsigned int i;
    uint32_t min_diff = 0xffffffff;
    uint32_t total_diff;
    uint8_t best_color = 0;
    for(i=1;i<4;i++){
        if(p[i].red() >= qRed(col)){
            total_diff = p[i].red() - qRed(col);
        }
        else{
            total_diff = qRed(col) - p[i].red();
        }
        if(p[i].green() >= qGreen(col)){
            total_diff += p[i].green() - qGreen(col);
        }
        else{
            total_diff += qGreen(col) - p[i].green();
        }
        if(p[i].blue() >= qBlue(col)){
            total_diff += p[i].blue() - qBlue(col);
        }
        else{
            total_diff += qBlue(col) - p[i].blue();
        }
        if(total_diff < min_diff){
            min_diff = total_diff;
            best_color = i;
        }
    }
    return best_color;
}


bool NESpalette::sameColors(const NESpalette & to_test){
    unsigned int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            if(to_test.nes_colors[j] == nes_colors[i]) break;
        }
        if(j>=4) return false;
    }
    return true;
}

uint8_t sprite::locateTileInArrangement(uint8_t to_find){
    uint8_t i,j;
    uint8_t tile_count = 0;
    uint8_t tiles_in_column;
    for(i=0;i<arrangement.length();){
        i++;
        tiles_in_column = arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            if(tile_count == to_find) return i;
            tile_count++;
            i+=2;
        }
    }
    return 0xff;
}

uint8_t sprite::locateColumnInArrangement(uint8_t to_find){
    uint8_t i,j,result;
    uint8_t tile_count = 0;
    uint8_t tiles_in_column;
    for(i=0;i<arrangement.length();){
        result = i;
        i++;
        tiles_in_column = arrangement[i++];
        for(j=0;j<tiles_in_column;j++){
            if(tile_count == to_find) return result;
            tile_count++;
            i+=2;
        }
    }
    return 0xff;
}
