#pragma once

#include "iupdatable.h"
#include "istate.h"

class StateStartup:public IState, public IUpdatable
{
public:
    StateStartup();
    ~StateStartup();

    static StateStartup &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    void Update(const int ticks, Game *game);
};
