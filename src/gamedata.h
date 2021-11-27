#pragma once

#include <stdint.h>

#include "map.h"
#include "questdata.h"

#define MOVE_NONE	0
#define MOVE_LEFT	1
#define	MOVE_RIGHT	2
#define MOVE_DOWN	4
#define MOVE_UP		8
#define MAX_QUESTS  5

#define MAX_GAMEMESSAGE 4

class GameData
{
public:
    GameData();
    ~GameData();

    /*
    void SetSaveSlot(uint8_t slot);
    uint8_t GetSaveSlot() const;

    void SetSeed(const uint64_t m_seed);
    uint64_t GetSeed() const;
    */

    void SetPlayerWorldPosition(const int64_t x, const int64_t y);
    /*
    void SetPlayerWorldX(const int64_t x);
    int64_t GetPlayerWorldX() const;
    void SetPlayerWorldY(const int64_t y);
    int64_t GetPlayerWorldY() const;
    */

    bool WriteGameData(void *data) const;
    bool LoadGameData(void *data);

    void AddGameMessage(const char *message);
    int16_t GameMessageCount() const;

    Map &m_map;
    uint64_t m_ticks;
    char m_gamemessages[MAX_GAMEMESSAGE][20];
    uint64_t m_gamemessagedecay[MAX_GAMEMESSAGE];
    uint8_t m_saveslot;
    uint64_t m_seed;
    int64_t m_playerworldx;
    int64_t m_playerworldy;
    int8_t m_movedir;
    int8_t m_selectedmenu;
    uint32_t m_questscompleted;
    QuestData m_quests[MAX_QUESTS];

private:

};
