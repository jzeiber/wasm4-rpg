#pragma once

#include <stdint.h>
#include "igameeventhandler.h"
#include "iupdatable.h"
#include "randommt.h"

class Game;

class Mob:public IGameEventHandler, public IUpdatable
{
public:
    Mob();
    virtual ~Mob();

    enum Flags
    {
        FLAG_ACTIVE     = 0b00000001,
        FLAG_HOSTILE    = 0b00000010,
        FLAG_AGGRESSIVE = 0b00000100,
        FLAG_TRAVELWATER= 0b00001000,
        FLAG_TRAVELLAND = 0b00010000
    };

    enum DropFlags
    {
        DROP_NONE       = 0b00000000,
        DROP_WEAPON     = 0b00000001,
        DROP_ARMOR      = 0b00000010,
        DROP_AMULET     = 0b00000100,
        DROP_RING       = 0b00001000,
        DROP_POTION     = 0b00010000
    };

    enum MobType
    {
        TYPE_BEAST1=0,
        TYPE_BEAST2,
        TYPE_BEAST3,
        TYPE_BEAST4,
        TYPE_SCORPION,
        TYPE_CRAB,
        TYPE_SEASERPENT1,
        TYPE_SEASERPENT2,
        TYPE_KRAKEN,
        TYPE_SPIDER1,
        TYPE_SPIDER2,
        TYPE_SPIDER3,
        TYPE_IMP,
        TYPE_MAX
    };

    void Reset();

    bool WriteMobData(void *data) const;
    bool LoadMobData(void *data);

    bool GetActive() const;
    void SetActive(const bool active);
    bool GetHostile() const;
    void SetHostile(const bool hostile);
    bool GetAggressive() const;
    void SetAggressive(const bool agressive);
    bool GetTravelWater() const;
    void SetTravelWater(const bool travelwater);
    bool GetTravelLand() const;
    void SetTravelLand(const bool travelland);

    uint8_t GetType() const;
    void SetType(const uint8_t type);

    int16_t GetSpriteIdxX() const;
    int16_t GetSpriteIdxY() const;

    void SetJitterX(const int16_t jitter);
    void SetJitterY(const int16_t jitter);
    int16_t GetJitterX() const;
    int16_t GetJitterY() const;

    void SetQuestIndex(const int8_t questidx);  // set if monster is tied to quest
    int8_t GetQuestIndex() const;               // get if monster is tied to quest

    int16_t GetMaxHealth() const;

    uint8_t GetDropFlags() const;

    virtual void Update(const int ticks, Game *game);

    virtual bool HandleGameEvent(const int16_t eventtype, GameData *gamedata, GameEventParam param);

    virtual bool CreateRandom(RandomMT &rand, const int16_t playerlevel, const uint8_t terraintype);

    static constexpr int16_t SaveDataLength()   { return 18; }

    uint8_t m_flags;
    uint8_t m_type;
    int16_t m_level;
    int16_t m_health;
    int32_t m_x;
    int32_t m_y;
    uint8_t m_data[4];  // m_data[3] lowest 4 bytes = jitter, highest 4 bytes = quest index

protected:

    struct MobData
    {
        uint8_t spriteidxx;
        uint8_t spriteidxy;
        uint8_t probability;        // random probability - all probabilities for type added together when creating random mobs
        uint8_t flags;
        uint8_t healthmult;         // health = maxlevelhealth * (healthmult / 128.0);
        uint8_t attackmult;         // attack multiplier (attackmult/32.0)
        uint8_t dropflags;
        uint8_t dropprobability;    // drop item probability (prob/255.0)
        uint8_t expmult;            // experience multiplier (expmult/64.0)
    };

    static constexpr struct MobData m_mobdata[]={
                            /*x,  y,prob, flags                             ,hlth,attk, drop flags                       ,dprb, exp*/
    /* TYPE_BEAST1       */ { 1,  2, 128, FLAG_TRAVELLAND                   ,  96,  96, DROP_WEAPON|DROP_ARMOR           ,  60, 64},
    /* TYPE_BEAST2       */ { 2,  2, 128, FLAG_TRAVELLAND                   ,  96,  96, DROP_WEAPON|DROP_ARMOR           ,  60, 64},
    /* TYPE_BEAST3       */ { 3,  2, 128, FLAG_TRAVELLAND                   ,  96,  96, DROP_WEAPON|DROP_ARMOR           ,  60, 64},
    /* TYPE_BEAST4       */ { 4,  2, 128, FLAG_TRAVELLAND                   , 128,  96, DROP_WEAPON|DROP_ARMOR           ,  60, 64},
    /* TYPE_SCORPION     */ { 8,  4, 128, FLAG_TRAVELLAND                   ,  64,  64, DROP_NONE                        ,  60, 32},
    /* TYPE_CRAB         */ { 9,  4, 128, FLAG_TRAVELLAND | FLAG_TRAVELWATER,  64,  64, DROP_NONE                        ,  60, 32},
    /* TYPE_SEASERPENT1  */ { 4,  8, 128, FLAG_TRAVELWATER                  ,  96,  96, DROP_NONE                        ,  60, 64},
    /* TYPE_SEASERPENT2  */ { 5,  8, 128, FLAG_TRAVELWATER                  ,  96,  96, DROP_NONE                        ,  60, 64},
    /* TYPE_KRAKEN       */ { 1,  8,  64, FLAG_TRAVELWATER                  , 192, 192, DROP_NONE                        ,  60, 96},
    /* TYPE_SPIDER1      */ {12,  4, 128, FLAG_TRAVELLAND                   ,  96,  48, DROP_NONE                        ,  60, 64},
    /* TYPE_SPIDER2      */ {13,  4, 128, FLAG_TRAVELLAND                   ,  96,  48, DROP_NONE                        ,  60, 64},
    /* TYPE_SPIDER3      */ {14,  4, 128, FLAG_TRAVELLAND                   ,  96,  48, DROP_NONE                        ,  60, 64},
    /* TYPE_IMP          */ {15,  4,  64, FLAG_TRAVELLAND                   , 192, 192, DROP_RING|DROP_AMULET|DROP_POTION,  60, 96} 
    };

    void SetFlag(const uint16_t flag, const bool val);
    bool GetFlag(const uint16_t flag) const;

    uint8_t RandomItemTypeFromDropType(RandomMT &rand, uint8_t droptype) const;

};
