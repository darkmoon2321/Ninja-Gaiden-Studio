#ifndef CMD_MOVESPRITE_H
#define CMD_MOVESPRITE_H

#include "script_command.h"

class cmd_movesprite : public script_command
{
public:
    cmd_movesprite();
    ~cmd_movesprite();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
    QString getName();
private:
    bool fliph;
    bool flipv;
    bool priority;
    uint8_t sprite_num;
    uint8_t movement_type;
    void * movement_pointer;
};

#endif // CMD_MOVESPRITE_H
