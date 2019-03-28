#ifndef BGSET_H
#define BGSET_H

#include "script_command.h"

class cmd_BGSET : public script_command
{
public:
    cmd_BGSET();
    ~cmd_BGSET();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
    QString getName();
private:
    uint8_t num_arrangements;
    NESpalette * pals[3];
    uint8_t nes_ubg;
    bg_arrangement ** arrangements;
    ppu_base_type ** bases;

};

#endif // BGSET_H
