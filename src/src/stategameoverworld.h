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
    int8_t m_cursormode;
    int8_t m_cursorx;
    int8_t m_cursory;
    int8_t m_changestate;
    bool m_showtowndialog;
    bool m_showingtowndialog;
    bool m_showexitdialog;
    bool m_showingexitdialog;
    bool m_savegame;
    int8_t m_towndialogtype;
    uint64_t m_lastmovetick;
    uint64_t m_lastrepeattick;
    uint64_t m_tick;

    enum MenuOptions
    {
        OPTION_SAVE=0,
        OPTION_JOURNAL,
        OPTION_MAP,
        OPTION_REST,
        OPTION_CHARACTER,
        OPTION_INVENTORY,
        OPTION_PICKUP,
        OPTION_HOME,
        OPTION_MAX,
    };

    enum CursorMode
    {
        MODE_MOVE=0,
        MODE_TARGET
    };

    int8_t GetNextAvailableQuestIndex() const;  // -1 if none are available
    bool HaveExistingActiveQuest(const uint64_t wx, const uint64_t wy) const;   // returns true if an existing active quest was giving by the location
    void DrawMenuBar();
    void DrawHealthBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h);
    void DrawManaBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h);
    void DrawExpBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h);
    void DrawMobHealth(const int16_t x, const int16_t y, const int16_t health, const int16_t maxhealth);
    void GetMenuSpriteSheetPos(const int16_t option, int16_t &x, int16_t &y);

};
