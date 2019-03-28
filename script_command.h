#ifndef SCRIPT_COMMAND_H
#define SCRIPT_COMMAND_H

#include <stdint.h>
#include <string>
#include "data_types.h"
#include "QString"

class data_handler;

class script_command
{
public:
    script_command();
    virtual ~script_command() = 0;
    virtual uint32_t writeToScene(uint8_t *,uint32_t) = 0;
    virtual std::string writeToTXT() = 0;
    virtual uint32_t parseData(uint8_t *, uint32_t,uint8_t) = 0;
    virtual bool killSignal() = 0;
    virtual void parseText(std::string,std::string,uint32_t &) = 0;
    virtual QString getName() = 0;
    uint8_t getCommand();
protected:
    uint8_t command;
};

#endif // SCRIPT_COMMAND_H
