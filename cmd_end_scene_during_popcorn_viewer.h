#ifndef CMD_END_SCENE_DURING_POPCORN_VIEWER_H
#define CMD_END_SCENE_DURING_POPCORN_VIEWER_H

#include "script_command.h"

class cmd_end_scene_during_popcorn_viewer : public script_command
{
public:
    cmd_end_scene_during_popcorn_viewer();
    ~cmd_end_scene_during_popcorn_viewer();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    QString getName();
private:
    uint8_t unused_var;
};

#endif // CMD_END_SCENE_DURING_POPCORN_VIEWER_H
