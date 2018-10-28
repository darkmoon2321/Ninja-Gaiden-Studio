#ifndef CMD_UNKNOWN_H
#define CMD_UNKNOWN_H

#include "script_command.h"

class cmd_unknown : public script_command
{
public:
    cmd_unknown();
    ~cmd_unknown();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
private:
    uint8_t var;
};

#endif // CMD_UNKNOWN_H
