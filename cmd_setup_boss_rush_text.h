#ifndef CMD_SETUP_BOSS_RUSH_TEXT_H
#define CMD_SETUP_BOSS_RUSH_TEXT_H

#include "script_command.h"

class cmd_setup_boss_rush_text : public script_command
{
public:
    cmd_setup_boss_rush_text();
    ~cmd_setup_boss_rush_text();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint8_t unused_var;
};

#endif // CMD_SETUP_BOSS_RUSH_TEXT_H
