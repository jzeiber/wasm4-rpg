#pragma once

#include <stdint.h>
#include <stddef.h>

#include "map.h"
#include "questdata.h"
#include "mob.h"
#include "gameevent.h"

#define MOVE_NONE	0
#define MOVE_LEFT	1
#define	MOVE_RIGHT	2
#define MOVE_DOWN	4
#define MOVE_UP		8
#define MAX_QUESTS  5

#define MAX_GAMEMESSAGE     4
#define MAX_MOBS            20
#define MAX_QUEUE_EVENTS    5

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

    void SetupNewGame(const uint64_t seed);

    void SetPlayerWorldPosition(const int64_t x, const int64_t y);

    static constexpr uint8_t GameDataVersion();
    /*
    void SetPlayerWorldX(const int64_t x);
    int64_t GetPlayerWorldX() const;
    void SetPlayerWorldY(const int64_t y);
    int64_t GetPlayerWorldY() const;
    */

    bool WriteGameData(void *data);
    bool LoadGameData(void *data);

    void ClearGameMessages();
    void AddGameMessage(const char *message);
    int16_t GameMessageCount() const;

    void QueueGameEvent(const int16_t gameevent, GameEventParam param);
    void DispatchGameEvents();
    void ClearGameEvents();

    bool WorldLocationOccupied(const int64_t worldx, const int64_t worldy, const int16_t ignoremobidx=-1) const;
    bool WorldLocationTown(const int64_t worldx, const int64_t worldy) const;
    bool WorldLocationLand(const int64_t worldx, const int64_t worldy) const;
    bool WorldLocationWater(const int64_t worldx, const int64_t worldy) const;

    int16_t GetPlayerMeleeAttack() const;
    bool HostileWithinArea(const int64_t tlx, const int64_t tly, const int64_t brx, const int64_t bry); // returns true if hostile mob within area - coords should be unwrapped (tl must always be less or equal to br)

    void RecycleMobs(); // remove distant mobs and add close ones if needed

    Map &m_map;
    uint64_t m_ticks;
    char m_gamemessages[MAX_GAMEMESSAGE][32];       // 4 x 32 = 128 bytes
    uint64_t m_gamemessagedecay[MAX_GAMEMESSAGE];   // 4 x 8 = 32 bytes
    uint8_t m_saveslot;
    uint64_t m_seed;
    int64_t m_playerworldx;
    int64_t m_playerworldy;
    int16_t m_playerlevel;
    int32_t m_playerexpnextlevel;
    int16_t m_playerhealth;
    int8_t m_movedir;
    int8_t m_selectedmenu;
    uint32_t m_questscompleted;
    QuestData m_quests[MAX_QUESTS];     // 5 quests at 25 bytes each = 125 bytes
    Mob m_mobs[MAX_MOBS];               // 20 mobs at 18 bytes each = 360 bytes
    int16_t m_queuedevent[MAX_QUEUE_EVENTS];
    GameEventParam m_queuedeventparam[MAX_QUEUE_EVENTS];

private:

    void SortClosestMobs(const int32_t x, const int32_t y);  // sorts m_mobs so closest to location are first

};
