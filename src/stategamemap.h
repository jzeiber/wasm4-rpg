#pragma once

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "gamedata.h"

class StateGameMap:public IState,public IUpdatable,public IDrawable,public IInputHandler
{
public:
    StateGameMap();
    ~StateGameMap();

    static StateGameMap &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

private:
    GameData *m_gamedata;
    int8_t m_changestate;
    int64_t m_x;
    int64_t m_y;
};