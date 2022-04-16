#include "gamedata.h"
#include "gameio.h"
#include "miscfuncs.h"
#include "game.h"
#include "gameevent.h"
#include "wasmmath.h"

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

bool GameData::WriteGameData(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;
    int32_t i;

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
    // 45 bytes

    for(i=0; i<MAX_QUESTS; i++)
    {
        m_quests[i].WriteQuestData(&p[pos]);
        pos+=QuestData::SaveDataLength();
    }
    // 125 bytes - total 170 bytes

    // save 5 closest mobs
    SortClosestMobs(m_playerworldx,m_playerworldy);
    for(i=0; i<5 && i<MAX_MOBS; i++)
    {
        m_mobs[i].WriteMobData(&p[pos]);
        pos+=Mob::SaveDataLength();
    }
    // 90 bytes - total 260 bytes

    // save inventory
    for(i=0; i<MAX_INVENTORY; i++)
    {
        m_inventory[i].WriteItemData(&p[pos]);
        pos+=ItemData::SaveDataLength();
    }
    // 120 bytes - total 380 bytes

    // save ground items
    for(i=0; i<MAX_GROUNDITEMS; i++)
    {
        // save offset loc in 2 bytes - 1 each x,y
        ItemData tempitem=m_grounditem[i];
        int8_t dx=0;
        int8_t dy=0;
        if(tempitem.GetActive()==true)
        {
            int64_t dx64=m_map.DeltaCoordinate(m_playerworldx,m_grounditemlocation[i][0]);
            int64_t dy64=m_map.DeltaCoordinate(m_playerworldy,m_grounditemlocation[i][1]);
            // if distance is too great we won't save this item
            if(_abs(dx64)>50 || _abs(dy64)>50)
            {
                tempitem.SetActive(false);
            }
            else
            {
                dx=dx64;
                dy=dy64;
            }
        }
        GameIO::write_int8_t(&p[pos],dx);
        pos+=1;
        GameIO::write_int8_t(&p[pos],dy);
        pos+=1;
        // save item data
        tempitem.WriteItemData(&p[pos]);
        pos+=ItemData::SaveDataLength();
    }
    // 40 bytes - total 420 bytes

    return true;
}

bool GameData::LoadGameData(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;
    int32_t i;

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

        m_map.SetSeed(m_seed);
        m_map.SetSize(mapsize); // need to set size before loading ground items so delta coords will calculate

        for(i=0; i<MAX_QUESTS; i++)
        {
            m_quests[i].LoadQuestData(&p[pos]);
            pos+=QuestData::SaveDataLength();
        }

        for(i=0; i<5 && i<MAX_MOBS; i++)
        {
            m_mobs[i].LoadMobData(&p[pos]);
            pos+=Mob::SaveDataLength();
        }

        for(i=5; i<MAX_MOBS; i++)
        {
            m_mobs[i].SetActive(false);
        }

        for(i=0; i<MAX_INVENTORY; i++)
        {
            m_inventory[i].LoadItemData(&p[pos]);
            pos+=ItemData::SaveDataLength();
        }

        // load ground items
        for(i=0; i<MAX_GROUNDITEMS; i++)
        {
            // load 2 bytes of x,y offset from player
            int8_t dx=0;
            int8_t dy=0;
            dx=GameIO::read_int8_t(&p[pos]);
            pos+=1;
            dy=GameIO::read_int8_t(&p[pos]);
            pos+=1;
            m_grounditemlocation[i][0]=m_map.WrapCoordinate(m_playerworldx+static_cast<int64_t>(dx));
            m_grounditemlocation[i][1]=m_map.WrapCoordinate(m_playerworldy+static_cast<int64_t>(dy));
            // load item data
            m_grounditem[i].LoadItemData(&p[pos]);
            pos+=ItemData::SaveDataLength();
        }

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
    for(int32_t i=0; i<MAX_GAMEMESSAGE; i++)
    {
        m_gamemessages[i][0]='\0';
        m_gamemessagedecay[i]=0;
    }
}

void GameData::AddGameMessage(const char *message)
{
    for(int32_t i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamemessages[i][0]=='\0')
        {
            strncpy(m_gamemessages[i],message,31);
            m_gamemessagedecay[i]=m_ticks+(60*5);
            return;
        }
    }
    // no open slots - remove the top message and move everything up 1 and add new message at end
    for(int32_t i=0; i<MAX_GAMEMESSAGE-1; i++)
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
    for(int32_t i=0; i<MAX_GAMEMESSAGE; i++)
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
    int32_t i;
    m_seed=seed;
    m_map.SetSeed(seed);
    m_map.SetSize(16384);
    m_selectedmenu=0;
    m_movedir=MOVE_NONE;
    m_playerworldx=0;
    m_playerworldy=0;
    m_playerlevel=1;
    m_playerexpnextlevel=Game::Instance().GetNextLevelExperience(m_playerlevel);
    m_playerhealth=GetPlayerMaxHealth();
    m_map.UpdateWorldPosition(m_playerworldx,m_playerworldy);
    m_questscompleted=0;

    for(i=0; i<MAX_QUESTS; i++)
    {
        m_quests[i].Reset();
    }

    for(i=0; i<MAX_MOBS; i++)
    {
        m_mobs[i].Reset();
    }

    for(i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        m_queuedevent[i]=EVENT_NONE;
    }

    for(i=0; i<MAX_INVENTORY; i++)
    {
        m_inventory[i].Reset();
    }

    for(i=0; i<MAX_GROUNDITEMS; i++)
    {
        m_grounditem[i].Reset();
    }

    // start quests
    
    QuestData *quest=&m_quests[0];
    quest->SetActive(true);
    quest->m_sourcex=0;
    quest->m_sourcey=0;
    quest->m_destx=0;
    quest->m_desty=0;
    quest->m_progress=0;
    quest->m_data[0]=50;
    quest->m_type=QuestData::TYPE_TRAVELDISTANCE;
    quest->SetHasSourceLocation(false);
    quest->SetTutorial(true);
    
    quest=&m_quests[1];
    quest->SetActive(true);
    quest->m_sourcex=0;
    quest->m_sourcey=0;
    quest->m_destx=0;
    quest->m_desty=0;
    quest->m_progress=0;
    quest->m_type=QuestData::TYPE_VISITANYTOWN;
    quest->SetHasSourceLocation(false);
    quest->SetTutorial(true);
    quest->m_data[1]=ItemData::TYPE_BOOT;
    quest->SetDropIsItemType(true);
    
    quest=&m_quests[2];
    quest->SetActive(true);
    quest->m_sourcex=0;
    quest->m_sourcey=0;
    quest->m_destx=0;
    quest->m_desty=0;
    quest->m_progress=0;
    quest->m_data[0]=5;
    quest->m_type=QuestData::TYPE_ACCEPTQUESTS;
    quest->SetHasSourceLocation(false);
    quest->SetTutorial(true);
    quest->m_data[1]=ItemData::TYPE_HELMET;
    quest->SetDropIsItemType(true);


    // basic sword

    /*
    m_inventory[0].SetActive(true);
    m_inventory[0].SetEquipped(true);
    m_inventory[0].SetEquipable(true);
    m_inventory[0].SetTemplate(1);
    m_inventory[0].m_data[0]=1; // damage
    */
    m_inventory[0].CreateFromTemplate(1,1);
    m_inventory[0].SetActive(true);
    m_inventory[0].SetEquipped(true);

    // basic shield

    /*
    m_inventory[1].SetActive(true);
    m_inventory[1].SetEquipped(true);
    m_inventory[1].SetEquipable(true);
    m_inventory[1].SetTemplate(19);
    m_inventory[1].m_data[0]=1; // armor
    */
    m_inventory[1].CreateFromTemplate(19,1);
    m_inventory[1].SetActive(true);
    m_inventory[1].SetEquipped(true);
    
    // basic armor

    /*
    m_inventory[2].SetActive(true);
    m_inventory[2].SetEquipped(true);
    m_inventory[2].SetEquipable(true);
    m_inventory[2].SetTemplate(42);
    m_inventory[2].m_data[0]=1; // armor
    */
    m_inventory[2].CreateFromTemplate(42,1);
    m_inventory[2].SetActive(true);
    m_inventory[2].SetEquipped(true);
    
    // basic gauntlets
    
    /*
    m_inventory[3].SetActive(true);
    m_inventory[3].SetEquipped(true);
    m_inventory[3].SetEquipable(true);
    m_inventory[3].SetTemplate(32);
    m_inventory[3].m_data[0]=1; // armor
    */
    m_inventory[3].CreateFromTemplate(32,1);
    m_inventory[3].SetActive(true);
    m_inventory[3].SetEquipped(true);
    
    // health potion
    
    /*
    m_inventory[4].SetActive(true);
    m_inventory[4].SetConsumable(true);
    m_inventory[4].SetTemplate(51);
    */
    m_inventory[4].CreateFromTemplate(51,1);
    m_inventory[4].SetActive(true);

    /*
    m_grounditem[0].Reset();
    m_grounditem[0].SetActive(true);
    m_grounditem[0].SetEquipable(true);
    m_grounditem[0].SetTemplate(52);
    m_grounditem[0].m_data[0]=1;
    m_grounditemlocation[0][0]=1;
    m_grounditemlocation[0][1]=1;

    m_grounditem[1]=m_grounditem[0];
    m_grounditem[1].SetTemplate(53);
    m_grounditemlocation[1][0]=2;
    m_grounditemlocation[1][1]=2;

    m_grounditem[2]=m_grounditem[0];
    m_grounditem[2].SetTemplate(54);
    m_grounditemlocation[2][0]=3;
    m_grounditemlocation[2][1]=3;

    m_grounditem[3]=m_grounditem[0];
    m_grounditem[3].SetTemplate(55);
    m_grounditemlocation[3][0]=4;
    m_grounditemlocation[3][1]=4;    
    */

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

    for(int32_t i=0; i<MAX_MOBS; i++)
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

bool GameData::FindWorldLocationTown(const int64_t cx, const int64_t cy, const int16_t rad, int64_t &townx, int64_t &towny) const
{
    for(int64_t r=0; r<rad; r++)
    {
        int32_t x;
        int32_t y;
        for(y=cy-r; y<=cy+r; y+=rad+rad)
        {
            for(x=cx-r; x<=cx+r; x++)
            {
                if(WorldLocationTown(m_map.WrapCoordinate(x),m_map.WrapCoordinate(y))==true)
                {
                    townx=m_map.WrapCoordinate(x);
                    towny=m_map.WrapCoordinate(y);
                    return true;
                }
            }
        }
        for(x=cx-r; x<=cx+r; x+=rad+rad)
        {
            for(y=cy-r; y<cy+r; y++)
            {
                if(WorldLocationTown(m_map.WrapCoordinate(x),m_map.WrapCoordinate(y))==true)
                {
                    townx=m_map.WrapCoordinate(x);
                    towny=m_map.WrapCoordinate(y);
                    return true;
                }               
            }
        }
    }
    return false;
}

bool GameData::HostileWithinArea(const int64_t tlx, const int64_t tly, const int64_t brx, const int64_t bry)
{
    for(int64_t y=tly; y<=bry; y++)
    {
        for(int64_t x=tlx; x<=brx; x++)
        {
            const int64_t wx=m_map.WrapCoordinate(x);
            const int64_t wy=m_map.WrapCoordinate(y);
            for(int32_t i=0; i<MAX_MOBS; i++)
            {
                if(m_mobs[i].GetActive()==true && m_mobs[i].GetHostile()==true && m_mobs[i].m_x==x && m_mobs[i].m_y==wy)
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
    Mob *m1;
    Mob *m2;
    for(int32_t i=0; i<MAX_MOBS-1; i++)
    {
        for(int32_t j=0; j<MAX_MOBS-1-i; j++)
        {
            m2=&m_mobs[j+1];
            //if(m_mobs[j+1].GetActive()==true)
            if(m2->GetActive()==true)
            {
                m1=&m_mobs[j];
                //td1=((x-m_mobs[j].m_x)*(x-m_mobs[j].m_x))+((y-m_mobs[j].m_y)*(y-m_mobs[j].m_y));
                td1=((x-m1->m_x)*(x-m1->m_x))+((y-m1->m_y)*(y-m1->m_y));
                //td2=((x-m_mobs[j+1].m_x)*(x-m_mobs[j+1].m_x))+((y-m_mobs[j+1].m_y)*(y-m_mobs[j+1].m_y));
                td2=((x-m2->m_x)*(x-m2->m_x))+((y-m2->m_y)*(y-m2->m_y));
                //if(m_mobs[j].GetActive()==false || td1>td2)
                if(m1->GetActive()==false || td1>td2)
                {
                    //tempmob=m_mobs[j];
                    tempmob=*m1;
                    //m_mobs[j]=m_mobs[j+1];
                    *m1=*m2;
                    //m_mobs[j+1]=tempmob;
                    *m2=tempmob;
                }
            }
        }
    }
}

bool GameData::PlaceMob(const int16_t mobidx, const int64_t x, const int64_t y, const int16_t mindist, const int16_t maxdist)
{
    RandomMT r;
    r.Seed(m_ticks);
    int16_t dx=0;
    int16_t dy=0;
    bool placementgood=false;
    int16_t trycount=0;

    do
    {
        placementgood=true;
        do
        {
            dx=(r.Next()%(maxdist+1));
            dy=(r.Next()%(maxdist+1));
        }while(dx<mindist && dy<mindist);   // only x or y needs to be more than mindist

        if(r.NextDouble()<0.5)
        {
            dx=-dx;
        }
        if(r.NextDouble()<0.5)
        {
            dy=-dy;
        }

        m_mobs[mobidx].m_x=m_map.WrapCoordinate(x+dx);
        m_mobs[mobidx].m_y=m_map.WrapCoordinate(y+dy);

        if(
            m_map.MoveBlocked(m_mobs[mobidx].m_x,m_mobs[mobidx].m_y,m_mobs[mobidx].m_x,m_mobs[mobidx].m_y)==true ||
            WorldLocationTown(m_mobs[mobidx].m_x,m_mobs[mobidx].m_y)==true ||
            WorldLocationOccupied(m_mobs[mobidx].m_x,m_mobs[mobidx].m_y,mobidx)==true
        )
        {
            placementgood=false;
        }

    }while(placementgood==false && ++trycount<20);
    
    return placementgood;
}

void GameData::RecycleMobs()
{
    int16_t alivecount=0;
    for(int32_t i=0; i<MAX_MOBS; i++)
    {
        if(m_mobs[i].GetActive()==true)
        {
            if(m_map.ComputeDistanceSq(m_mobs[i].m_x,m_mobs[i].m_y,m_playerworldx,m_playerworldy)>(30*30))
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
        for(int32_t i=0; i<MAX_MOBS; i++)
        {
            if(m_mobs[i].GetActive()==false)
            {
                m_mobs[i].Reset();
                RandomMT r;
                r.Seed(m_ticks);

                if(PlaceMob(i,m_playerworldx,m_playerworldy,10,20)==true)
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

bool GameData::CreateCloseMob(const int64_t x, const int64_t y, const int16_t mindist, const int16_t maxdist, const bool aggressive)
{
    for(int32_t i=0; i<MAX_MOBS; i++)
    {
        if(m_mobs[i].GetActive()==false)
        {
            m_mobs[i].Reset();
            RandomMT r;
            r.Seed(m_ticks);

            if(PlaceMob(i,m_playerworldx,m_playerworldy,mindist,maxdist)==true)
            {
                const uint8_t terrain=m_map.GetTerrainType(m_mobs[i].m_x,m_mobs[i].m_y,true);
                m_mobs[i].CreateRandom(r,m_playerlevel,terrain);

                m_mobs[i].SetActive(true);
                m_mobs[i].SetHostile(true);
                m_mobs[i].SetAggressive(aggressive);

                return true;
            }
        }
    }
    return false;
}

void GameData::RecycleGroundItems()
{
    for(int32_t i=0; i<MAX_GROUNDITEMS; i++)
    {
        if(m_grounditem[i].GetActive()==true && m_map.ComputeDistanceManhattan(m_playerworldx,m_playerworldy,m_grounditemlocation[i][0],m_grounditemlocation[i][1])>20)
        {
            m_grounditem[i].Reset();
        }
    }
}

void GameData::CheckAndAddQuestItems()
{
    for(int32_t i=0; i<MAX_QUESTS; i++)
    {
        if(m_quests[i].GetActive()==true && m_quests[i].m_sourcex==m_playerworldx && m_quests[i].m_sourcey==m_playerworldy)
        {
            if(m_quests[i].m_type==QuestData::TYPE_DELIVERY && m_quests[i].m_progress==0 && QuestItemInInventorySlot(i)<0)
            {
                ItemData item;
                item.Reset();
                item.ApplyTemplate(m_quests[i].m_data[0]);
                item.SetQuestItem(true);
                int8_t slot=0;
                slot=GetNextUnusedInventorySlot();
                if(slot>-1 && slot<MAX_INVENTORY)
                {
                    item.SetActive(true);
                    m_quests[i].m_progress=1;
                    m_inventory[slot]=item;
                    m_inventory[slot].m_data[0]=i;
                    AddGameMessage("Quest item retrieved");
                }
                else
                {
                    AddGameMessage("No free space!");
                }
            }
        }
    }
}

int8_t GameData::QuestItemInInventorySlot(const uint8_t questindex) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetQuestItem()==true && m_inventory[i].m_data[0]==questindex)
        {
            return i;
        }
    }
    return -1;
}

void GameData::QueueGameEvent(const int16_t gameevent, GameEventParam param)
{
    for(int32_t i=0; i<MAX_QUEUE_EVENTS; i++)
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
    for(int32_t i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        if(m_queuedevent[i]!=EVENT_NONE)
        {
            int32_t j;
            for(j=0; j<MAX_MOBS; j++)
            {
                if(m_mobs[j].GetActive()==true)
                {
                    m_mobs[j].HandleGameEvent(m_queuedevent[i],this,m_queuedeventparam[i],j);
                }
            }
            for(j=0; j<MAX_QUESTS; j++)
            {
                if(m_quests[j].GetActive()==true)
                {
                    m_quests[j].HandleGameEvent(m_queuedevent[i],this,m_queuedeventparam[i],j);
                }
            }
            if(m_queuedevent[i]==EVENT_PLAYERMOVE)
            {
                CheckAndAddQuestItems();
            }
            m_queuedevent[i]=EVENT_NONE;
        }
    }
}

void GameData::ClearGameEvents()
{
    for(int32_t i=0; i<MAX_QUEUE_EVENTS; i++)
    {
        m_queuedevent[i]=EVENT_NONE;
    }
}

int16_t GameData::GetPlayerMeleeAttack() const
{
    return GetPlayerMeleeAttack(ItemData::TYPE_ANY);
}

int16_t GameData::GetPlayerArmor() const
{
    return GetPlayerArmor(ItemData::TYPE_ANY);
}

int16_t GameData::GetPlayerMeleeAttack(const uint8_t itemtype) const
{
    //int16_t attack=m_playerlevel;   // base attack without weapon is just player level
    int16_t attack=_max(1,_sqrt(m_playerlevel));
    // TODO - calculate attack based on stats/equipment/bonuses
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (itemtype==ItemData::TYPE_ANY || itemtype==m_inventory[i].GetMeleeAttack()))
        {
            attack+=m_inventory[i].GetMeleeAttack();
        }
    }
    return _max(0,attack);
}

int16_t GameData::GetPlayerArmor(const uint8_t itemtype) const
{
    int16_t armor=0;
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (itemtype==ItemData::TYPE_ANY || itemtype==m_inventory[i].GetArmor()))
        {
            armor+=m_inventory[i].GetArmor();
        }
    }
    return _max(0,armor);
}

int16_t GameData::GetEquippedMeleeAttack(const uint8_t equipslot) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (equipslot==ItemData::EQUIP_ANY || equipslot==m_inventory[i].GetEquipSlot()))
        {
            return m_inventory[i].GetMeleeAttack();
        }
    }
    return -1;
}

int16_t GameData::GetEquippedArmor(const uint8_t equipslot) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (equipslot==ItemData::EQUIP_ANY || equipslot==m_inventory[i].GetEquipSlot()))
        {
            return m_inventory[i].GetArmor();
        }
    }
    return -1;
}

bool GameData::HasItemWithProperties(const uint8_t flags, const uint8_t templateflags, const bool equippedonly) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if((m_inventory[i].m_flags & flags)==flags && ((m_inventory[i].TemplateFlags() & templateflags) == templateflags) && (equippedonly==false || m_inventory[i].GetEquipped()==true))
        {
            return true;
        }
    }
    return false;
}

int16_t GameData::GetEquippedHealth(const uint8_t equipslot) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (equipslot==ItemData::EQUIP_ANY || equipslot==m_inventory[i].GetEquipSlot()))
        {
            return m_inventory[i].GetHealth(Game::Instance().GetLevelMaxHealth(m_playerlevel));
        }
    }
    return -1;
}

bool GameData::HasEquippedHealth(const uint8_t equipslot) const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && (equipslot==ItemData::EQUIP_ANY || equipslot==m_inventory[i].GetEquipSlot()) && m_inventory[i].GetAddHealth()==true)
        {
            return true;
        }
    }
    return false;
}

int32_t GameData::GetPlayerMaxHealth() const
{
    // calculate max health based on items equipped
    int16_t health=Game::Instance().GetLevelMaxHealth(m_playerlevel);
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==true && m_inventory[i].GetEquipped()==true && m_inventory[i].GetAddHealth()==true)
        {
            health+=m_inventory[i].GetHealth(Game::Instance().GetLevelMaxHealth(m_playerlevel));
        }
    }
    return health;
}

int8_t GameData::GetNextUnusedInventorySlot() const
{
    for(int32_t i=0; i<MAX_INVENTORY; i++)
    {
        if(m_inventory[i].GetActive()==false)
        {
            return i;
        }
    }
    return -1;
}

void GameData::ToggleInventoryEquipped(uint8_t idx)
{
    if(idx<MAX_INVENTORY && m_inventory[idx].GetActive()==true && m_inventory[idx].GetEquipable()==true)
    {
        if(m_inventory[idx].GetEquipped()==true)
        {
            m_inventory[idx].SetEquipped(false);
        }
        else
        {
            for(int32_t i=0; i<MAX_INVENTORY; i++)
            {
                if(i!=idx && m_inventory[i].GetEquipped()==true && m_inventory[idx].GetEquipSlot()==m_inventory[i].GetEquipSlot())
                {
                    m_inventory[i].SetEquipped(false);
                }
            }
            m_inventory[idx].SetEquipped(true);
        }
    }
    // max health may have changed based on what was equipped, so cap it
    if(m_playerhealth>GetPlayerMaxHealth())
    {
        m_playerhealth=GetPlayerMaxHealth();
    }
}

int8_t GameData::ClearAndGetGroundSlot()
{
    int64_t maxdist=0;
    int8_t maxslot=0;
    for(int32_t i=0; i<MAX_GROUNDITEMS; i++)
    {
        if(m_grounditem[i].GetActive()==false)
        {
            return i;
        }
        int64_t dist=m_map.ComputeDistanceSq(m_playerworldx,m_playerworldy,m_grounditemlocation[i][0],m_grounditemlocation[i][1]);
        if(dist>maxdist)
        {
            maxdist=dist;
            maxslot=i;
        }
    }
    m_grounditem[maxslot].SetActive(false);
    return maxslot;
}

void GameData::AddPlayerExperience(const int32_t exp)
{
    m_playerexpnextlevel-=exp;
    while(m_playerexpnextlevel<=0)
    {
        GameEventParam p;
        m_playerlevel++;
        AddGameMessage("You gained a level!");
        QueueGameEvent(EVENT_PLAYERLEVELUP,p);
        m_playerexpnextlevel=Game::Instance().GetNextLevelExperience(m_playerlevel)+m_playerexpnextlevel;
    }
}

void GameData::Consume(const uint8_t itemidx)
{
    //if(itemidx<MAX_INVENTORY && m_inventory[itemidx].GetActive()==true && m_inventory[itemidx].GetConsumable()==true)
    if(itemidx<MAX_INVENTORY)
    {
        ItemData *item=&m_inventory[itemidx];
        if(item->GetActive()==true && item->GetConsumable()==true)
        {
        //if(m_inventory[itemidx].GetType()==ItemData::TYPE_HEALTHPOTION && m_playerhealth<GetPlayerMaxHealth())
        if(item->GetType()==ItemData::TYPE_HEALTHPOTION && m_playerhealth<GetPlayerMaxHealth())
        {
            const int64_t maxhealth=GetPlayerMaxHealth();
            m_playerhealth=_min(maxhealth,m_playerhealth+(maxhealth/2));
            //m_inventory[itemidx].SetActive(false);
            item->SetActive(false);
        }
        }
    }
}
