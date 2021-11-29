#pragma once

#include "questdata.h"

// cache a few quests so that when we revisit the same location they offer the same quest
class QuestCache
{
public:
    QuestCache();
    ~QuestCache();

    static QuestCache &Instance();

    bool GetCache(const int64_t x, const int64_t y, QuestData &qd);
    void AddCache(const QuestData &qd, const int64_t x, const int64_t y, const uint64_t ticks);
    void RemoveCache(const int64_t x, const int64_t y);

private:
    QuestData m_cache[5];
    int64_t m_cacheloc[5][2];
    uint64_t m_cachetick[5];
};
