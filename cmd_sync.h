#ifndef CMD_SYNC_H
#define CMD_SYNC_H

#include "script_command.h"

class cmd_sync : public script_command
{
public:
    cmd_sync();
    ~cmd_sync();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
};

#endif // CMD_SYNC_H
