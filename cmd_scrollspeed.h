#ifndef CMD_SCROLLSPEED_H
#define CMD_SCROLLSPEED_H

#include "script_command.h"

class cmd_scrollspeed : public script_command
{
public:
    cmd_scrollspeed();
    ~cmd_scrollspeed();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
    QString getName();
private:
    bool stopX;
    bool stopY;
    uint8_t stop_splits;
    scroll_params * params;
};

#endif // CMD_SCROLLSPEED_H
