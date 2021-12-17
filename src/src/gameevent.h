#pragma once
#include <stdint.h>
#include <stddef.h>

class Mob;

enum GameEvent
{
    EVENT_NONE=0,
    EVENT_PLAYERMOVE,
    EVENT_PLAYERATTACK,
    EVENT_PLAYERLEVELUP,
    EVENT_PLAYERPICKUP,
    EVENT_KILLMOB,
    EVENT_ARRIVETOWN,
    EVENT_ACCEPTQUEST,
    EVENT_DECLINEQUEST,
    EVENT_MAX
};

struct GameEventParam
{
    GameEventParam():m_target(nullptr),m_int64(0)   { };
    ~GameEventParam()                               { };

    union
    {
        uintptr_t *m_target;
        Mob *m_targetmob;
    };
    union
    {
        uint8_t m_uint8[8];
        int8_t m_int8[8];
        uint16_t m_uint16[4];
        int16_t m_int16[4];
        uint32_t m_uint32[2];
        int32_t m_int32[2];
        uint64_t m_uint64;
        int64_t m_int64;
    };
};
