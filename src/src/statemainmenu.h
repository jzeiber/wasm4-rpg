#pragma once

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "game.h"

class StateMainMenu:public IState,public IUpdatable,public IDrawable,public IInputHandler
{
public:
    StateMainMenu();
    ~StateMainMenu();

    static StateMainMenu &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

private:
    uint8_t m_changestate;
    int8_t m_selectedslot;
    bool m_showdialog;
    bool m_showingdialog;
    Game *m_game;
};
