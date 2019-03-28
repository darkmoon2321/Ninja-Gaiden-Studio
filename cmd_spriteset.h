#ifndef SPRITESET_H
#define SPRITESET_H

#include "script_command.h"

class cmd_spriteset : public script_command
{
public:
    cmd_spriteset();
    ~cmd_spriteset();
    uint32_t writeToScene(uint8_t *,uint32_t);
    std::string writeToTXT();
    uint32_t parseData(uint8_t *, uint32_t,uint8_t);
    bool killSignal();
    void parseText(std::string,std::string,uint32_t &);
    void addRef(uint8_t);
    QString getName();
private:
    uint8_t num_arrangements;
    NESpalette * pals[4];
    bool * animation;
    void ** arrangements;
    std::string x_values;
    std::string y_values;
    std::string attribs;
    std::string move_types;
    void ** movement_pointers;


};

#endif // SPRITESET_H
