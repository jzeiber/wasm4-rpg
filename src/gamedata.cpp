#include "gamedata.h"
#include "gameio.h"
#include "miscfuncs.h"

GameData::GameData():m_map(Map::Instance()),m_ticks(0),m_saveslot(0),m_seed(0),m_playerworldx(0),m_playerworldy(0),m_movedir(MOVE_NONE),m_selectedmenu(0),m_questscompleted(0)
{

}

GameData::~GameData()
{

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

    p[pos]=0x01;
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
    if(p[pos]==0x01)
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

        for(int i=0; i<MAX_QUESTS; i++)
        {
            m_quests[i].LoadQuestData(&p[pos]);
            pos+=25;
        }

        m_map.SetSeed(m_seed);
        m_map.SetSize(mapsize);

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
