#include "questdata.h"
#include "gamedata.h"
#include "gameio.h"
#include "gameevent.h"
#include "questcache.h"
#include "outputstringstream.h"
#include "miscfuncs.h"
#include "generatortownname.h"
#include "wasmmath.h"

QuestData::QuestData()
{
    Reset();
}

QuestData::~QuestData()
{

}

void QuestData::Reset()
{
    m_flags=0;
    m_type=0;
    m_sourcex=0;
    m_sourcey=0;
    m_destx=0;
    m_desty=0;
    m_progress=0;
    for(int32_t i=0; i<6; i++)
    {
        m_data[i]=0;
    }
}

void QuestData::SetFlag(const uint16_t flag, const bool val)
{
    m_flags=(m_flags & ~flag) | (val ? flag : 0);
}

bool QuestData::GetFlag(const uint16_t flag) const
{
    return (m_flags & flag) == flag;
}

bool QuestData::GetActive() const
{
    return (m_flags & FLAG_ACTIVE)==FLAG_ACTIVE;
}

bool QuestData::GetSelected() const
{
    return (m_flags & FLAG_SELECTED)==FLAG_SELECTED;
}

void QuestData::SetActive(const bool active)
{
    SetFlag(FLAG_ACTIVE,active);
}

void QuestData::SetSelected(const bool selected)
{
    SetFlag(FLAG_SELECTED,selected);
}

void QuestData::SetHasSourceLocation(const bool hassourceloc)
{
    SetFlag(FLAG_SOURCELOC,hassourceloc);
}

bool QuestData::HasSourceLocation() const
{
    return GetFlag(FLAG_SOURCELOC);
}

void QuestData::SetTutorial(const bool tutorial)
{
    SetFlag(FLAG_TUTORIAL,tutorial);
}

bool QuestData::Tutorial() const
{
    return GetFlag(FLAG_TUTORIAL);
}

void QuestData::SetCompleted(const bool completed)
{
    SetFlag(FLAG_COMPLETED,completed);
}

bool QuestData::GetCompleted() const
{
    return GetFlag(FLAG_COMPLETED);
}

int64_t QuestData::GetCurrentTargetWorldX() const
{
    //TODO - different types of quests and their progress may have different targets
    switch(m_type)
    {
    case TYPE_KILLAREAMONSTERS:
        if(m_progress==m_data[0])   // killed all monsters - return to source for reward
        {
            return m_sourcex;
        }
        break;
    case TYPE_DELIVERY:
        if(m_progress==0 || GetCompleted()==true)
        {
            return m_sourcex;
        }
        break;
    }
    return m_destx;
}

int64_t QuestData::GetCurrentTargetWorldY() const
{
    //TODO - different types of quests and their progress may have different targets
    switch(m_type)
    {
    case TYPE_KILLAREAMONSTERS:
        if(m_progress==m_data[0])   // killed all monsters - return to source for reward
        {
            return m_sourcey;
        }
        break;
    case TYPE_DELIVERY:
        if(m_progress==0 || GetCompleted()==true)
        {
            return m_sourcey;
        }
        break;
    }
    return m_desty;
}

bool QuestData::WriteQuestData(void *data) const
{
    int64_t pos=0;
    uint8_t *p=(uint8_t *)data;

    GameIO::write_uint8_t(&p[pos],m_flags);
    pos+=1;
    GameIO::write_uint8_t(&p[pos],m_type);
    pos+=1;
    GameIO::write_uint32_t(&p[pos],m_sourcex);
    pos+=4;
    GameIO::write_uint32_t(&p[pos],m_sourcey);
    pos+=4;
    GameIO::write_uint32_t(&p[pos],m_destx);
    pos+=4;
    GameIO::write_uint32_t(&p[pos],m_desty);
    pos+=4;
    GameIO::write_uint8_t(&p[pos],m_progress);
    pos+=1;
    for(int32_t i=0; i<6; i++)
    {
        GameIO::write_uint8_t(&p[pos],m_data[i]);
        pos+=1;
    }
    return true;
}

bool QuestData::LoadQuestData(void *data)
{
    int64_t pos=0;
    const uint8_t *p=(uint8_t *)data;

    m_flags=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    m_type=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    m_sourcex=GameIO::read_uint32_t(&p[pos]);
    pos+=4;
    m_sourcey=GameIO::read_uint32_t(&p[pos]);
    pos+=4;
    m_destx=GameIO::read_uint32_t(&p[pos]);
    pos+=4;
    m_desty=GameIO::read_uint32_t(&p[pos]);
    pos+=4;
    m_progress=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    for(int32_t i=0; i<6; i++)
    {
        m_data[i]=GameIO::read_uint8_t(&p[pos]);
        pos+=1;
    }
    return true;
}

bool QuestData::HasTargetLocation() const
{
    switch(m_type)
    {
    case TYPE_TRAVELAREA:
    case TYPE_TRAVELLOCATION:
    case TYPE_KILLAREAMONSTERS:
    case TYPE_DELIVERY:
        return true;
        break;
    case TYPE_TRAVELDISTANCE:
    case TYPE_VISITANYTOWN:
    case TYPE_ACCEPTQUESTS:
        return false;
        break;
    }
    return false;
}

bool QuestData::HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param, const int8_t idx)
{
    if(GetActive()==true)
    {
        bool questcompleted=false;
        bool questmessage=true;
        switch(eventtype)
        {
        case EVENT_PLAYERMOVE:
            if(m_type==TYPE_TRAVELLOCATION && gamedata->m_playerworldx==m_destx && gamedata->m_playerworldy==m_desty)
            {
                questcompleted=true;
            }
            if(m_type==TYPE_TRAVELAREA && gamedata->m_map.ComputeDistanceSq(gamedata->m_playerworldx,gamedata->m_playerworldy,m_destx,m_desty)<=(static_cast<int32_t>(m_data[0])*static_cast<int32_t>(m_data[0])))
            {
                questcompleted=true;
            }
            if(m_type==TYPE_TRAVELDISTANCE && ++m_progress==m_data[0])
            {
                questcompleted=true;
            }
            // arrive at town and killed all monsters that was requested
            if(m_type==TYPE_KILLAREAMONSTERS && m_sourcex==gamedata->m_playerworldx && m_sourcey==gamedata->m_playerworldy && GetCompleted()==true)
            {
                questcompleted=true;
                questmessage=false;
            }
            // arrive at destination for delivery quest
            if(m_type==TYPE_DELIVERY && m_destx==gamedata->m_playerworldx && m_desty==gamedata->m_playerworldy && m_progress==1)
            {
                // check if item in inventory and remove
                int8_t slot=gamedata->QuestItemInInventorySlot(idx);
                if(slot>=0)
                {
                    questcompleted=true;
                    gamedata->m_inventory[slot].SetActive(false);
                }
            }
            break;
        case EVENT_ARRIVETOWN:
            if(m_type==TYPE_VISITANYTOWN)
            {
                questcompleted=true;
            }
            break;
        case EVENT_ACCEPTQUEST:
            if(m_type==TYPE_ACCEPTQUESTS && ++m_progress==m_data[0])
            {
                questcompleted=true;
            }
            break;
        case EVENT_KILLMOB:
            if(m_type==TYPE_KILLAREAMONSTERS)
            {
                if(GetCompleted()==false)
                {
                    // really shold be monster coord, but player is close enough
                    int64_t dx=_abs(gamedata->m_map.DeltaCoordinate(gamedata->m_playerworldx,m_destx));
                    int64_t dy=_abs(gamedata->m_map.DeltaCoordinate(gamedata->m_playerworldy,m_desty));
                    if(dx<=m_data[2] && dy<=m_data[2] && ++m_progress==m_data[0])
                    {
                        gamedata->AddGameMessage("Quest Completed!");
                        gamedata->AddGameMessage("Visit village for reward");
                        SetCompleted(true);
                    }
                }
            }
        }

        if(questcompleted==true)
        {
            SetActive(false);
            gamedata->m_questscompleted++;
            if(questmessage==true)
            {
                gamedata->AddGameMessage("Quest Completed!");
            }
            QuestCache::Instance().RemoveCache(m_sourcex,m_sourcey);

            if(m_data[1]!=0)
            {
                RandomMT rand;
                rand.Seed(gamedata->m_ticks);
                uint8_t itemtype=ItemData::RandomItemTypeFromDropType(rand,m_data[1]);
                if(itemtype!=ItemData::TYPE_NONE)
                {
                    ItemData item;
                    item.CreateRandom(rand,itemtype,ItemData::EQUIP_ANY,gamedata->m_playerlevel);
                    int8_t gidx=gamedata->ClearAndGetGroundSlot();
                    if(gidx>=0)
                    {
                        item.SetActive(true);
                        gamedata->m_grounditem[gidx]=item;
                        gamedata->m_grounditemlocation[gidx][0]=gamedata->m_playerworldx;
                        gamedata->m_grounditemlocation[gidx][1]=gamedata->m_playerworldy;
                        gamedata->AddGameMessage("Here is your reward");
                    }
                }
            }
        }

        return true;
    }

    return false;
}

void QuestData::GetQuestGiverDescription(char *desc, const int16_t len)
{
    if(desc)
    {
        OutputStringStream ostr;

        //snprintf(global::buff,global::buffsize,"The town of %s is being harassed by monsters.\n\nThey need you to clear the surrounding area.\n\nDo you accept the quest?",town);

        if(HasSourceLocation()==true)
        {
            char town[32];
            town[31]='\0';
            GenerateTownName(((static_cast<uint64_t>(m_sourcex) << 32) | static_cast<uint64_t>(m_sourcey)),town,31);
            ostr << "The villagers in " << town << " have need of your services.\n\n";
        }

        switch(m_type)
        {
        case TYPE_TRAVELLOCATION:
            ostr << "Travel to a nearby location marked on your map.";
            break;
        case TYPE_TRAVELAREA:
            ostr << "Travel close to the location marked on your map.";
            break;
        case TYPE_KILLAREAMONSTERS:
            ostr << "They need you to clear the surrounding area of monsters.";
            break;
        case TYPE_DELIVERY:
            ostr << "They need you to deliver an item to another village.";
            break;
        default:
            ostr << "????";
        }

        ostr << "\n\nAccept?";

        strncpy(desc,ostr.Buffer(),len-1);
        desc[len-1]='\0';
    }
}

void QuestData::GetDescription(char *desc, const int16_t len)
{
    if(desc)
    {
        OutputStringStream ostr;
        ostr << "Type : ";
        switch(m_type)
        {
        case TYPE_TRAVELAREA:
        case TYPE_TRAVELLOCATION:
        case TYPE_TRAVELDISTANCE:
            ostr << "Travel\n";
            break;
        case TYPE_DELIVERY:
            ostr << "Delivery\n";
            break;
        case TYPE_KILLAREAMONSTERS:
            ostr << "Clear Area\n";
            break;
        case TYPE_VISITANYTOWN:
            ostr << "Visit Town\n";
            break;
        case TYPE_ACCEPTQUESTS:
            ostr << "General\n";
            break;
        default:
            ostr << "????";
        }
        switch(m_type)
        {
        case TYPE_TRAVELAREA:
            ostr << "Go to the area marked on your map.\n";
            break;
        case TYPE_TRAVELLOCATION:
            ostr << "Go to the location marked on your map.\n";
            ostr << m_destx << "," << m_desty << "\n";
            break;
        case TYPE_TRAVELDISTANCE:
            ostr << "Move " << static_cast<int32_t>(m_data[0]) << " spaces in any direction\n";
            ostr << "Remaining " << (m_data[0]-m_progress) << "\n";
            break;
        case TYPE_VISITANYTOWN:
            ostr << "Visit any town.\n";
            break;
        case TYPE_ACCEPTQUESTS:
            ostr << "Accept " << static_cast<int32_t>(m_data[0]) << " quest" << (m_data[0]==1 ? "" : "s") << "\n";
            ostr << "Remaining " << (m_data[0]-m_progress) << "\n";
            break;
        case TYPE_KILLAREAMONSTERS:
            ostr << "Kill " << static_cast<int32_t>(m_data[0]) << " monsters near the marked location\n";
            ostr << "Remaining " << (m_data[0]-m_progress) << "\n";
            if((m_data[0]-m_progress)==0)
            {
                ostr << "Return to village for reward\n";
            }
            break;
        case TYPE_DELIVERY:
            ostr << "Deliver " << ItemData::GetShortDescription(m_data[0]) << " to the location marked on your map\n";
            if(m_progress==0)
            {
                ostr << "You need to pick up the " << ItemData::GetShortDescription(m_data[0]) << " from the village first\n";
            }
            else if(m_progress==1)
            {
                //ostr << "Deliver the " << ItemData::GetShortDescription(m_data[0]) << " to the destination\n";
            }
            /* - reward is given at destination location
            else if(m_progress==2)
            {
                ostr << "Return to village for reward\n";
            }
            */
            break;
        default:
            ostr << "????";
        }
        strncpy(desc,ostr.Buffer(),len-1);
        desc[len-1]='\0';
    }
}

bool QuestData::GetTargetLocationDistance(char *dist, const int16_t len, GameData *gamedata, const int64_t sourcex, const int64_t sourcey)
{
    if(dist && HasTargetLocation())
    {
        const float destdistsq=gamedata->m_map.ComputeDistanceSq(sourcex,sourcey,GetCurrentTargetWorldX(),GetCurrentTargetWorldY());
        OutputStringStream ostr;

        ostr << "Distance ";

        if(destdistsq<(10*10))
        {
            ostr << "Very Close";
        }
        else if(destdistsq<(50*50))
        {
            ostr << "Close";
        }
        else if(destdistsq<(500*200))
        {
            ostr << "Middling";
        }
        else if(destdistsq<(1000*1000))
        {
            ostr << "Somewhat Far";
        }
        else if(destdistsq<(2000*2000))
        {
            ostr << "Far";
        }
        else
        {
            ostr << "Very Far";
        }

        strncpy(dist,ostr.Buffer(),len-1);
        dist[len-1]='\0';

        return true;
    }
    else
    {
        return false;
    }
}

bool QuestData::GetTargetLocationDirection(char *direction, const int16_t len, GameData *gamedata, const int64_t sourcex, const int64_t sourcey)
{
    if(direction && HasTargetLocation())
    {
        // flip source and dest y because +y is down on map, but tan expects +y to be up
        const float destang=gamedata->m_map.ComputeAngle(sourcex,GetCurrentTargetWorldY(),GetCurrentTargetWorldX(),sourcey);
        OutputStringStream ostr;

        ostr << "Direction ";

        constexpr float mpi8=M_PI_4/2;
        if(destang!=destang)
        {
            ostr << "Here";
        }
        else if(destang>=mpi8 && destang<M_PI_4+mpi8)
        {
            ostr << "Northeast";
        }
        else if(destang>=M_PI_4+mpi8 && destang<M_PI_2+mpi8)
        {
            ostr << "North";
        }
        else if(destang>=M_PI_2+mpi8 && destang<M_PI_2+M_PI_4+mpi8)
        {
            ostr << "Northwest";
        }
        else if(destang>=M_PI_2+M_PI_4+mpi8 && destang<M_PI+mpi8)
        {
            ostr << "West";
        }
        else if(destang>=M_PI+mpi8 && destang<M_PI+M_PI_4+mpi8)
        {
            ostr << "Southwest";
        }
        else if(destang>=M_PI+M_PI_4+mpi8 && destang<M_PI+M_PI_2+mpi8)
        {
            ostr << "South";
        }
        else if(destang>=M_PI+M_PI_2+mpi8 && destang<M_PI+M_PI_2+M_PI_4+mpi8)
        {
            ostr << "Southeast";
        }
        else
        {
            ostr << "East";
        }

        strncpy(direction,ostr.Buffer(),len-1);
        direction[len-1]='\0';

        return true;
    }
    else
    {
        return false;
    }
}
