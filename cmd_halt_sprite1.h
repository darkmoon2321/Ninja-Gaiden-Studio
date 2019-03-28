#ifndef CMD_HALT_SPRITE1_H
#define CMD_HALT_SPRITE1_H

#include "script_command.h"

class cmd_halt_sprite1 : public script_command
{
public:
    cmd_halt_sprite1();
    ~cmd_halt_sprite1();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint8_t speed;
};

#endif // CMD_HALT_SPRITE1_H
