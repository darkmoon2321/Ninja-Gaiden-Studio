#ifndef CMD_MIRRORING_H
#define CMD_MIRRORING_H

#include "script_command.h"

class cmd_mirroring : public script_command
{
public:
    cmd_mirroring();
    ~cmd_mirroring();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
private:
    bool horizontal;
    uint8_t value;
};

#endif // CMD_MIRRORING_H
