#pragma once

#include <stdint.h>

#include "gameevent.h"

class GameData;

class IGameEventHandler
{
public:
    IGameEventHandler()             { };
    virtual ~IGameEventHandler()    { };

    virtual bool HandleGameEvent(const int16_t eventype, GameData *gamedata, GameEventParam param, const int8_t idx)=0;

private:

};
