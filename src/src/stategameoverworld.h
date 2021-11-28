#pragma once

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "gamedata.h"
#include "questdata.h"

class StateGameOverworld:public IState,public IUpdatable,public IDrawable,public IInputHandler
{
public:
    StateGameOverworld();
    ~StateGameOverworld();

    static StateGameOverworld &Instance();

    void StateChanged(const uint8_t prevstate, void *params);
    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

private:
    GameData *m_gamedata;
    QuestData m_tempquest;
    int8_t m_changestate;
    bool m_showtowndialog;
    bool m_showingtowndialog;
    bool m_savegame;
    int8_t m_towndialogtype;
    uint64_t m_lastmovetick;
    uint64_t m_tick;

    int8_t GetNextAvailableQuestIndex() const;  // -1 if none are available
    bool HaveExistingActiveQuest(const uint64_t wx, const uint64_t wy) const;   // returns true if an existing active quest was giving by the location

};
