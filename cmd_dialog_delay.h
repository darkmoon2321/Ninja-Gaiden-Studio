#ifndef CMD_DIALOG_DELAY_H
#define CMD_DIALOG_DELAY_H

#include "script_command.h"

class cmd_dialog_delay : public script_command
{
public:
    cmd_dialog_delay();
    ~cmd_dialog_delay();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
private:
    uint8_t delay;
};

#endif // CMD_DIALOG_DELAY_H
