#ifndef CMD_FLASH_H
#define CMD_FLASH_H

#include "script_command.h"

class cmd_flash : public script_command
{
public:
    cmd_flash();
    ~cmd_flash();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t, uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
};

#endif // CMD_FLASH_H
