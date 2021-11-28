#include "generatorquest.h"
#include "randommt.h"
#include "map.h"

void GenerateQuest(const uint64_t seed, const uint64_t wx, const uint64_t wy, QuestData &quest)
{
    RandomMT &r=RandomMT::Instance();
    r.Seed(seed);
    quest.Reset();
    quest.SetActive(true);
    quest.m_sourcex=wx;
    quest.m_sourcey=wy;

    quest.m_destx=Map::Instance().WrapCoordinate(wx+((r.Next()%40)-20));
    quest.m_desty=Map::Instance().WrapCoordinate(wy+((r.Next()%40)-20));

}
