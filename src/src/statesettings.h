#pragma once

#include "iupdatable.h"
#include "istate.h"
#include "idrawable.h"
#include "iinputhandler.h"

class StateSettings:public IState, public IUpdatable, public IDrawable, public IInputHandler
{
public:
    StateSettings();
    ~StateSettings();

    static StateSettings &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    void Update(const int ticks, Game *game);
    bool HandleInput(const Input *i);
    void Draw();

private:
    uint8_t m_changestate;
    uint8_t m_selected;

    void DrawSlider(const int16_t x, const int16_t y, const int16_t min, const int16_t max, const int16_t val, const bool selected);

};
