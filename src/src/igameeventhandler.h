#pragma once

#include <stdint.h>

#include "gameevent.h"

class GameData;

class IGameEventHandler
{
public:
    IGameEventHandler()             { };
    virtual ~IGameEventHandler()    { };

    virtual bool HandleGameEvent(const int16_t eventype, GameData *gamedata, GameEventParam param)=0;

private:

};
