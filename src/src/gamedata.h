#pragma once

#include <stdint.h>
#include <stddef.h>

#include "map.h"
#include "questdata.h"
#include "mob.h"
#include "itemdata.h"
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
#define MAX_INVENTORY       20
#define MAX_GROUNDITEMS     5

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
    bool FindWorldLocationTown(const int64_t cx, const int64_t cy, const int16_t rad, int64_t &townx, int64_t &towny) const;

    int16_t GetPlayerMeleeAttack() const;
    int16_t GetPlayerMeleeAttack(const uint8_t itemtype) const;
    int16_t GetEquippedMeleeAttack(const uint8_t equipslot) const;
    int16_t GetPlayerArmor() const;
    int16_t GetPlayerArmor(const uint8_t itemtype) const;
    int16_t GetEquippedArmor(const uint8_t equipslot) const;
    int16_t GetEquippedHealth(const uint8_t equipslot) const;
    bool HasEquippedHealth(const uint8_t equipslot) const;
    int32_t GetPlayerMaxHealth() const;   // accounts for any items that may increase/decrease health


    void AddPlayerExperience(const int32_t exp);
    void Consume(const uint8_t itemidx);

    bool HostileWithinArea(const int64_t tlx, const int64_t tly, const int64_t brx, const int64_t bry); // returns true if hostile mob within area - coords should be unwrapped (tl must always be less or equal to br)

    int8_t GetNextUnusedInventorySlot() const;   // return <0 if no slots available
    void ToggleInventoryEquipped(uint8_t idx);
    int8_t ClearAndGetGroundSlot(); // gets next available ground slot, if none available, clears out most distant slot and returns it

    void RecycleMobs(); // remove distant mobs and add close ones if needed
    bool CreateCloseMob(const int64_t x, const int64_t y, const int16_t mindist, const int16_t maxdist, const bool aggressive);
    void RecycleGroundItems();  // remove items on ground too far away
    void CheckAndAddQuestItems();     // makes sure quest items are in inventory - adds to inventory if at source location and not in inventory
    int8_t QuestItemInInventorySlot(const uint8_t questindex) const;
    bool HasItemWithProperties(const uint8_t flags, const uint8_t templateflags, const bool equippedonly) const;

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
    ItemData m_inventory[MAX_INVENTORY];    // 20 items at 6 bytes each = 120 bytes
    ItemData m_grounditem[MAX_GROUNDITEMS];
    int32_t m_grounditemlocation[MAX_GROUNDITEMS][2];   // when we save gound item loc - only use 1 byte each for x,y, which will be x,y offset from current player location
    int16_t m_queuedevent[MAX_QUEUE_EVENTS];
    GameEventParam m_queuedeventparam[MAX_QUEUE_EVENTS];

private:

    void SortClosestMobs(const int32_t x, const int32_t y);  // sorts m_mobs so closest to location are first
    bool PlaceMob(const int16_t mobidx, const int64_t x, const int64_t y, const int16_t mindist, const int16_t maxdist);

};
