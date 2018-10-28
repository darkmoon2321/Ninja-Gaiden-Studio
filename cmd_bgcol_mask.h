#ifndef CMD_BGCOL_MASK_H
#define CMD_BGCOL_MASK_H

#include "script_command.h"

class cmd_BGCOL_MASK : public script_command
{
public:
    cmd_BGCOL_MASK();
    ~cmd_BGCOL_MASK();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
private:
    uint8_t color;
    uint8_t mask_bits;
};

#endif // CMD_BGCOL_MASK_H
