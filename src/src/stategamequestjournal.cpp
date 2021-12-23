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
#include "lz4blitter.h"

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
        for(int32_t i=0; i<MAX_QUESTS; i++)
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
    if(input->GamepadButtonPress(1,BUTTON_LEFT))
    {
        for(int32_t i=m_questidx-1; i>=0; i--)
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
        for(int32_t i=m_questidx+1; i<MAX_QUESTS; i++)
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
    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());

    tp.PrintCentered("Quest Journal",SCREEN_SIZE/2,1,128,PALETTE_WHITE);

    snprintf(global::buff,global::buffsize,"Completed : %u",m_gamedata->m_questscompleted);
    tp.PrintWrapped(global::buff,1,15,128,SCREEN_SIZE-2,PALETTE_WHITE);

    tp.PrintCentered("Active Quests",SCREEN_SIZE/2,45,128,PALETTE_WHITE);

    if(HavePreviousActiveQuest(m_questidx)==true)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        //blitSub(spriteitem,2,40,16,16,(7*16),(15*16),spriteitemWidth,spriteitemFlags);
        LZ4Blitter::Instance().Blit(2,40,16,16,7,1,spriteitemFlags);
    }
    if(HaveNextActiveQuest(m_questidx)==true)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        //blitSub(spriteitem,SCREEN_SIZE-(16+2),40,16,16,(5*16),(15*16),spriteitemWidth,spriteitemFlags);
        LZ4Blitter::Instance().Blit(SCREEN_SIZE-(16+2),40,16,16,5,1,spriteitemFlags);
    }
    if(HavePreviousActiveQuest(m_questidx)==false && HaveNextActiveQuest(m_questidx)==false && (m_questidx<0 || m_questidx>=MAX_QUESTS || m_gamedata->m_quests[m_questidx].GetActive()==false))
    {
        tp.PrintCentered("No Active Quests",SCREEN_SIZE/2,100,128,PALETTE_WHITE);
    }
    
    if(m_questidx>=0 && m_questidx<MAX_QUESTS && m_gamedata->m_quests[m_questidx].GetActive()==true)
    {
        m_gamedata->m_quests[m_questidx].GetDescription(global::buff,global::buffsize);
        int16_t linecount=tp.PrintWrapped(global::buff,1,60,global::buffsize,SCREEN_SIZE-2,PALETTE_WHITE);
        int16_t ypos=60+(linecount*10);

        if(m_gamedata->m_quests[m_questidx].GetTargetLocationDirection(global::buff,global::buffsize,m_gamedata,m_gamedata->m_playerworldx,m_gamedata->m_playerworldy)==true)
        {
            tp.Print(global::buff,1,ypos,128,PALETTE_WHITE);
            ypos+=10;
        }
        if(m_gamedata->m_quests[m_questidx].GetTargetLocationDistance(global::buff,global::buffsize,m_gamedata,m_gamedata->m_playerworldx,m_gamedata->m_playerworldy)==true)
        {
            tp.Print(global::buff,1,ypos,128,PALETTE_WHITE);
            ypos+=10;
        }

    }

}

bool StateGameQuestJournal::HavePreviousActiveQuest(const int8_t questidx) const
{
    for(int32_t i=questidx-1; i>=0; i--)
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
    for(int32_t i=questidx+1; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive()==true)
        {
            return true;
        }
    }
    return false;
}