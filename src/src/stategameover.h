#pragma once

#include "idrawable.h"
#include "iupdatable.h"
#include "istate.h"
#include "iinputhandler.h"

class StateGameOver:public IState, public IUpdatable, public IInputHandler, public IDrawable
{
public:
    StateGameOver();
    ~StateGameOver();

    static StateGameOver &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    void Update(const int ticks, Game *game);
    bool HandleInput(const Input *i);
    void Draw();

private:
    uint8_t m_changestate;
};
