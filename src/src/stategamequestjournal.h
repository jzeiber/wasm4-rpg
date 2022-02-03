#pragma once

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "gamedata.h"

class StateGameQuestJournal:public IState,public IUpdatable,public IDrawable,public IInputHandler
{
public:
    StateGameQuestJournal();
    ~StateGameQuestJournal();

    static StateGameQuestJournal &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

private:
    GameData *m_gamedata;
    int8_t m_changestate;
    int8_t m_questidx;

    bool HavePreviousActiveQuest(const int8_t questidx) const;
    bool HaveNextActiveQuest(const int8_t questidx) const;
    bool SetQuestOffset(const int32_t offset);

};