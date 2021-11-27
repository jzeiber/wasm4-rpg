#include "statestartup.h"
#include "game.h"

StateStartup::StateStartup()
{

}

StateStartup::~StateStartup()
{

}

StateStartup &StateStartup::Instance()
{
    static StateStartup ss;
    return ss;
}

void StateStartup::StateChanged(const uint8_t prevstate, void *params)
{

}

void StateStartup::Update(const int ticks, Game *game)
{
    game->ChangeState(Game::STATE_MAINMENU,game);
}
