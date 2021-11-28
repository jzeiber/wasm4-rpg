#pragma once

#include <stdint.h>

class IState
{
public:
    IState()            { };
    virtual ~IState()   { };

    virtual void StateChanged(const uint8_t prevstate, void *params)=0;
};
