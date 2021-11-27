
#include "global.h"

#include "wasm4.h"
#include "printf.h"

namespace global
{
    Game *game;
    Input *input;
    const uint8_t buffsize=192;
    char buff[192];

    void SetupGlobals()
    {
        game=&Game::Instance();
        input=&Input::Instance();
    }
}
