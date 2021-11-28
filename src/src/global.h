#pragma once

#include "game.h"
#include "input.h"

namespace global
{
    extern Game *game;
    extern Input *input;
    extern const uint8_t buffsize;
    extern char buff[];

    void SetupGlobals();
}
