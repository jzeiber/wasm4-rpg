#pragma once

#include <stdint.h>

class GameData;

class Mob
{
public:
    Mob();
    virtual ~Mob();

    enum Flags
    {
        FLAG_ACTIVE = 0b00000001,
    };

    bool GetActive() const;
    void SetActive(const bool active);

    virtual void Update(const int ticks, GameData *gamedata);

    uint8_t m_flags;
    uint8_t m_type;
    int32_t m_x;
    int32_t m_y;
    uint8_t m_data[6];

protected:

};
