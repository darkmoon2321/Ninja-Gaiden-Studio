#ifndef CMD_PALETTESET_H
#define CMD_PALETTESET_H

#include "script_command.h"

class cmd_paletteset : public script_command
{
public:
    cmd_paletteset();
    ~cmd_paletteset();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
private:
    uint8_t num_to_change;
    bool start_sprites;
    NESpalette ** changed_palettes;
};

#endif // CMD_PALETTESET_H
