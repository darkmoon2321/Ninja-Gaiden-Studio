#ifndef CMD_BAR_OR_RESET_H
#define CMD_BAR_OR_RESET_H

#include "script_command.h"

class cmd_bar_or_reset : public script_command
{
public:
    cmd_bar_or_reset();
    ~cmd_bar_or_reset();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string text, std::string line, uint32_t &offset);
    QString getName();
private:
    uint8_t unused_var;
};

#endif // CMD_BAR_OR_RESET_H
