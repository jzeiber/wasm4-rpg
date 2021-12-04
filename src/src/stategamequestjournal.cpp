#include "stategamequestjournal.h"

#include "stategamemap.h"
#include "game.h"
#include "printf.h"
#include "global.h"
#include "wasm4.h"
#include "textprinter.h"
#include "font5x7.h"
#include "spriteitem.h"
#include "generatortownname.h"
#include "wasmmath.h"
#include "miscfuncs.h"

StateGameQuestJournal::StateGameQuestJournal():m_gamedata(nullptr),m_changestate(-1),m_questidx(-1)
{

}

StateGameQuestJournal::~StateGameQuestJournal()
{

}

StateGameQuestJournal &StateGameQuestJournal::Instance()
{
    static StateGameQuestJournal qj;
    return qj;
}

void StateGameQuestJournal::StateChanged(const uint8_t prevstate, void *params)
{
    m_questidx=-1;
    if(params)
    {
        m_gamedata=((GameData *)params);
        for(int i=0; i<MAX_QUESTS; i++)
        {
            if(m_gamedata->m_quests[i].GetActive()==true)
            {
                m_questidx=i;
                i=MAX_QUESTS;
            }
        }
    }
    m_changestate=-1;
}

bool StateGameQuestJournal::HandleInput(const Input *input)
{
    /*
    int64_t dx=0;
    int64_t dy=0;
    if(input->GamepadButtonDown(1,BUTTON_RIGHT))
    {
        dx+=5;
        m_gamedata->m_movedir=MOVE_RIGHT;
    }
    if(input->GamepadButtonDown(1,BUTTON_LEFT))
    {
        dx-=5;
        m_gamedata->m_movedir=MOVE_LEFT;
    }
    if(input->GamepadButtonDown(1,BUTTON_UP))
    {
        dy-=5;
        m_gamedata->m_movedir=MOVE_UP;
    }
    if(input->GamepadButtonDown(1,BUTTON_DOWN))
    {
        dy+=5;
        m_gamedata->m_movedir=MOVE_DOWN;
    }
    if(dx!=0 || dy!=0)
    {
        m_x=m_gamedata->m_map.WrapCoordinate(m_x+dx);
        m_y=m_gamedata->m_map.WrapCoordinate(m_y+dy);
    }
    */

    if(input->GamepadButtonPress(1,BUTTON_LEFT))
    {
        for(int i=m_questidx-1; i>=0; i--)
        {
            if(m_gamedata->m_quests[i].GetActive()==true)
            {
                m_questidx=i;
                i=0;
            }
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_RIGHT))
    {
        for(int i=m_questidx+1; i<MAX_QUESTS; i++)
        {
            if(m_gamedata->m_quests[i].GetActive()==true)
            {
                m_questidx=i;
                i=MAX_QUESTS;
            }
        }
    }

    if(input->GamepadButtonPress(1,BUTTON_1) || input->GamepadButtonPress(1,BUTTON_2))
    {
        m_changestate=Game::STATE_GAMEOVERWORLD;
    }

    return true;
}

void StateGameQuestJournal::Update(const int ticks, Game *game)
{
    if(m_changestate>=0)
    {
        game->ChangeState(m_changestate,m_gamedata);
    }
}

void StateGameQuestJournal::Draw()
{
    *DRAW_COLORS=PALETTE_WHITE;
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());

    tp.PrintCentered("Quest Journal",SCREEN_SIZE/2,1,128);

    snprintf(global::buff,global::buffsize,"Completed : %u",m_gamedata->m_questscompleted);
    tp.PrintWrapped(global::buff,1,15,128,SCREEN_SIZE-2);

    tp.PrintCentered("Active Quests",SCREEN_SIZE/2,45,128);

    if(HavePreviousActiveQuest(m_questidx)==true)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        blitSub(spriteitem,2,40,16,16,(7*16),(15*16),spriteitemWidth,spriteitemFlags);
    }
    if(HaveNextActiveQuest(m_questidx)==true)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        blitSub(spriteitem,SCREEN_SIZE-(16+2),40,16,16,(5*16),(15*16),spriteitemWidth,spriteitemFlags);
    }
    if(HavePreviousActiveQuest(m_questidx)==false && HaveNextActiveQuest(m_questidx)==false && (m_questidx<0 || m_questidx>=MAX_QUESTS || m_gamedata->m_quests[m_questidx].GetActive()==false))
    {
        *DRAW_COLORS=PALETTE_WHITE;
        tp.PrintCentered("No Active Quests",SCREEN_SIZE/2,100,128);
    }
    
    if(m_questidx>=0 && m_questidx<MAX_QUESTS && m_gamedata->m_quests[m_questidx].GetActive()==true)
    {
        *DRAW_COLORS=PALETTE_WHITE;
        /*
        const int64_t qx=m_gamedata->m_quests[m_questidx].m_sourcex;
        const int64_t qy=m_gamedata->m_quests[m_questidx].m_sourcey;
        char temp[32];
        if(m_gamedata->m_quests[m_questidx].HasSourceLocation()==true)
        {
            GenerateTownName((qx << 32) | qy,temp,31);
            tp.PrintWrapped(temp,1,60,128,SCREEN_SIZE-2);
        }

        // flip source and dest y because +y is down on map, but tan expects +y to be up
        const float destang=m_gamedata->m_map.ComputeAngle(m_gamedata->m_playerworldx,m_gamedata->m_quests[m_questidx].m_desty,m_gamedata->m_quests[m_questidx].m_destx,m_gamedata->m_playerworldy);
        const float destdistsq=m_gamedata->m_map.ComputeDistanceSq(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_gamedata->m_quests[m_questidx].m_destx,m_gamedata->m_quests[m_questidx].m_desty);

        const float mpi8=M_PI_4/2;
        if(destang!=destang)
        {
            snprintf(temp,31,"Here");
        }
        else if(destang>=mpi8 && destang<M_PI_4+mpi8)
        {
            snprintf(temp,31,"Northeast");
        }
        else if(destang>=M_PI_4+mpi8 && destang<M_PI_2+mpi8)
        {
            snprintf(temp,31,"North");
        }
        else if(destang>=M_PI_2+mpi8 && destang<M_PI_2+M_PI_4+mpi8)
        {
            snprintf(temp,31,"Northwest");
        }
        else if(destang>=M_PI_2+M_PI_4+mpi8 && destang<M_PI+mpi8)
        {
            snprintf(temp,31,"West");
        }
        else if(destang>=M_PI+mpi8 && destang<M_PI+M_PI_4+mpi8)
        {
            snprintf(temp,31,"Southwest");
        }
        else if(destang>=M_PI+M_PI_4+mpi8 && destang<M_PI+M_PI_2+mpi8)
        {
            snprintf(temp,31,"South");
        }
        else if(destang>=M_PI+M_PI_2+mpi8 && destang<M_PI+M_PI_2+M_PI_4+mpi8)
        {
            snprintf(temp,31,"Southeast");
        }
        else
        {
            snprintf(temp,31,"East");
        }

        snprintf(global::buff,global::buffsize,"Direction %s",temp);
        tp.PrintWrapped(global::buff,1,70,128,SCREEN_SIZE-2);

        if(destdistsq<(10*10))
        {
            snprintf(temp,31,"Very Close");
        }
        else if(destdistsq<(50*50))
        {
            snprintf(temp,31,"Close");
        }
        else if(destdistsq<(500*200))
        {
            snprintf(temp,31,"Middling");
        }
        else if(destdistsq<(1000*1000))
        {
            snprintf(temp,31,"Somewhat Far");
        }
        else if(destdistsq<(2000*2000))
        {
            snprintf(temp,31,"Far");
        }
        else
        {
            snprintf(temp,31,"Very Far");
        }
        snprintf(global::buff,global::buffsize,"Distance %s",temp);
        tp.PrintWrapped(global::buff,1,80,128,SCREEN_SIZE-2);
        */

        m_gamedata->m_quests[m_questidx].GetDescription(global::buff,global::buffsize);
        tp.PrintWrapped(global::buff,1,60,global::buffsize,SCREEN_SIZE-2);
        
    }

}

bool StateGameQuestJournal::HavePreviousActiveQuest(const int8_t questidx) const
{
    for(int i=questidx-1; i>=0; i--)
    {
        if(m_gamedata->m_quests[i].GetActive()==true)
        {
            return true;
        }
    }
    return false;
}

bool StateGameQuestJournal::HaveNextActiveQuest(const int8_t questidx) const
{
    for(int i=questidx+1; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive()==true)
        {
            return true;
        }
    }
    return false;
}