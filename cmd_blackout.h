#ifndef CMD_BLACKOUT_H
#define CMD_BLACKOUT_H

#include "script_command.h"

class cmd_blackout : public script_command
{
public:
    cmd_blackout();
    ~cmd_blackout();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    bool erase_sprites;
    bool erase_bg;
};

#endif // CMD_BLACKOUT_H
