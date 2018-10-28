#ifndef CMD_TEXT_H
#define CMD_TEXT_H

#include "script_command.h"

class cmd_text : public script_command
{
public:
    cmd_text();
    ~cmd_text();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
private:
    dialog_string * dialog;
};

#endif // CMD_TEXT_H
