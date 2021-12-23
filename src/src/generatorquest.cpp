#include "generatorquest.h"
#include "randommt.h"
#include "map.h"

void GenerateQuest(const uint64_t seed, const uint64_t wx, const uint64_t wy, GameData *gamedata, QuestData &quest)
{
    bool trynext=false;
    RandomMT &r=RandomMT::Instance();
    r.Seed(seed);
    quest.Reset();
    quest.SetActive(true);
    quest.m_sourcex=wx;
    quest.m_sourcey=wy;

    const double rd=r.NextDouble();
    
    if(rd<0.5)
    {
        trynext=false;

        quest.m_type=QuestData::TYPE_DELIVERY;
        quest.SetHasSourceLocation(true);
        bool foundloc=false;
        int64_t townx=0;
        int64_t towny=0;
        constexpr int16_t rad=80;
        int16_t tries=0;
        do
        {
            int64_t tryx=wx+(r.Next()%1000)-500;
            int64_t tryy=wy+(r.Next()%1000)-500;
            if(gamedata->FindWorldLocationTown(tryx,tryy,rad,townx,towny)==true && townx!=wx && towny!=wy)
            {
                foundloc=true;
            }
        }while(foundloc==false && ++tries<50);

        if(foundloc==true)
        {
            ItemData item;
            item.CreateRandom(r,ItemData::TYPE_MISC,ItemData::EQUIP_NONE,1);

            quest.m_destx=townx;
            quest.m_desty=towny;
            quest.m_data[0]=item.m_template;
            quest.m_data[1]=ItemData::DROP_ARMOR|ItemData::DROP_WEAPON|ItemData::DROP_AMULET|ItemData::DROP_RING|ItemData::DROP_POTION;
            quest.m_progress=0;
        }
        else
        {
            trynext=true;
        }

    }
    else if(rd<=1.0 || trynext==true)
    {
        trynext=false;
        quest.m_type=QuestData::TYPE_KILLAREAMONSTERS;
        quest.SetHasSourceLocation(true);
        quest.m_destx=wx;
        quest.m_desty=wy;
        quest.m_data[0]=r.Next()%20+10; // count
        quest.m_data[1]=ItemData::DROP_ARMOR|ItemData::DROP_WEAPON|ItemData::DROP_AMULET|ItemData::DROP_RING|ItemData::DROP_POTION;
        quest.m_data[2]=30;     // distance
    }
    else if(trynext==true)
    {
        trynext=false;
        // loop while position is too close to current position
        int16_t dx=0;
        int16_t dy=0;
        do
        {
            dx=(r.Next()%100)-50;
            dy=(r.Next()%100)-50;
        }while(((dx*dx)+(dy*dy))<200);
        
        quest.m_destx=Map::Instance().WrapCoordinate(wx+(dx));
        quest.m_desty=Map::Instance().WrapCoordinate(wy+(dy));

        //quest.m_type=QuestData::QuestType::TYPE_TRAVELLOCATION;
        quest.SetHasSourceLocation(true);
        quest.m_type=QuestData::QuestType::TYPE_TRAVELAREA;
        quest.m_data[0]=(r.Next()%5)+3;
    }

}
