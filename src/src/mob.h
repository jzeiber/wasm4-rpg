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
        FLAG_ACTIVE  = 0b00000001,
        FLAG_HOSTILE = 0b00000010
    };

    bool WriteMobData(void *data) const;
    bool LoadMobData(void *data);

    bool GetActive() const;
    void SetActive(const bool active);
    bool GetHostile() const;
    void SetHostile(const bool hostile);

    void SetJitterX(const int16_t jitter);
    void SetJitterY(const int16_t jitter);
    int16_t GetJitterX() const;
    int16_t GetJitterY() const;

    void SetQuestIndex(const int8_t questidx);  // set if monster is tied to quest
    int8_t GetQuestIndex() const;               // get if monster is tied to quest

    virtual void Update(const int ticks, Game *game);

    virtual bool HandleGameEvent(const int16_t eventtype, GameData *gamedata, GameEventParam param);

    virtual void CreateRandom(RandomMT &rand, const int16_t playerlevel, const uint8_t terraintype);

    static constexpr int16_t SaveDataLength()   { return 18; }

    uint8_t m_flags;
    uint8_t m_type;
    int16_t m_level;
    int16_t m_health;
    int32_t m_x;
    int32_t m_y;
    uint8_t m_data[4];  // m_data[3] lowest 4 bytes = jitter, highest 4 bytes = quest index

protected:

    void SetFlag(const uint16_t flag, const bool val);
    bool GetFlag(const uint16_t flag) const;

};
