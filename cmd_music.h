#ifndef CMD_MUSIC_H
#define CMD_MUSIC_H

#include "script_command.h"

class cmd_music : public script_command
{
public:
    cmd_music();
    ~cmd_music();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint8_t track_number;
};

#endif // CMD_MUSIC_H
