#include "questcache.h"

QuestCache::QuestCache():m_cacheloc{{0,0},{0,0},{0,0},{0,0},{0,0}},m_cachetick{0,0,0,0,0}
{

}

QuestCache::~QuestCache()
{

}

QuestCache &QuestCache::Instance()
{
    static QuestCache qc;
    return qc;
}

bool QuestCache::GetCache(const int64_t x, const int64_t y, QuestData &qd)
{
    for(int i=0; i<5; i++)
    {
        if(m_cacheloc[i][0]==x && m_cacheloc[i][1]==y)
        {
            qd=m_cache[i];
            return true;
        }
    }
    return false;
}

void QuestCache::AddCache(const QuestData &qd, const int64_t x, const int64_t y, const uint64_t ticks)
{
    int16_t bestpos=0;
    for(int i=1; i<5; i++)
    {
        if(m_cachetick[i]<m_cachetick[bestpos])
        {
            bestpos=i;
        }
    }
    m_cache[bestpos]=qd;
    m_cacheloc[bestpos][0]=x;
    m_cacheloc[bestpos][1]=y;
    m_cachetick[bestpos]=ticks;
}

void QuestCache::RemoveCache(const int64_t x, const int64_t y)
{
    for(int i=0; i<5; i++)
    {
        if(m_cacheloc[i][0]==x && m_cacheloc[i][1]==y)
        {
            m_cache[i].SetActive(false);
            m_cacheloc[i][0]=0;
            m_cacheloc[i][1]=0;
            m_cachetick[i]=0;
        }
    }
}
