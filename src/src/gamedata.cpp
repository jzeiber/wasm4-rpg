#include "gamedata.h"
#include "gameio.h"
#include "miscfuncs.h"
#include "game.h"
#include "gameevent.h"

GameData::GameData():m_map(Map::Instance()),m_ticks(0),m_saveslot(0),m_seed(0),m_playerworldx(0),m_playerworldy(0),m_playerlevel(0),m_playerexpnextlevel(0),m_movedir(MOVE_NONE),m_selectedmenu(0),m_questscompleted(0)
{
    for(int i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        m_queuedevent[i]=EVENT_NONE;
    }
}

GameData::~GameData()
{

}

constexpr uint8_t GameData::GameDataVersion()
{
    return 0x01;
}

void GameData::SetPlayerWorldPosition(const int64_t x, const int64_t y)
{
    m_playerworldx=x;
    m_playerworldy=y;
}

bool GameData::WriteGameData(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;

    p[pos]=GameDataVersion();
    pos+=1;
    GameIO::write_uint64_t(&p[pos],m_seed);
    pos+=8;
    GameIO::write_int64_t(&p[pos],m_playerworldx);
    pos+=8;
    GameIO::write_int64_t(&p[pos],m_playerworldy);
    pos+=8;
    GameIO::write_uint64_t(&p[pos],m_map.GetSize());
    pos+=8;
    GameIO::write_uint32_t(&p[pos],m_questscompleted);
    pos+=4;
    GameIO::write_int16_t(&p[pos],m_playerlevel);
    pos+=2;
    GameIO::write_int32_t(&p[pos],m_playerexpnextlevel);
    pos+=4;
    GameIO::write_int16_t(&p[pos],m_playerhealth);
    pos+=2;

    for(int i=0; i<MAX_QUESTS; i++)
    {
        m_quests[i].WriteQuestData(&p[pos]);
        pos+=QuestData::SaveDataLength();
    }

    // save 5 closest mobs
    SortClosestMobs(m_playerworldx,m_playerworldy);
    for(int i=0; i<5 && i<MAX_MOBS; i++)
    {
        m_mobs[i].WriteMobData(&p[pos]);
        pos+=Mob::SaveDataLength();
    }

    return true;
}

bool GameData::LoadGameData(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;
    if(p[pos]==GameDataVersion())
    {
        pos+=1;
        m_seed=GameIO::read_uint64_t(&p[pos]);
        pos+=8;
        m_playerworldx=GameIO::read_int64_t(&p[pos]);
        pos+=8;
        m_playerworldy=GameIO::read_int64_t(&p[pos]);
        pos+=8;
        uint64_t mapsize=GameIO::read_uint64_t(&p[pos]);
        pos+=8;
        m_questscompleted=GameIO::read_uint32_t(&p[pos]);
        pos+=4;
        m_playerlevel=GameIO::read_int16_t(&p[pos]);
        pos+=2;
        m_playerexpnextlevel=GameIO::read_int32_t(&p[pos]);
        pos+=4;
        m_playerhealth=GameIO::read_int16_t(&p[pos]);
        pos+=2;

        for(int i=0; i<MAX_QUESTS; i++)
        {
            m_quests[i].LoadQuestData(&p[pos]);
            pos+=QuestData::SaveDataLength();
        }

        for(int i=0; i<5 && i<MAX_MOBS; i++)
        {
            m_mobs[i].LoadMobData(&p[pos]);
            pos+=Mob::SaveDataLength();
        }

        for(int i=5; i<MAX_MOBS; i++)
        {
            m_mobs[i].SetActive(false);
        }

        m_map.SetSeed(m_seed);
        m_map.SetSize(mapsize);
        m_map.UpdateWorldPosition(m_playerworldx,m_playerworldy);

        ClearGameMessages();
        ClearGameEvents();

        m_selectedmenu=0;

        return true;
    }
    return false;
}

void GameData::ClearGameMessages()
{
    for(int i=0; i<MAX_GAMEMESSAGE; i++)
    {
        m_gamemessages[i][0]='\0';
        m_gamemessagedecay[i]=0;
    }
}

void GameData::AddGameMessage(const char *message)
{
    for(int i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamemessages[i][0]=='\0')
        {
            strncpy(m_gamemessages[i],message,31);
            m_gamemessagedecay[i]=m_ticks+(60*5);
            return;
        }
    }
    // no open slots - remove the top message and move everything up 1 and add new message at end
    for(int i=0; i<MAX_GAMEMESSAGE-1; i++)
    {
        if(m_gamemessages[i+1][0]!='\0')
        {
            strncpy(m_gamemessages[i],m_gamemessages[i+1],31);
            m_gamemessagedecay[i]=m_gamemessagedecay[i+1];
            m_gamemessages[i+1][0]=='\0';
            m_gamemessagedecay[i+1]=0;
        }
    }
    strncpy(m_gamemessages[MAX_GAMEMESSAGE-1],message,31);
    m_gamemessagedecay[MAX_GAMEMESSAGE-1]=m_ticks+(60*5);
}

int16_t GameData::GameMessageCount() const
{
    int16_t count=0;
    for(int i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamemessages[i][0]!='\0')
        {
            count++;
        }
    }
    return count;
}

void GameData::SetupNewGame(const uint64_t seed)
{
    m_seed=seed;
    m_map.SetSeed(seed);
    m_map.SetSize(16384);
    m_selectedmenu=0;
    m_movedir=MOVE_NONE;
    m_playerworldx=0;
    m_playerworldy=0;
    m_playerlevel=1;
    m_playerexpnextlevel=Game::Instance().GetNextLevelExperience(m_playerlevel);
    m_playerhealth=Game::Instance().GetLevelMaxHealth(m_playerlevel);
    m_map.UpdateWorldPosition(m_playerworldx,m_playerworldy);
    m_questscompleted=0;

    for(int i=0; i<MAX_QUESTS; i++)
    {
        m_quests[i].SetActive(false);
    }

    for(int i=0; i<MAX_MOBS; i++)
    {
        m_mobs[i].SetActive(false);
    }

    for(int i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        m_queuedevent[i]=EVENT_NONE;
    }

    // start quests
    m_quests[0].SetActive(true);
    m_quests[0].m_sourcex=0;
    m_quests[0].m_sourcey=0;
    m_quests[0].m_destx=0;
    m_quests[0].m_desty=0;
    m_quests[0].m_progress=0;
    m_quests[0].m_data[0]=50;
    m_quests[0].m_type=QuestData::TYPE_TRAVELDISTANCE;
    m_quests[0].SetHasSourceLocation(false);
    m_quests[0].SetTutorial(true);

    m_quests[1].SetActive(true);
    m_quests[1].m_sourcex=0;
    m_quests[1].m_sourcey=0;
    m_quests[1].m_destx=0;
    m_quests[1].m_desty=0;
    m_quests[1].m_progress=0;
    m_quests[1].m_type=QuestData::TYPE_VISITANYTOWN;
    m_quests[1].SetHasSourceLocation(false);
    m_quests[1].SetTutorial(true);

    m_quests[2].SetActive(true);
    m_quests[2].m_sourcex=0;
    m_quests[2].m_sourcey=0;
    m_quests[2].m_destx=0;
    m_quests[2].m_desty=0;
    m_quests[2].m_progress=0;
    m_quests[2].m_data[0]=5;
    m_quests[2].m_type=QuestData::TYPE_ACCEPTQUESTS;
    m_quests[2].SetHasSourceLocation(false);
    m_quests[2].SetTutorial(true);

    ClearGameMessages();
    ClearGameEvents();
    AddGameMessage("You have new quests");
}

bool GameData::WorldLocationOccupied(const int64_t worldx, const int64_t worldy, const int16_t ignoremobidx) const
{
    if(m_playerworldx==worldx && m_playerworldy==worldy)
    {
        return true;
    }

    for(int i=0; i<MAX_MOBS; i++)
    {
        if(i!=ignoremobidx && m_mobs[i].GetActive()==true && m_mobs[i].m_x==worldx && m_mobs[i].m_y==worldy)
        {
            return true;
        }
    }

    return false;
}

bool GameData::WorldLocationTown(const int64_t worldx, const int64_t worldy) const
{
    Tile t=m_map.GetTile(worldx,worldy);
    if(t.GetFeature()==Tile::FEATURE_TOWN1 || t.GetFeature()==Tile::FEATURE_TOWN2)
    {
        return true;
    }
    return false;
}

bool GameData::WorldLocationLand(const int64_t worldx, const int64_t worldy) const
{
    return m_map.GetTile(worldx,worldy).GetTerrainType()==Tile::TERRAIN_LAND;
}

bool GameData::WorldLocationWater(const int64_t worldx, const int64_t worldy) const
{
    return m_map.GetTile(worldx,worldy).GetTerrainType()==Tile::TERRAIN_WATER;
}

bool GameData::HostileWithinArea(const int64_t tlx, const int64_t tly, const int64_t brx, const int64_t bry)
{
    for(int64_t y=tly; y<=bry; y++)
    {
        for(int64_t x=tlx; x<=brx; x++)
        {
            const int64_t wx=m_map.WrapCoordinate(x);
            const int64_t wy=m_map.WrapCoordinate(y);
            for(int i=0; i<MAX_MOBS; i++)
            {
                if(m_mobs[i].GetActive()==true && m_mobs[i].m_x==x && m_mobs[i].m_y==wy)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void GameData::SortClosestMobs(const int32_t x, const int32_t y)
{
    int64_t td1=0;
    int64_t td2=0;
    Mob tempmob;
    for(int i=0; i<MAX_MOBS-1; i++)
    {
        for(int j=0; j<MAX_MOBS-1-i; j++)
        {
            if(m_mobs[j+1].GetActive()==true)
            {
                td1=((x-m_mobs[j].m_x)*(x-m_mobs[j].m_x))+((y-m_mobs[j].m_y)*(y-m_mobs[j].m_y));
                td2=((x-m_mobs[j+1].m_x)*(x-m_mobs[j+1].m_x))+((y-m_mobs[j+1].m_y)*(y-m_mobs[j+1].m_y));
                if(m_mobs[j].GetActive()==false || td1>td2)
                {
                    tempmob=m_mobs[j];
                    m_mobs[j]=m_mobs[j+1];
                    m_mobs[j+1]=tempmob;
                }
            }
        }
    }
}

void GameData::RecycleMobs()
{
    int16_t alivecount=0;
    for(int i=0; i<MAX_MOBS; i++)
    {
        if(m_mobs[i].GetActive()==true)
        {
            if(m_map.ComputeDistanceSq(m_mobs[i].m_x,m_mobs[i].m_y,m_playerworldx,m_playerworldy)>(50*50))
            {
                m_mobs[i].SetActive(false);
            }
            else
            {
                alivecount++;
            }
        }
    }
    if(alivecount<(MAX_MOBS*3)/4)
    {
        for(int i=0; i<MAX_MOBS; i++)
        {
            if(m_mobs[i].GetActive()==false)
            {
                m_mobs[i].Reset();
                RandomMT r;
                r.Seed(m_ticks);

                int16_t dx=0;
                int16_t dy=0;
                bool placementgood=false;
                int16_t trycount=0;
                // don't place on town/unmovable/player pos
                do
                {
                    placementgood=true;
                    do
                    {
                        dx=(r.Next()%40)-20;
                        dy=(r.Next()%40)-20;
                    }while(((dx*dx)+(dy*dy)) < 100);
                    
                    m_mobs[i].m_x=m_map.WrapCoordinate(m_playerworldx+dx);
                    m_mobs[i].m_y=m_map.WrapCoordinate(m_playerworldy+dy);

                    if(
                        m_map.MoveBlocked(m_mobs[i].m_x,m_mobs[i].m_y,m_mobs[i].m_x,m_mobs[i].m_y)==true ||
                        WorldLocationTown(m_mobs[i].m_x,m_mobs[i].m_y)==true ||
                        WorldLocationOccupied(m_mobs[i].m_x,m_mobs[i].m_y,i)==true
                    )
                    {
                        placementgood=false;
                    }

                }while(placementgood==false && ++trycount<20);

                if(trycount<20)
                {
                    const uint8_t terrain=m_map.GetTerrainType(m_mobs[i].m_x,m_mobs[i].m_y,true);
                    m_mobs[i].CreateRandom(r,m_playerlevel,terrain);

                    m_mobs[i].SetActive(true);
                    m_mobs[i].SetHostile(true);
                    m_mobs[i].SetAggressive(false); // ?? % chance to be aggressive?
                }

                break;
            }
        }
    }
}

void GameData::QueueGameEvent(const int16_t gameevent, GameEventParam param)
{
    for(int i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        if(m_queuedevent[i]==EVENT_NONE)
        {
            m_queuedevent[i]=gameevent;
            m_queuedeventparam[i]=param;
            break;
        }
    }
}

void GameData::DispatchGameEvents()
{
    for(int i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        if(m_queuedevent[i]!=EVENT_NONE)
        {
            for(int j=0; j<MAX_MOBS; j++)
            {
                if(m_mobs[j].GetActive()==true)
                {
                    m_mobs[j].HandleGameEvent(m_queuedevent[i],this,m_queuedeventparam[i]);
                }
            }
            for(int j=0; j<MAX_QUESTS; j++)
            {
                if(m_quests[j].GetActive()==true)
                {
                    m_quests[j].HandleGameEvent(m_queuedevent[i],this,m_queuedeventparam[i]);
                }
            }
            m_queuedevent[i]=EVENT_NONE;
        }
    }
}

void GameData::ClearGameEvents()
{
    for(int i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        m_queuedevent[i]=EVENT_NONE;
    }
}

int16_t GameData::GetPlayerMeleeAttack() const
{
    // TODO - calculate attack based on stats/equipment/bonuses
    return m_playerlevel*2;
}
