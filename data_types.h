#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>
#include <QImage>
#include <QColor>

/* ASM LOCATIONS TO LOCATE AND PLACE DATA POINTERS:
 * Fade timer: x106A4
 * Animated sprite timer: x105CD
 * fixed sprite movement timers: x10657
 * earthquake scroll timers: x10529
 * earthquake scroll pointers: x10894 low byte, x10899 high byte
 * scroll setup pointers: x106E8/x1076B low byte, x106ED/x10770 high byte
 * sprite linear motion values x10637(byte 0), x1063D(byte 1),x10643(byte 2), x10649(byte 3)
 * sprite accelerating motion values x1066B(byte 0), x10671(byte 1), x10677(byte 2), x1067D(byte 3),x10683(byte 4), x10689(byte 5)
 * PPU base addresses for BG images: x10566, and x1056B
 * Main cut scene scripting pointers: x100D1 low byte and x100D6 high byte
 * sprite x,y position table x1403D/x1408B
 * animated sprite sequence pointers x141EC low byte and x141F2 high byte
 * pointers to fixed sprite movement sequences x1413C low byte and x14141 high byte
 * sprite definition pointers: x14025 low byte, x1402A high byte
 * animated sprite timers when set by dialog engine x142F9
 * text sequence pointers: x143CA low byte, x143CF high byte
 * BG arrangement pointers: x1801F low byte, x18024 high byte
 * BG tileset definition pointers: x1803B low byte, x18040 high byte
 * BG tile attribute definition pointers: x18045 low byte, x1804A high byte
 * */
typedef struct ng_type{
    uint64_t references;
    uint8_t id;

}ng_type;

typedef struct NEStile{     //checksum makes searching for duplicates significantly faster
    uint8_t t[0x10];
    uint16_t checksum;
    uint8_t id;
    bool shared;
    uint8_t attrib_changes;

    NEStile & operator = (const NEStile & to_copy);
    bool operator == (const NEStile &);
    void hflip();
    void vflip();
    void hvflip();
}NEStile;


typedef struct CHR_page : ng_type{
    NEStile t[0x100];
    uint32_t address;
    uint16_t num_used;
    bool bg_used[0x100];
    bool sprite_used[0x100];
    //uint8_t attrib_changes[0x100];

    CHR_page & operator = (const CHR_page &);
} CHR_page;

typedef struct tileset_def{
    NEStile t[8];
    uint8_t attribs;
} tileset_def;

typedef struct scroll_splits{
    uint8_t X_coarse;
    uint8_t split_height_fine;
    uint8_t split_height_coarse;
    uint8_t Vx_fine;
    uint8_t Vx_coarse;
} scroll_splits;

typedef struct scroll_params : ng_type{       //Command 3 uses all this, command B only uses num_splits and the splits.  Mostly command B is used for stopping scrolling though.
    uint8_t num_splits;
    uint8_t ppu_ctrl_mod;
    uint8_t sprite_0_Y;
    uint8_t X_coarse;
    uint8_t Y_coarse;
    uint8_t Vx_fine;
    uint8_t Vx_coarse;
    uint8_t Vy_fine;
    uint8_t Vy_coarse;
    scroll_splits splits[4];
    scroll_params &operator = (const scroll_params &s);
    bool operator == (const scroll_params & s);
    bool speedEquals(const scroll_params & s);
    std::string write();
} scroll_params;

typedef struct sprite_linear : ng_type{
    uint8_t Vx_fine;
    uint8_t Vx_coarse;
    uint8_t Vy_fine;
    uint8_t Vy_coarse;

    sprite_linear & operator = (const sprite_linear &);
    bool operator == (const sprite_linear &);
    std::string write();
} sprite_linear;

typedef struct sprite_accel : ng_type{
    uint8_t Vx_fine;
    uint8_t Vx_coarse;
    uint8_t Vy_fine;
    uint8_t Vy_coarse;
    uint8_t Ax;
    uint8_t Ay;

    sprite_accel & operator = (const sprite_accel &);
    bool operator == (const sprite_accel &);
    std::string write();
} sprite_accel;


typedef struct earthquake : ng_type{
    std::string ppu_ctrl;
    std::string x;
    std::string y;
    uint8_t num_steps;
    uint8_t timer;

    earthquake & operator = (const earthquake &);
    bool operator == (const earthquake &);
    std::string write();
}earthquake;

typedef struct NESpalette : ng_type{
    uint8_t nes_colors[4];
    QColor p[4];

    //void set(uint8_t);
    NESpalette &operator = (const NESpalette &);
    bool operator == (const NESpalette &);
    std::string write();
    uint32_t colorDiff(QRgb);
    uint8_t bestColor(QRgb);
    bool sameColors(const NESpalette &);
} NESpalette;

typedef struct palette_set{
    QColor ubg_color;
    NESpalette p[4];
    uint8_t nes_ubg;

    palette_set & operator = (const palette_set &);
    bool operator == (const palette_set &);
    uint8_t duplicates();
    void convertColors();
} palette_set;

typedef struct sprite : ng_type{
    QImage image;
    std::vector<NEStile *> tiles;
//    uint8_t num_tiles;
    uint8_t gfx_page;
    uint8_t bg_gfx_page;
    std::string arrangement;
    bool modified;
    std::vector <palette_set> pals;

    sprite & operator = (const sprite &);
    bool operator == (const sprite &);
    std::string write();
    palette_set bestPalette();
    void generateImage();
}sprite;

typedef struct metatile : ng_type{
    NEStile * tiles[8];
    uint8_t attrib_bits;
    uint16_t checksum;

    std::string write();
    metatile & operator = (const metatile &);
    bool operator == (const metatile &);
} metatile;

typedef struct metatile_set : ng_type{
    metatile set[0x100];
    uint16_t num_metatiles;

    uint16_t add(metatile);
} metatile_set;

typedef struct bg_arrangement : ng_type{
    QImage image;
    std::vector <NEStile *> tiles;
    std::string attrib_bits;
    uint8_t gfx_page;
    std::string arrangement;
    uint8_t metatilesX;
    uint8_t metatilesY;
    metatile * metatiles;
    uint8_t tileset;
    std::vector <palette_set> pals;
    bool modified;

    bg_arrangement();
    ~bg_arrangement();
    bg_arrangement & operator = (const bg_arrangement &);
    bool operator == (const bg_arrangement &);
    std::string write();
    palette_set bestPalette();
    void generateImage();
} bg_arrangement;

typedef struct sprite_animation : ng_type{
    uint8_t timer;
    sprite ** sequence;
    uint8_t num_steps;
    bool text_engine;

    sprite_animation();
    ~sprite_animation();
    sprite_animation & operator = (const sprite_animation &);
    bool operator == (const sprite_animation &);
    std::string write();
}sprite_animation;

typedef struct sprite_shake : ng_type{
    std::string x;
    std::string y;
    uint8_t num_steps;
    uint8_t timer;

    sprite_shake &operator = (const sprite_shake &);
    bool operator == (const sprite_shake &);
    std::string write();
}sprite_shake;

typedef struct ppu_string : ng_type{
    QImage image;
    std::string s;
    std::vector <NEStile *> tiles;
    uint16_t address;
    uint8_t gfx_page;
    bool modified;
    ppu_string &operator = (const ppu_string &);
    bool operator == (const ppu_string &);
    std::string write();
    void generateImage();
}ppu_string;

typedef struct dialog_string : ng_type{
    std::string text;
    std::vector<ng_type *> data;
    std::vector<bool> is_animation;
    std::string write();
    bool operator == (const dialog_string & s);
} dialog_string;

typedef struct ppu_base_type : ng_type{
    uint16_t a;
    std::string write();
} ppu_base_type;



extern const int16_t NES_COLORS_RGB[];
extern uint32_t sprite_timer_address;
extern uint32_t sprite_shake_timer_address;
extern uint32_t earthquake_timer_address;
extern uint32_t earthquake_ptr_address;
extern uint32_t scroll_setup_address;
extern uint32_t sprite_linear_address;
extern uint32_t sprite_accel_address;
extern uint32_t ppu_base_address;
extern uint32_t scene_ptr_address;
extern uint32_t spritexy_address;
extern uint32_t animated_sprite_ptr_address;
extern uint32_t sprite_shake_address;
extern uint32_t sprite_ptr_address;
extern uint32_t sprite_timer_txt_address;
extern uint32_t text_ptr_address;
extern uint32_t bg_arrangement_address;
extern uint32_t bg_tileset_address;
extern uint32_t bg_attrib_address;
extern uint32_t palette_ptr_address;

extern ppu_string * ppu_strings[0x100];
extern uint16_t num_ppu_strings;
extern ppu_base_type * ppu_bases[0x100];
extern uint16_t num_ppu_bases;
//extern uint8_t * spriteXYs;
//extern uint8_t num_spriteXYs;
extern scroll_params * scroll_params_list[0x40];
extern uint16_t num_params;
extern sprite_linear * sprite_linears[0x100];
extern uint16_t num_sprite_linears;
extern sprite_accel * sprite_accels[0x100];
extern uint16_t num_sprite_accels;
extern earthquake * earthquakes[0x100];
extern uint16_t num_earthquakes;
extern sprite_shake * sprite_shakes[0x100];
extern uint16_t num_sprite_shakes;
extern sprite * sprites[0x100];
extern uint16_t num_sprites;
extern sprite_animation * sprite_animations[0x80];
extern uint16_t num_sprite_animations;
extern NESpalette * palettes[0x100];
extern uint16_t num_palettes;
extern bg_arrangement * current_bgs[0x10];
extern uint8_t num_current_bgs;
extern std::vector<sprite *> current_sprites;
extern uint8_t num_current_sprites;
extern palette_set current_palettes[2];
extern ppu_string * current_strings[0x10];
extern uint8_t num_current_strings;
extern dialog_string * all_dialog[0x100];
extern uint16_t num_dialogs;
extern bg_arrangement * bgs[0x100];
extern uint16_t num_bgs;
extern uint16_t sprite_CHR_page;
extern uint16_t bg_CHR_page;
extern bool image_changed;
extern CHR_page * CHR_pages;
extern uint8_t num_chr_pages;
extern uint32_t line_number;
extern uint8_t spritexy_values[0x29];

scroll_params * getScrollParams(uint8_t *,uint8_t);
ppu_string * getPPUString(uint8_t *,uint16_t);
sprite_linear * getSpriteLinear(uint8_t *,uint8_t);
sprite_shake * getSpriteShake(uint8_t *,uint8_t);
sprite_accel * getSpriteAccel(uint8_t *,uint8_t);
sprite * getSprite(uint8_t *,uint8_t);
sprite_animation * getSpriteAnimation(uint8_t *,uint8_t);
earthquake * getEarthquake(uint8_t * ,uint8_t);
NESpalette * getPalette(uint8_t *rom_data, uint8_t);
dialog_string * getText(uint8_t *,uint8_t);
ppu_base_type * getPPUBase(uint8_t *, uint8_t);
bg_arrangement * getBGArrangement(uint8_t *, uint8_t);

char convertToASCII(char c);
std::string convertByteToHexString(uint8_t toConvert);
std::string convert24BitToHexString(uint32_t toConvert);
QColor getColor(uint8_t);
std::string getLine(const std::string &s, uint32_t & offset);
bool isValidText(uint8_t to_test);
bool isWhitespace(uint8_t to_test);
void skipWhitespace(const std::string s,uint32_t & offset);
int8_t convertToHex(char c);
int32_t getIntFromTXT(const std::string & s,uint32_t & offset);
std::string getWordFromTXT(const std::string & s,uint32_t & offset);
void lineUpper(std::string &);
#endif // DATA_TYPES_H
