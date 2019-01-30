#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "data_types.h"
#include "script_command.h"
#include "cmd_bar_or_reset.h"
#include "cmd_bgcol_mask.h"
#include "cmd_bggfxpage.h"
#include "cmd_bgset.h"
#include "cmd_blackout.h"
#include "cmd_dialog_delay.h"
#include "cmd_earthquake.h"
#include "cmd_end_scene_during_popcorn_viewer.h"
#include "cmd_fade.h"
#include "cmd_flash.h"
#include "cmd_halt_sprite1.h"
#include "cmd_mirroring.h"
#include "cmd_movesprite.h"
#include "cmd_music.h"
#include "cmd_paletteset.h"
#include "cmd_pputransmit.h"
#include "cmd_scrollsetup.h"
#include "cmd_scrollspeed.h"
#include "cmd_setup_boss_rush_text.h"
#include "cmd_setup_hard_mode.h"
#include "cmd_setup_ranking_text.h"
#include "cmd_spriteset.h"
#include "cmd_sync.h"
#include "cmd_text.h"
#include "cmd_unknown.h"
#include "cmd_wait.h"

class data_handler
{
public:
    data_handler();
    void importAll(uint8_t *);
    uint8_t * exportAll();
    void writeToTXT(std::string file_base);
    void importScene(uint8_t scene_num, std::string);
    uint8_t getNumScenes();
private:
    //const uint32_t ASM_FADE = 0x106A4;                        //Don't need to modify the fade timers.  They are all equal to the array index, 0 to 1F.
    const uint32_t ASM_ANIM_SPRITE_TIMER = 0x105CD;
    const uint32_t ASM_FIXED_SPRITE_MOVEMENT_TIMER = 0x10657;
    const uint32_t ASM_EARTHQUAKE_TIMER = 0x10529;
    const uint32_t ASM_EARTHQUAKE_PTR_LO = 0x10894;
    const uint32_t ASM_EARTHQUAKE_PTR_HI = 0x10899;
    const uint32_t ASM_SCROLL_SETUP_PTR_LO = 0x106E8;
    const uint32_t ASM_SCROLL_SETUP_PTR_HI = 0x106ED;
    const uint32_t ASM_SCROLL_SETUP_PTR_LO_ALT = 0x1076B;
    const uint32_t ASM_SCROLL_SETUP_PTR_HI_ALT = 0x10770;
    const uint32_t ASM_SPRITE_LINEAR_MOTION_0 = 0x10637;
    const uint32_t ASM_SPRITE_LINEAR_MOTION_1 = 0x1063D;
    const uint32_t ASM_SPRITE_LINEAR_MOTION_2 = 0x10643;
    const uint32_t ASM_SPRITE_LINEAR_MOTION_3 = 0x10649;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_0 = 0x1066B;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_1 = 0x10671;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_2 = 0x10677;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_3 = 0x1067D;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_4 = 0x10683;
    const uint32_t ASM_SPRITE_ACCEL_MOTION_5 = 0x10689;
    const uint32_t ASM_PPU_BASE_ADDRESS_0 = 0x10566;
    const uint32_t ASM_PPU_BASE_ADDRESS_1 = 0x1056B;
    const uint32_t ASM_PALLETE_PTR_ADDRESS_LO = 0x1036F;
    const uint32_t ASM_PALLETE_PTR_ADDRESS_HI = 0x10374;
    const uint32_t ASM_SCENE_PTR_LO = 0x100D1;
    const uint32_t ASM_SCENE_PTR_HI = 0x100D6;
    const uint32_t ASM_SPRITEXY = 0x1403D;
    const uint32_t ASM_SPRITEXY_ALT = 0x1408B;
    const uint32_t ASM_ANIM_SPRITE_PTR_LO = 0x141EC;
    const uint32_t ASM_ANIM_SPRITE_PTR_HI = 0x141F2;
    const uint32_t ASM_SPRITE_SHAKE_LO = 0x1413C;
    const uint32_t ASM_SPRITE_SHAKE_HI = 0X14141;
    const uint32_t ASM_SPRITE_PTR_LO = 0x14025;
    const uint32_t ASM_SPRITE_PTR_HI = 0x1402A;
    const uint32_t ASM_ANIM_SPRITE_TIMER_TXT_ENGINE = 0x142F9;
    const uint32_t ASM_TXT_PTR_LO = 0x143CA;
    const uint32_t ASM_TXT_PTR_HI = 0x143CF;
    const uint32_t ASM_BG_ARRANGEMENT_PTR_LO = 0x1801F;
    const uint32_t ASM_BG_ARRANGEMENT_PTR_HI = 0x18024;
    const uint32_t ASM_BG_TILESET_PTR_LO = 0x1803B;
    const uint32_t ASM_BG_TILESET_PTR_HI = 0x18040;
    const uint32_t ASM_BG_ATTRIB_PTR_LO = 0x18045;
    const uint32_t ASM_BG_ATTRIB_PTR_HI = 0x1804A;
    const uint32_t ASM_SPRITE_PATTERN_TABLE = 0X1002B;

    const uint32_t BANK4_WRITE_START = 0x108DF; //currently don't modify the fade timer array (value == index)
    const uint32_t BANK5_WRITE_START = 0x14418; //don't include spriteXY position array
    const uint32_t BANK6_WRITE_START = 0x182E4;

    uint8_t * rom_data;
    uint16_t * start_timers;
    //script_command * scenes[0x100][0x800];
    std::vector<script_command *> scenes[0x100];
    //uint16_t * scene_sizes;
    uint8_t num_scenes;

    void clearData();
    bool compactImages();
};

#endif // DATA_HANDLER_H
