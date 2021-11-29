#include "mob.h"

Mob::Mob():m_flags(0),m_type(0),m_x(0),m_y(0)
{

}

Mob::~Mob()
{

}

bool Mob::GetActive() const
{
    return (m_flags & FLAG_ACTIVE)==FLAG_ACTIVE;
}

void Mob::SetActive(const bool active)
{
    m_flags=(m_flags & ~FLAG_ACTIVE) | (active ? FLAG_ACTIVE : 0);
}

void Mob::Update(const int ticks, GameData *gamedata)
{

}
