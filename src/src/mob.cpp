#include "mob.h"
#include "randommt.h"
#include "game.h"
#include "miscfuncs.h"
#include "gameio.h"
#include "wasmmath.h"

Mob::Mob():m_flags(0),m_type(0),m_level(0),m_health(0),m_x(0),m_y(0),m_data{0,0,0,0}
{

}

Mob::~Mob()
{

}

void Mob::Reset()
{
    m_flags=0;
    m_type=0;
    m_level=0;
    m_health=0;
    m_x=0;
    m_y=0;
    m_data[0]=0;
    m_data[1]=0;
    m_data[2]=0;
    m_data[3]=0;
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

bool Mob::GetAggressive() const
{
    return GetFlag(FLAG_AGGRESSIVE);
}

void Mob::SetAggressive(const bool aggressive)
{
    SetFlag(FLAG_AGGRESSIVE,aggressive);
}

bool Mob::GetTravelWater() const
{
    return GetFlag(FLAG_TRAVELWATER);
}

void Mob::SetTravelWater(const bool travelwater)
{
    SetFlag(FLAG_TRAVELWATER,travelwater);
}

bool Mob::GetTravelLand() const
{
    return GetFlag(FLAG_TRAVELLAND);
}

void Mob::SetTravelLand(const bool travelland)
{
    SetFlag(FLAG_TRAVELLAND,travelland);
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
    bool trymove=false;
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
            SetAggressive(true);
            gamedata->AddGameMessage("Attacked Mob");
        }
    }
    if(eventtype==EVENT_PLAYERATTACK || eventtype==EVENT_PLAYERMOVE)
    {
        trymove=true;
    }

    if(trymove==true)
    {
        RandomMT r;
        r.Seed(gamedata->m_ticks+m_x+m_y);
        const int16_t xdist=gamedata->m_map.ComputeDistanceCoord(m_x,gamedata->m_playerworldx);
        const int16_t ydist=gamedata->m_map.ComputeDistanceCoord(m_y,gamedata->m_playerworldy);
        // move toward player if within 4 spaces
        if((xdist>1 || ydist>1) && xdist<=4 && ydist<=4)
        {
            if(GetAggressive()==true || (r.Next()%3)==0)
            {
                int16_t dx=0;
                int16_t dy=0;
                float a=gamedata->m_map.ComputeAngle(m_x,m_y,gamedata->m_playerworldx,gamedata->m_playerworldy);
                if((m_x!=gamedata->m_playerworldx || r.Next()%5==0) && (a<M_PI_2 || a>(M_PI+M_PI_2)))
                {
                    dx=1;
                }
                else if((m_x!=gamedata->m_playerworldx || r.Next()%5==0) && (a>M_PI_2 && a<(M_PI+M_PI_2)))
                {
                    dx=-1;
                }
                // -/+ y flipped on map
                if((m_y!=gamedata->m_playerworldy || r.Next()%5==0) && a<M_PI)
                {
                    dy=1;
                }
                else if((m_y!=gamedata->m_playerworldy || r.Next()%5==0) && a>M_PI)
                {
                    dy=-1;
                }
                int64_t newx=gamedata->m_map.WrapCoordinate(m_x+dx);
                int64_t newy=gamedata->m_map.WrapCoordinate(m_y+dy);
                if(gamedata->m_map.MoveBlocked(m_x,m_y,newx,newy)==false && gamedata->WorldLocationOccupied(newx,newy)==false && gamedata->WorldLocationTown(newx,newy)==false &&
                ((GetTravelLand()==true && gamedata->WorldLocationLand(newx,newy)==true) || (GetTravelWater()==true && gamedata->WorldLocationWater(newx,newy)==true))
                )
                {
                    m_x=newx;
                    m_y=newy;
                }
                else if(newx!=m_x && gamedata->m_map.MoveBlocked(m_x,m_y,newx,m_y)==false && gamedata->WorldLocationOccupied(newx,m_y)==false && gamedata->WorldLocationTown(newx,m_y)==false &&
                ((GetTravelLand()==true && gamedata->WorldLocationLand(newx,m_y)==true) || (GetTravelWater()==true && gamedata->WorldLocationWater(newx,m_y)==true))
                )
                {
                    m_x=newx;
                }
                else if(newy!=m_y && gamedata->m_map.MoveBlocked(m_x,m_y,m_x,newy)==false && gamedata->WorldLocationOccupied(m_x,newy)==false && gamedata->WorldLocationTown(m_x,newy)==false &&
                ((GetTravelLand()==true && gamedata->WorldLocationLand(m_x,newy)==true) || (GetTravelWater()==true && gamedata->WorldLocationWater(m_x,newy)==true))
                )
                {
                    m_y=newy;
                }
            }
        }
        // TODO attack player
        else if(GetHostile()==true && xdist<=1 && ydist<=1)
        {
            // TODO - calculate damage done
            int16_t damage=1;
            if(gamedata->m_playerhealth >= damage)
            {
                gamedata->m_playerhealth-=damage;
            }
            else
            {
                gamedata->m_playerhealth=0;
            }
        }
    }

    return true;
}

void Mob::CreateRandom(RandomMT &rand, const int16_t playerlevel, const uint8_t terraintype)
{
    SetActive(true);
    // TODO - change distribution of level
    m_level=_max(1,playerlevel+(rand.Next()%3)-1);
    
    //set random type for terrain
    {
        m_type=0;
        int32_t total=0;
        for(int i=0; i<MobType::TYPE_MAX; i++)
        {
            if(
                ((m_mobdata[i].flags & FLAG_TRAVELLAND)==FLAG_TRAVELLAND && terraintype==Tile::TERRAIN_LAND) || 
                ((m_mobdata[i].flags & FLAG_TRAVELWATER)==FLAG_TRAVELWATER && terraintype==Tile::TERRAIN_WATER)
            )
            {
                total+=m_mobdata[i].probability;
            }
        }
        int32_t val=rand.Next()%total;
        int32_t current=0;
        for(int i=0; i<MobType::TYPE_MAX; i++)
        {
            if(
                ((m_mobdata[i].flags & FLAG_TRAVELLAND)==FLAG_TRAVELLAND && terraintype==Tile::TERRAIN_LAND) || 
                ((m_mobdata[i].flags & FLAG_TRAVELWATER)==FLAG_TRAVELWATER && terraintype==Tile::TERRAIN_WATER)
            )
            {
                current+=m_mobdata[i].probability;
                if(current>val)
                {
                    m_type=i;
                    break;
                }
            }
        }
    }

    // multiply health by factor for type
    m_health=GetMaxHealth();
    m_flags=m_mobdata[m_type].flags;
}

uint8_t Mob::GetType() const
{
    return m_type;
}

void Mob::SetType(const uint8_t type)
{
    if(type>=0 && type<MobType::TYPE_MAX)
    {
        m_type=type;
    }
}

int16_t Mob::GetSpriteIdxX() const
{
    if(m_type>=0 && m_type<MobType::TYPE_MAX)
    {
        return m_mobdata[m_type].spriteidxx;
    }
    return 0;
}

int16_t Mob::GetSpriteIdxY() const
{
    if(m_type>=0 && m_type<MobType::TYPE_MAX)
    {
        return m_mobdata[m_type].spriteidxy;
    }
    return 0;
}

int16_t Mob::GetMaxHealth() const
{
    if(m_type>=0 && m_type<MobType::TYPE_MAX)
    {
        return _max(1,static_cast<float>(Game::Instance().GetLevelMaxHealth(m_level))*(static_cast<float>(m_mobdata[m_type].healthmult)/128.0));
    }
    return 0;
}
