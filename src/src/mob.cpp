#include "mob.h"
#include "randommt.h"
#include "game.h"
#include "miscfuncs.h"
#include "gameio.h"

Mob::Mob():m_flags(0),m_type(0),m_level(0),m_health(0),m_x(0),m_y(0),m_data{0,0,0,0}
{

}

Mob::~Mob()
{

}

void Mob::SetFlag(const uint16_t flag, const bool val)
{
    m_flags=(m_flags & ~flag) | (val ? flag : 0);
}

bool Mob::GetFlag(const uint16_t flag) const
{
    return (m_flags & flag) == flag;
}

bool Mob::GetActive() const
{
    return GetFlag(FLAG_ACTIVE);
}

void Mob::SetActive(const bool active)
{
    SetFlag(FLAG_ACTIVE,active);
}

bool Mob::GetHostile() const
{
    return GetFlag(FLAG_HOSTILE);
}

void Mob::SetHostile(const bool hostile)
{
    SetFlag(FLAG_HOSTILE,hostile);
}

bool Mob::WriteMobData(void *data) const
{
    int64_t pos=0;
    uint8_t *p=(uint8_t *)data;

    GameIO::write_uint8_t(&p[pos],m_flags);
    pos+=1;
    GameIO::write_uint8_t(&p[pos],m_type);
    pos+=1;
    GameIO::write_int16_t(&p[pos],m_level);
    pos+=2;
    GameIO::write_int16_t(&p[pos],m_health);
    pos+=2;
    GameIO::write_int32_t(&p[pos],m_x);
    pos+=4;
    GameIO::write_int32_t(&p[pos],m_y);
    pos+=4;
    for(int i=0; i<4; i++)
    {
        GameIO::write_uint8_t(&p[pos],m_data[i]);
        pos+=1;
    }
    return true;
}

bool Mob::LoadMobData(void *data)
{
    int64_t pos=0;
    const uint8_t *p=(uint8_t *)data;

    m_flags=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    m_type=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    m_level=GameIO::read_int16_t(&p[pos]);
    pos+=2;
    m_health=GameIO::read_int16_t(&p[pos]);
    pos+=2;
    m_x=GameIO::read_int32_t(&p[pos]);
    pos+=4;
    m_y=GameIO::read_int32_t(&p[pos]);
    pos+=4;
    for(int i=0; i<4; i++)
    {
        m_data[i]=GameIO::read_uint8_t(&p[pos]);
        pos+=1;
    }
    return true;
}

void Mob::Update(const int ticks, Game *game)
{
    RandomMT rand;
    rand.Seed(game->GetTicks() | m_x << 8 | m_y << 8);
    if((game->GetTicks()+m_x)%20==0)
    {
        SetJitterX((rand.Next()%3)-1);
        SetJitterY((rand.Next()%3)-1);
    }

}

void Mob::SetJitterX(const int16_t jitter)
{
    m_data[3]=(m_data[3] & ~(0x03 << 2)) | (((jitter+1) & 0x03) << 2);
}

void Mob::SetJitterY(const int16_t jitter)
{
    m_data[3]=(m_data[3] & ~(0x03)) | ((jitter+1) & 0x03);
}

int16_t Mob::GetJitterX() const
{
    return ((m_data[3] >> 2) & 0x03)-1;
}

int16_t Mob::GetJitterY() const
{
    return (m_data[3] & 0x03)-1;
}

void Mob::SetQuestIndex(const int8_t questidx)
{
    if(questidx>=0 && questidx<MAX_QUESTS)
    {
        m_data[3]=(m_data[3] & ~(0xf0)) | ((questidx+1) << 4);
    }
}

int8_t Mob::GetQuestIndex() const
{
    return ((m_data[3] & 0xf0) >> 4)-1;
}

bool Mob::HandleGameEvent(const int16_t eventtype, GameData *gamedata, GameEventParam param)
{
    if(eventtype==EVENT_PLAYERATTACK && param.m_targetmob==this)
    {
        if(m_health<param.m_int16[0])
        {
            SetActive(false); 
            gamedata->AddGameMessage("Killed Mob");
            // - TODO queue game event - add quest index if set in param
        }
        else
        {
            m_health-=param.m_int16[0]; 
            gamedata->AddGameMessage("Attacked Mob");
        }
    }
    return true;
}

void Mob::CreateRandom(RandomMT &rand, const int16_t playerlevel, const uint8_t terraintype)
{
    SetActive(true);
    // TODO - change distribution of level
    m_level=_max(1,playerlevel+(rand.Next()%3)-1);
    // TODO - set random type for terrain
    // TODO - multiple health by factor for type
    m_health=Game::Instance().GetLevelMaxHealth(m_level);
}
