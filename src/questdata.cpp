#include "questdata.h"
#include "gameio.h"

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
    m_flags=(m_flags & ~FLAG_ACTIVE) | (active ? FLAG_ACTIVE : 0);
}

void QuestData::SetSelected(const bool selected)
{
    m_flags=(m_flags & ~FLAG_SELECTED) | (selected ? FLAG_SELECTED : 0);
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
