#include "generatorquest.h"
#include "randommt.h"
#include "map.h"

void GenerateQuest(const uint64_t seed, const uint64_t wx, const uint64_t wy, GameData *gamedata, QuestData &quest)
{
    RandomMT &r=RandomMT::Instance();
    r.Seed(seed);
    quest.Reset();
    quest.SetActive(true);
    quest.m_sourcex=wx;
    quest.m_sourcey=wy;

    const double rd=r.NextDouble();
    /*
    if(rd<0.1)
    {

    }
    else*/
    {
        // loop while position is too close to current position
        int16_t dx=0;
        int16_t dy=0;
        do
        {
            dx=(r.Next()%80)-40;
            dy=(r.Next()%80)-40;
        }while(((dx*dx)+(dy*dy))<100);
        
        quest.m_destx=Map::Instance().WrapCoordinate(wx+(dx));
        quest.m_desty=Map::Instance().WrapCoordinate(wy+(dy));

        //quest.m_type=QuestData::QuestType::TYPE_TRAVELLOCATION;
        quest.SetHasSourceLocation(true);
        quest.m_type=QuestData::QuestType::TYPE_TRAVELAREA;
        quest.m_data[0]=(r.Next()%5)+3;
    }

}
