#include "mainwindow.h"
#include <QApplication>

const int16_t NES_COLORS_RGB[] = {124,124,124, 0,0,252, 0,0,188, 68,40,188, 148,0,132, 168,0,32, 168,16,0, 136,20,0, 80,48,0, 0,120,0, 0,104,0, 0,88,0, 0,64,88, 0,0,0, 0,0,0, 0,0,0, 188,188,188, 0,120,248, 0,88,248, 104,68,252, 216,0,204, 228,0,88, 248,56,0, 228,92,16, 172,124,0, 0,184,0, 0,168,0, 0,168,68, 0,136,136, 0,0,0, 0,0,0, 0,0,0, 248,248,248, 60,188,252, 104,136,252, 152,120,248, 248,120,248, 248,88,152, 248,120,88, 252,160,68, 248,184,0, 184,248,24, 88,216,84, 88,248,152, 0,232,216, 120,120,120, 0,0,0, 0,0,0, 252,252,252, 164,228,252, 184,184,248, 216,184,248, 248,184,248, 248,164,192, 240,208,176, 252,224,168, 248,216,120, 216,248,120, 184,248,184, 184,248,216, 0,252,252, 248,216,248, 0,0,0, 0,0,0};

uint32_t sprite_timer_address;
uint32_t sprite_shake_timer_address;
uint32_t earthquake_timer_address;
uint32_t earthquake_ptr_address;
uint32_t scroll_setup_address;
uint32_t sprite_linear_address;
uint32_t sprite_accel_address;
uint32_t ppu_base_address;
uint32_t scene_ptr_address;
uint32_t spritexy_address = 0x143EF;
uint32_t animated_sprite_ptr_address;
uint32_t sprite_shake_address;
uint32_t sprite_ptr_address;
uint32_t sprite_timer_txt_address;
uint32_t text_ptr_address;
uint32_t bg_arrangement_address;
uint32_t bg_tileset_address;
uint32_t bg_attrib_address;
uint32_t palette_ptr_address;

ppu_string * ppu_strings[0x100];
uint16_t num_ppu_strings;
ppu_base_type * ppu_bases[0x100];
uint16_t num_ppu_bases;
scroll_params * scroll_params_list[0x40];
uint16_t num_params;
sprite_linear * sprite_linears[0x100];
uint16_t num_sprite_linears;
sprite_accel * sprite_accels[0x100];
uint16_t num_sprite_accels;
earthquake * earthquakes[0x100];
uint16_t num_earthquakes;
sprite_shake * sprite_shakes[0x100];
uint16_t num_sprite_shakes;
sprite * sprites[0x100];
uint16_t num_sprites;
sprite_animation * sprite_animations[0x80];
uint16_t num_sprite_animations;
NESpalette * palettes[0x100];
uint16_t num_palettes;
bg_arrangement * current_bgs[0x10];
uint8_t num_current_bgs;
std::vector<sprite *> current_sprites;
uint8_t num_current_sprites;
palette_set current_palettes[2];
dialog_string * all_dialog[0x100];
uint16_t num_dialogs;
bg_arrangement * bgs[0x100];
uint16_t num_bgs;
uint16_t sprite_CHR_page;
uint16_t bg_CHR_page;
bool image_changed;
ppu_string * current_strings[0x10];
uint8_t num_current_strings;
CHR_page * CHR_pages;
uint8_t num_chr_pages;
uint32_t line_number;
uint8_t spritexy_values[0x29];

int main(int argc, char *argv[])
{
    line_number = 0;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
