#include "questdata.h"
#include "gamedata.h"
#include "gameio.h"
#include "gameevent.h"
#include "questcache.h"
#include "outputstringstream.h"
#include "miscfuncs.h"
#include "generatortownname.h"

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
    for(int i=0; i<6; i++)
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

int64_t QuestData::GetCurrentTargetWorldX() const
{
    //TODO - different types of quests and their progress may have different targets
    return m_destx;
}

int64_t QuestData::GetCurrentTargetWorldY() const
{
    //TODO - different types of quests and their progress may have different targets
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
    for(int i=0; i<6; i++)
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
    for(int i=0; i<6; i++)
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

bool QuestData::HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param)
{
    if(GetActive()==true)
    {
        bool questcompleted=false;
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
        }

        if(questcompleted==true)
        {
            SetActive(false);
            gamedata->m_questscompleted++;
            gamedata->AddGameMessage("Quest Completed!");
            QuestCache::Instance().RemoveCache(m_sourcex,m_sourcey);
        }

        return true;
    }

    return false;
}

void QuestData::GetQuestGiverDescription(char *desc, const int16_t len)
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
    default:
        ostr << "????";
    }

    strncpy(desc,ostr.Buffer(),len-1);
}

void QuestData::GetDescription(char *desc, const int16_t len)
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
    default:
        ostr << "????";
    }
    strncpy(desc,ostr.Buffer(),len-1);
}
