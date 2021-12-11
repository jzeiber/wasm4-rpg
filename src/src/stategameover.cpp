#include "stategameover.h"
#include "wasm4.h"
#include "game.h"
#include "textprinter.h"

StateGameOver::StateGameOver():m_changestate(0)
{

}

StateGameOver::~StateGameOver()
{

}

StateGameOver &StateGameOver::Instance()
{
    static StateGameOver so;
    return so;
}

void StateGameOver::StateChanged(const uint8_t prevstate, void *params)
{
    m_changestate=0;
}

void StateGameOver::Update(const int ticks, Game *game)
{
    if(m_changestate!=0)
    {
        game->ChangeState(m_changestate,game);
    }
}

bool StateGameOver::HandleInput(const Input *input)
{
    if(input->GamepadButtonPress(1,BUTTON_1)==true || input->GamepadButtonDown(1,BUTTON_2)==true)
    {
        m_changestate=Game::GameState::STATE_MAINMENU;
    }
    return true;
}

void StateGameOver::Draw()
{
    TextPrinter tp;
    tp.PrintCentered("Game Over",SCREEN_SIZE/2,70,20,PALETTE_WHITE);
}
