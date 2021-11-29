#include "gamedata.h"
#include "gameio.h"
#include "miscfuncs.h"
#include "game.h"

GameData::GameData():m_map(Map::Instance()),m_ticks(0),m_saveslot(0),m_seed(0),m_playerworldx(0),m_playerworldy(0),m_playerlevel(0),m_playerexpnextlevel(0),m_movedir(MOVE_NONE),m_selectedmenu(0),m_questscompleted(0)
{

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

bool GameData::WriteGameData(void *data) const
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
        pos+=25;
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
            pos+=25;
        }

        for(int i=0; i<MAX_MOBS; i++)
        {
            m_mobs[i].SetActive(false);
        }

        m_map.SetSeed(m_seed);
        m_map.SetSize(mapsize);
        m_map.UpdateWorldPosition(m_playerworldx,m_playerworldy);

        m_selectedmenu=0;

        return true;
    }
    return false;
}

void GameData::AddGameMessage(const char *message)
{
    for(int i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamemessages[i][0]=='\0')
        {
            strncpy(m_gamemessages[i],message,19);
            m_gamemessagedecay[i]=m_ticks+(60*5);
            return;
        }
    }
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

    //debug
    //m_playerhealth=m_playerhealth/4;
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
