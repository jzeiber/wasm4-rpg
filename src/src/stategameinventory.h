#pragma once

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "gamedata.h"

class StateGameInventory:public IState,public IUpdatable,public IDrawable,public IInputHandler
{
public:
    StateGameInventory();
    ~StateGameInventory();

    static StateGameInventory &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

private:
    GameData *m_gamedata;
    int8_t m_changestate;
    int8_t m_selecteditem;
    uint8_t m_selectedmenu;

    enum MenuOptions
    {
        OPTION_EXIT=0,
        OPTION_EQUIP=1,
        OPTION_CONSUME=2,
        OPTION_DROP=3,
        OPTION_MOVELEFT=4,
        OPTION_MOVERIGHT=5,
        OPTION_MAX
    };

    void SelectItemOffset(int16_t offset);

};
