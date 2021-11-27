#include "stategameoverworld.h"
#include "game.h"
#include "spritecharacter.h"
#include "spriteitem.h"
#include "generatortownname.h"
#include "generatorquest.h"
#include "global.h"
#include "printf.h"
#include "font5x7.h"
#include "settings.h"
#include "wasmmath.h"
#include "textprinter.h"

StateGameOverworld::StateGameOverworld():m_gamedata(nullptr),m_changestate(-1),m_showtowndialog(false),m_showingtowndialog(false),m_savegame(false),m_towndialogtype(0),m_lastmovetick(0),m_tick(0)
{

}

StateGameOverworld::~StateGameOverworld()
{

}

StateGameOverworld &StateGameOverworld::Instance()
{
    static StateGameOverworld go;
    return go;
}

void StateGameOverworld::StateChanged(const uint8_t prevstate, void *params)
{
    if(params)
    {
        m_gamedata=((GameData *)params);
    }
    m_tick=0;
    m_changestate=-1;
    m_lastmovetick=0;
    m_savegame=false;
}

bool StateGameOverworld::HandleInput(const Input *input)
{
    int64_t dx=0;
    int64_t dy=0;
    bool domove=false;
    if(input->GamepadButtonDown(1,BUTTON_RIGHT))
    {
        domove=false;
        if(input->GamepadButtonPress(1,BUTTON_RIGHT))
        {
            m_lastmovetick=m_tick;
            domove=true;
        }
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick)
        {
            domove=true;
        }
        if(domove==true)
        {
            dx++;
            m_gamedata->m_movedir=MOVE_RIGHT;
        }
    }
    if(input->GamepadButtonDown(1,BUTTON_LEFT))
    {
        domove=false;
        if(input->GamepadButtonPress(1,BUTTON_LEFT))
        {
            m_lastmovetick=m_tick;
            domove=true;
        }
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick)
        {
            domove=true;
        }
        if(domove==true)
        {
            dx--;
            m_gamedata->m_movedir=MOVE_LEFT;
        }
    }
    if(input->GamepadButtonDown(1,BUTTON_UP))
    {
        domove=false;
        if(input->GamepadButtonPress(1,BUTTON_UP))
        {
            m_lastmovetick=m_tick;
            domove=true;
        }
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick)
        {
            domove=true;
        }
        if(domove==true)
        {
            dy--;
            m_gamedata->m_movedir=MOVE_UP;
        }
    }
    if(input->GamepadButtonDown(1,BUTTON_DOWN))
    {
        domove=false;
        if(input->GamepadButtonPress(1,BUTTON_DOWN))
        {
            m_lastmovetick=m_tick;
            domove=true;
        }
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick)
        {
            domove=true;
        }
        if(domove==true)
        {
            dy++;
            m_gamedata->m_movedir=MOVE_DOWN;
        }
    }
    if(dx!=0 || dy!=0)
    {
        m_gamedata->m_playerworldx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+dx);
        m_gamedata->m_playerworldy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+dy);
        m_gamedata->m_map.UpdateWorldPosition(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy);

        // check for town
        Tile t=m_gamedata->m_map.GetTile(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy);
        if((t.GetFeature()==Tile::FEATURE_TOWN1) || (t.GetFeature()==Tile::FEATURE_TOWN2))
        {
            m_showtowndialog=true;
        }
    }

    if(input->GamepadButtonPress(1,BUTTON_1))
    {
        // choose menu item
        if(m_gamedata->m_selectedmenu==0)
        {
            m_savegame=true;
        }
        if(m_gamedata->m_selectedmenu==1)
        {
            m_changestate=Game::STATE_GAMEQUESTJOURNAL;
        }
        if(m_gamedata->m_selectedmenu==2)
        {
            m_changestate=Game::STATE_GAMEMAP;
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_2))
    {
        m_gamedata->m_selectedmenu++;
        if(m_gamedata->m_selectedmenu>3)
        {
            m_gamedata->m_selectedmenu=0;
        }
    }
    return true;
}

void StateGameOverworld::Update(const int ticks, Game *game)
{
    m_tick=game->GetTicks();
    if(m_changestate>=0)
    {
        switch(m_changestate)
        {
        case Game::STATE_GAMEMAP:
        case Game::STATE_GAMEQUESTJOURNAL:
            game->ChangeState(m_changestate,m_gamedata);
            break;
        default:
            trace("StateGameOverworld::Update changing to unknown state");
            game->ChangeState(m_changestate,nullptr);
        }
    }
    if(m_showtowndialog==true)
    {
        ModalDialog &m=ModalDialog::Instance();
        m.Reset();
        m.SetCustomFont(&Font5x7::Instance());
        m.SetTextWidth(144);
        const int8_t qi=GetNextAvailableQuestIndex();
        const bool existingquest=HaveExistingActiveQuest(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy);
        if(qi>=0 && existingquest==false)
        {
            m_tempquest.Reset();
            GenerateQuest(game->GetTicks(),m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_tempquest);
            char town[32]="\0";
            GenerateTownName((m_gamedata->m_playerworldx << 32) | (m_gamedata->m_playerworldy),town,31);
            snprintf(global::buff,global::buffsize,"The town of %s is being harassed by monsters.\n\nThey need you to clear the surrounding area.\n\nDo you accept the quest?",town);
            m.SetText(global::buff);
            m.SetOption(0,"Yes");
            m.SetOption(1,"No");
            m.SetSelectedOption(1);
            m_towndialogtype=0;
        }
        else if(existingquest==true)
        {
            m.SetText("You need to complete the existing quest from this location before accepting a new one.");
            m.SetOption(0,"Continue");
            m.SetSelectedOption(0);
            m_towndialogtype=1;
        }
        else
        {
            m.SetText("You need to complete one of your existing quests before accepting a new one.");
            m.SetOption(0,"Continue");
            m.SetSelectedOption(0);
            m_towndialogtype=2;
        }
        game->ShowModalDialog(&ModalDialog::Instance());
        m_showingtowndialog=true;
        m_showtowndialog=false;
    }
    if(m_showingtowndialog==true && ModalDialog::Instance().Shown()==false)
    {
        if(m_towndialogtype==0 && ModalDialog::Instance().SelectedOption()==0)
        {
            int8_t qi=GetNextAvailableQuestIndex();
            if(qi>=0)
            {
                m_gamedata->m_quests[qi]=m_tempquest;
            }
        }
        m_showingtowndialog=false;
        m_showtowndialog=false;
    }

    // check if we've completed any quests
    // right now just see if we've arrived close to destination and mark as completed
    for(int i=0; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive() && _abs(m_gamedata->m_quests[i].m_destx-m_gamedata->m_playerworldx)<4 && _abs(m_gamedata->m_quests[i].m_desty-m_gamedata->m_playerworldy)<4)
        {
            m_gamedata->m_questscompleted++;
            m_gamedata->m_quests[i].SetActive(false);
            m_gamedata->AddGameMessage("Quest Completed");
        }
    }

    // save game
    if(m_savegame)
    {
        Game::Instance().SaveGameData();
        m_savegame=false;
        m_gamedata->AddGameMessage("Game Saved");
    }

}

void StateGameOverworld::Draw()
{
    const int16_t viewsize=144;
    for(int64_t y=0; y<9; y++)
    {
        for(int64_t x=0; x<9; x++)
        {
            uint64_t wx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+x-4);
            uint64_t wy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+y-4);
            m_gamedata->m_map.DrawTile(wx,wy,x*16,y*16);

			if(wx==m_gamedata->m_playerworldx && wy==m_gamedata->m_playerworldy)
			{
				*DRAW_COLORS=(PALETTE_BROWN << 4);
				if(m_gamedata->m_map.GetTerrainType(wx,wy,true)==Tile::TERRAIN_LAND)
				{
                    *DRAW_COLORS=(PALETTE_WHITE << 4);
                    blitSub(spritecharacter,x*16,y*16,16,16,(3*16),(1*16),spritecharacterWidth,spritecharacterFlags);
                    *DRAW_COLORS=(PALETTE_BROWN << 4);
                    blitSub(spritecharacter,x*16,y*16,16,16,(3*16),(0*16),spritecharacterWidth,spritecharacterFlags);
				}
				else
				{
                    if(m_gamedata->m_movedir==MOVE_LEFT)
                    {
                        *DRAW_COLORS=(PALETTE_WHITE << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(10*16),(11*16),spritecharacterWidth,spritecharacterFlags|BLIT_FLIP_X);
                        *DRAW_COLORS=(PALETTE_BROWN << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(10*16),(10*16),spritecharacterWidth,spritecharacterFlags|BLIT_FLIP_X);
                    }
                    else if(m_gamedata->m_movedir==MOVE_UP || m_gamedata->m_movedir==MOVE_NONE)
                    {
                        *DRAW_COLORS=(PALETTE_WHITE << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(8*16),(11*16),spritecharacterWidth,spritecharacterFlags);                        
                        *DRAW_COLORS=(PALETTE_BROWN << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(8*16),(10*16),spritecharacterWidth,spritecharacterFlags);
                    }
                    else if(m_gamedata->m_movedir==MOVE_RIGHT)
                    {
                        *DRAW_COLORS=(PALETTE_WHITE << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(10*16),(11*16),spritecharacterWidth,spritecharacterFlags);
                        *DRAW_COLORS=(PALETTE_BROWN << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(10*16),(10*16),spritecharacterWidth,spritecharacterFlags);
                    }
                    else if(m_gamedata->m_movedir==MOVE_DOWN)
                    {
                        *DRAW_COLORS=(PALETTE_WHITE << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(9*16),(11*16),spritecharacterWidth,spritecharacterFlags);
                        *DRAW_COLORS=(PALETTE_BROWN << 4);
                        blitSub(spritecharacter,x*16,y*16,16,16,(9*16),(10*16),spritecharacterWidth,spritecharacterFlags);
                    }
				}
			}

        }
    }

    // draw quest arrows
    {
        for(int i=0; i<MAX_QUESTS; i++)
        {
            // TODO - check if quest is selected
            if(m_gamedata->m_quests[i].GetActive()==true)
            {
                *DRAW_COLORS=PALETTE_WHITE;
                // swap source and dest y because on map +y is down and for tan it's +y up
                //const float ang=m_gamedata->m_map.ComputeAngle(m_gamedata->m_playerworldx,m_gamedata->m_quests[i].m_desty,m_gamedata->m_quests[i].m_destx,m_gamedata->m_playerworldy);
                const float ang=m_gamedata->m_map.ComputeAngle(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_gamedata->m_quests[i].m_destx,m_gamedata->m_quests[i].m_desty);
                // check for nan
                if(ang==ang)
                {
                    const float dx=_cos(ang);
                    const float dy=_sin(ang);
                    //line((16*4)+8,(16*4)+8,((16*4)+8)+(dx*20),((16*4)+8)+(dy*20));

                    // draw arrow icon pointing to quest location
                    *DRAW_COLORS=(PALETTE_WHITE << 4);
                    if(_dabs(dx)>=_dabs(dy))
                    {
                        const float scale=_dabs(static_cast<double>(viewsize)/(dx*2.0));
                        const int64_t xpos=(dx<0 ? -4 : viewsize-16+4);   // offset sprite by 16 pixels to the left
                        const int64_t ypos=(viewsize/2)+(scale*dy)-8;  // offset 16x16 sprite by 8 pixels
                        const int16_t h=(ypos+16>viewsize ? 16-(ypos+16-viewsize) : 16);
                        if(dx<0)
                        {
                            blitSub(spriteitem,xpos,ypos,16,h,(7*16),(14*16),spriteitemWidth,spriteitemFlags);
                        }
                        else
                        {
                            blitSub(spriteitem,xpos,ypos,16,h,(5*16),(14*16),spriteitemWidth,spriteitemFlags);
                        }
                    }
                    else
                    {
                        const float scale=_dabs(static_cast<double>(viewsize)/(dy*2.0));
                        const int64_t xpos=(viewsize/2)+(scale*dx)-8;
                        const int64_t ypos=(dy<0 ? -4 : viewsize-16+4);
                        const int16_t w=(xpos+16>viewsize ? 16-(xpos+16-viewsize) : 16);
                        if(dy<0)
                        {
                            blitSub(spriteitem,xpos,ypos,w,16,(4*16),(14*16),spriteitemWidth,spriteitemFlags);
                        }
                        else
                        {
                            blitSub(spriteitem,xpos,ypos,w,16,(6*16),(14*16),spriteitemWidth,spriteitemFlags);
                        }
                    }
                }

            }
        }
    }

    // print messages
    int16_t tpos=viewsize-(m_gamedata->GameMessageCount()*8);
    for(int i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamedata->m_gamemessagedecay[i]>0 && m_gamedata->m_gamemessages[i])
        {
            TextPrinter tp;
            tp.SetCustomFont(&Font5x7::Instance());
            tp.Print(m_gamedata->m_gamemessages[i],10,tpos,20);
            tpos+=8;
        }
    }

    // bottom menu bar
	*DRAW_COLORS=m_gamedata->m_selectedmenu==0 ? (PALETTE_BROWN << 4 | PALETTE_WHITE) : (PALETTE_WHITE << 4);
	blitSub(spriteitem,0,SCREEN_SIZE-16,16,16,(7*16),(0*16),spriteitemWidth,BLIT_1BPP);	// save icon
    *DRAW_COLORS=m_gamedata->m_selectedmenu==1 ? (PALETTE_BROWN << 4 | PALETTE_WHITE) : (PALETTE_WHITE << 4);
	blitSub(spriteitem,16,SCREEN_SIZE-16,16,16,(2*16),(0*16),spriteitemWidth,BLIT_1BPP);	// journal icon
    *DRAW_COLORS=m_gamedata->m_selectedmenu==2 ? (PALETTE_BROWN << 4 | PALETTE_WHITE) : (PALETTE_WHITE << 4);
	blitSub(spriteitem,32,SCREEN_SIZE-16,16,16,(3*16),(0*16),spriteitemWidth,BLIT_1BPP);	// map icon
    *DRAW_COLORS=m_gamedata->m_selectedmenu==3 ? (PALETTE_BROWN << 4 | PALETTE_WHITE) : (PALETTE_WHITE << 4);
	blitSub(spriteitem,48,SCREEN_SIZE-16,16,16,(9*16),(0*16),spriteitemWidth,BLIT_1BPP);	// inventory icon

}

int8_t StateGameOverworld::GetNextAvailableQuestIndex() const
{
    for(int i=0; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive()==false)
        {
            return i;
        }
    }
    return -1;
}

bool StateGameOverworld::HaveExistingActiveQuest(const uint64_t wx, const uint64_t wy) const
{
    for(int i=0; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive()==true && m_gamedata->m_quests[i].m_sourcex==wx && m_gamedata->m_quests[i].m_sourcey==wy)
        {
            return true;
        }
    }
    return false;
}