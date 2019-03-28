#ifndef CMD_WAIT_H
#define CMD_WAIT_H

#include "script_command.h"

class cmd_wait : public script_command
{
public:
    cmd_wait();
    ~cmd_wait();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint16_t delay;
};

#endif // CMD_WAIT_H
