#ifndef CMD_EARTHQUAKE_H
#define CMD_EARTHQUAKE_H

#include "script_command.h"

class cmd_earthquake : public script_command
{
public:
    cmd_earthquake();
    ~cmd_earthquake();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
private:
    earthquake * quake;
};

#endif // CMD_EARTHQUAKE_H
