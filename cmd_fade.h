#ifndef CMD_FADE_H
#define CMD_FADE_H

#include "script_command.h"

class cmd_fade : public script_command
{
public:
    cmd_fade();
    ~cmd_fade();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    bool fade_in;
    bool fade_sprites;
    bool fade_bg;
    uint8_t fade_timer;
};

#endif // CMD_FADE_H
