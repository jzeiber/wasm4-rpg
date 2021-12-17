#include "statesettings.h"
#include "textprinter.h"
#include "font5x7.h"
#include "wasm4.h"
#include "game.h"
#include "spriteitem.h"
#include "settings.h"
#include "lz4blitter.h"

StateSettings::StateSettings():m_changestate(0),m_selected(0)
{

}

StateSettings::~StateSettings()
{

}

StateSettings &StateSettings::Instance()
{
    static StateSettings ss;
    return ss;
}

void StateSettings::StateChanged(const uint8_t prevstate, void *params)
{
    m_changestate=0;
    m_selected=0;
}

void StateSettings::Update(const int ticks, Game *game)
{
    if(m_changestate!=0)
    {
        game->ChangeState(m_changestate,nullptr);
    }
}

bool StateSettings::HandleInput(const Input *input)
{
    if(input->GamepadButtonPress(1,BUTTON_1)==true && m_selected==2)
    {
        m_changestate=Game::STATE_MAINMENU;
    }
    if(input->GamepadButtonPress(1,BUTTON_DOWN)==true)
    {
        if(m_selected<2)
        {
            m_selected++;
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_UP)==true)
    {
        if(m_selected>0)
        {
            m_selected--;
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_RIGHT)==true)
    {
        if(m_selected==0)
        {
            int16_t md=Settings::Instance().GetMoveDelay();
            md-=10;
            if(md<0)
            {
                md=0;
            }
            Settings::Instance().SetMoveDelay(md);
        }
        if(m_selected==1)
        {
            int16_t mr=Settings::Instance().GetMoveRepeat();
            mr-=1;
            if(mr<1)
            {
                mr=1;
            }
            Settings::Instance().SetMoveRepeat(mr);
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_LEFT)==true)
    {
        if(m_selected==0)
        {
            int16_t md=Settings::Instance().GetMoveDelay();
            md+=10;
            if(md>120)
            {
                md=120;
            }
            Settings::Instance().SetMoveDelay(md);
        }
        if(m_selected==1)
        {
            int16_t mr=Settings::Instance().GetMoveRepeat();
            mr+=1;
            if(mr>30)
            {
                mr=30;
            }
            Settings::Instance().SetMoveRepeat(mr);
        }
    }
    return true;
}

void StateSettings::Draw()
{
    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());

    tp.PrintCentered("Settings",SCREEN_SIZE/2,1,128,PALETTE_WHITE);

    int16_t ypos=14;
    tp.PrintCentered("Move Repeat Delay",SCREEN_SIZE/2,ypos,128,PALETTE_WHITE);
    ypos=24;
    DrawSlider(16,ypos,0,120,120-Settings::Instance().GetMoveDelay(),(m_selected==0 ? true : false));
    ypos+=16;
    tp.Print("Slow",20,ypos,128,PALETTE_WHITE);
    tp.Print("Fast",118,ypos,128,PALETTE_WHITE);

    ypos=54;
    tp.PrintCentered("Move Repeat Rate",SCREEN_SIZE/2,ypos,128,PALETTE_WHITE);
    ypos=64;
    DrawSlider(16,ypos,1,30,31-Settings::Instance().GetMoveRepeat(),(m_selected==1 ? true : false));
    ypos+=16;    
    tp.Print("Slow",20,ypos,128,PALETTE_WHITE);
    tp.Print("Fast",118,ypos,128,PALETTE_WHITE);

    ypos=104;
    tp.PrintCentered("Done",SCREEN_SIZE/2,ypos,128,(m_selected==2 ? PALETTE_GREEN : PALETTE_WHITE));

}

void StateSettings::DrawSlider(const int16_t x, const int16_t y, const int16_t min, const int16_t max, const int16_t val, const bool selected)
{
    *DRAW_COLORS=(selected==false ? PALETTE_WHITE << 4 : PALETTE_GREEN << 4);
    //blitSub(spriteitem,x,y,16,16,4*16,12*16,spriteitemWidth,spriteitemFlags);
    //blitSub(spriteitem,x+112,y,16,16,4*16,12*16,spriteitemWidth,spriteitemFlags|BLIT_FLIP_X);
    LZ4Blitter::Instance().Blit(x,y,16,16,4,12,spriteitemFlags);
    LZ4Blitter::Instance().Blit(x+112,y,16,16,4,12,spriteitemFlags|BLIT_FLIP_X);

    for(int16_t segment=1; segment<7; segment++)
    {
        //blitSub(spriteitem,x+(16*segment),y,16,16,5*16,12*16,spriteitemWidth,spriteitemFlags);
        LZ4Blitter::Instance().Blit(x+(16*segment),y,16,16,5,12,spriteitemFlags);
    }

    // slider range = 104 pixels (128-16)-(4+4)
    const float p=static_cast<float>(val-min)/static_cast<float>(max-min);
    *DRAW_COLORS=*DRAW_COLORS | PALETTE_BROWN;
    //blitSub(spriteitem,x+4+(104.0f*p),y,16,16,6*16,12*16,spriteitemWidth,spriteitemFlags);
    LZ4Blitter::Instance().Blit(x+4+(104.0*p),y,16,16,6,12,spriteitemFlags);

}
