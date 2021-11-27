#pragma once

class Mob
{
public:
    Mob();
    virtual ~Mob();

protected:
    int64_t m_x;
    int64_t m_y;
};