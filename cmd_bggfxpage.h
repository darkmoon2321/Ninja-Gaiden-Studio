#ifndef CMD_BGGFXPAGE_H
#define CMD_BGGFXPAGE_H

#include "script_command.h"

class cmd_BGGFXpage : public script_command
{
public:
    cmd_BGGFXpage();
    ~cmd_BGGFXpage();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint8_t chr_page;
    bool lower;
};

#endif // CMD_BGGFXPAGE_H
