#ifndef CMD_SCROLLSETUP_H
#define CMD_SCROLLSETUP_H

#include "script_command.h"

class cmd_scrollsetup : public script_command
{
public:
    cmd_scrollsetup();
    ~cmd_scrollsetup();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
private:
    bool enable_text;
    bool bit6;
    scroll_params * params;
};

#endif // CMD_SCROLLSETUP_H
