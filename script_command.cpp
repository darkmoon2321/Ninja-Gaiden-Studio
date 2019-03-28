#include "script_command.h"

script_command::script_command()
{
    command = 2;
}

script_command::~script_command(){

}

uint8_t script_command::getCommand(){
    return command;
}
