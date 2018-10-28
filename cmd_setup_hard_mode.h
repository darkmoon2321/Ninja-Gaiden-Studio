#ifndef CMD_SETUP_HARD_MODE_H
#define CMD_SETUP_HARD_MODE_H

#include "script_command.h"

class cmd_setup_hard_mode : public script_command
{
public:
    cmd_setup_hard_mode();
    ~cmd_setup_hard_mode();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
private:
    uint8_t unused_var;
};

#endif // CMD_SETUP_HARD_MODE_H
