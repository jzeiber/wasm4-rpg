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
#include "questcache.h"
#include "gameevent.h"
#include "drawfuncs.h"
#include "lz4blitter.h"
#include "outputstringstream.h"

StateGameOverworld::StateGameOverworld():m_gamedata(nullptr),m_cursormode(MODE_MOVE),m_cursorx(0),m_cursory(0),m_changestate(-1),m_showtowndialog(false),m_showingtowndialog(false),m_showexitdialog(false),m_showingexitdialog(false),m_savegame(false),m_towndialogtype(0),m_lastmovetick(0),m_lastrepeattick(0),m_tick(0)
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
    m_lastrepeattick=0;
    m_savegame=false;
    m_cursormode=MODE_MOVE;
    m_cursorx=0;
    m_cursory=0;
}

bool StateGameOverworld::HandleInput(const Input *input)
{
    int64_t dx=0;
    int64_t dy=0;
    bool domove=false;
    bool repeat=false;
    if(input->GamepadButtonDown(1,BUTTON_RIGHT))
    {
        domove=false;
        if(input->GamepadButtonPress(1,BUTTON_RIGHT))
        {
            m_lastmovetick=m_tick;
            domove=true;
        }
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick && m_lastrepeattick+Settings::Instance().GetMoveRepeat()<=m_tick)
        {
            domove=true;
            repeat=true;
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
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick && m_lastrepeattick+Settings::Instance().GetMoveRepeat()<=m_tick)
        {
            domove=true;
            repeat=true;
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
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick && m_lastrepeattick+Settings::Instance().GetMoveRepeat()<=m_tick)
        {
            domove=true;
            repeat=true;
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
        else if(m_lastmovetick+Settings::Instance().GetMoveDelay()<=m_tick && m_lastrepeattick+Settings::Instance().GetMoveRepeat()<=m_tick)
        {
            domove=true;
            repeat=true;
        }
        if(domove==true)
        {
            dy++;
            m_gamedata->m_movedir=MOVE_DOWN;
        }
    }
    if(dx!=0 || dy!=0)
    {
        bool domove=true;
        const int64_t newx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+dx);
        const int64_t newy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+dy);

        if(repeat==true)
        {
            m_lastrepeattick=m_tick;
        }

        if(m_gamedata->m_map.MoveBlocked(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,newx,newy)==true)
        {
            domove=false;
        }

        // check for monster and attack
        for(int i=0; i<MAX_MOBS; i++)
        {
            if(m_gamedata->m_mobs[i].GetActive()==true && m_gamedata->m_mobs[i].m_x==newx && m_gamedata->m_mobs[i].m_y==newy)
            {
                domove=false;
                if(m_gamedata->m_mobs[i].GetHostile()==true)
                {
                    GameEventParam ge;
                    ge.m_targetmob=&m_gamedata->m_mobs[i];
                    ge.m_int16[0]=m_gamedata->GetPlayerMeleeAttack();    // damage
                    m_gamedata->QueueGameEvent(EVENT_PLAYERATTACK,ge);
                }
            }
        }

        if(domove==true)
        {
            m_gamedata->m_playerworldx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+dx);
            m_gamedata->m_playerworldy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+dy);
            m_gamedata->m_map.UpdateWorldPosition(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy);

            m_gamedata->QueueGameEvent(EVENT_PLAYERMOVE,GameEventParam());

            // check for town
            Tile t=m_gamedata->m_map.GetTile(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy);
            if((t.GetFeature()==Tile::FEATURE_TOWN1) || (t.GetFeature()==Tile::FEATURE_TOWN2))
            {
                m_showtowndialog=true;
                m_gamedata->QueueGameEvent(EVENT_ARRIVETOWN,GameEventParam());
            }
        }
    }

    if(input->GamepadButtonPress(1,BUTTON_1))
    {
        // choose menu item
        if(m_gamedata->m_selectedmenu==OPTION_SAVE)
        {
            m_savegame=true;
        }
        if(m_gamedata->m_selectedmenu==OPTION_JOURNAL)
        {
            m_changestate=Game::STATE_GAMEQUESTJOURNAL;
        }
        if(m_gamedata->m_selectedmenu==OPTION_MAP)
        {
            m_changestate=Game::STATE_GAMEMAP;
        }
        if(m_gamedata->m_selectedmenu==OPTION_REST)
        {
            if(m_gamedata->HostileWithinArea(m_gamedata->m_playerworldx-4,m_gamedata->m_playerworldy-4,m_gamedata->m_playerworldx+4,m_gamedata->m_playerworldy+4)==false)
            {
                // TODO - potential to interrupt with mob (rest in town no chance of interrupt)
                RandomMT rand;
                rand.Seed(m_tick);
                if(m_gamedata->m_playerhealth==m_gamedata->GetPlayerMaxHealth())
                {
                    m_gamedata->AddGameMessage("Health already full");
                }
                else if(m_gamedata->WorldLocationTown(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy)==true || m_gamedata->HasItemWithProperties(0,ItemData::TEMPLATE_RESTFULSLEEP,true) || rand.Next()%5>0)
                {
                    m_gamedata->AddGameMessage("Rested");
                    m_gamedata->m_playerhealth=m_gamedata->GetPlayerMaxHealth();
                }
                else
                {
                    //interrupted by mob
                    m_gamedata->AddGameMessage("Interrupted");
                    // TODO - add 1-3 mob(s) near player
                    double r=rand.NextDouble();
                    m_gamedata->CreateCloseMob(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,2,4,true);
                    if(r<0.25)
                    {
                        m_gamedata->CreateCloseMob(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,3,4,true);
                    }
                    if(r<0.1)
                    {
                        m_gamedata->CreateCloseMob(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,3,4,true);
                    }
                }
            }
            else
            {
                m_gamedata->AddGameMessage("You can't rest now");
            }
        }
        if(m_gamedata->m_selectedmenu==OPTION_CHARACTER)
        {
            m_changestate=Game::STATE_GAMECHARACTERSHEET;
        }
        if(m_gamedata->m_selectedmenu==OPTION_INVENTORY)
        {
            m_changestate=Game::STATE_GAMEINVENTORY;
        }
        /*
        if(m_gamedata->m_selectedmenu==OPTION_INVENTORY)
        {
            m_gamedata->AddGameMessage("Not Implemented Yet");
        }
        */
        if(m_gamedata->m_selectedmenu==OPTION_PICKUP)
        {
            int8_t itemidx=-1;
            for(int i=0; i<MAX_GROUNDITEMS; i++)
            {
                if(m_gamedata->m_grounditem[i].GetActive()==true && m_gamedata->m_grounditemlocation[i][0]==m_gamedata->m_playerworldx && m_gamedata->m_grounditemlocation[i][1]==m_gamedata->m_playerworldy)
                {
                    itemidx=i;
                    break;
                }
            }
            if(itemidx>=0)
            {
                const int8_t invidx=m_gamedata->GetNextUnusedInventorySlot();
                if(invidx>=0)
                {
                    m_gamedata->m_inventory[invidx]=m_gamedata->m_grounditem[itemidx];
                    m_gamedata->m_grounditem[itemidx].Reset();
                    m_gamedata->AddGameMessage("Picked up item");
                    GameEventParam p;
                    p.m_target=(uintptr_t *)&m_gamedata->m_inventory[invidx];
                    m_gamedata->QueueGameEvent(EVENT_PLAYERPICKUP,p);
                }
                else
                {
                    m_gamedata->AddGameMessage("No free space!");
                }
            }
            else
            {
                m_gamedata->AddGameMessage("No item to pick up");
            }
        }
        if(m_gamedata->m_selectedmenu==OPTION_HOME)
        {
            m_showexitdialog=true;
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_2))
    {
        m_gamedata->m_selectedmenu++;
        if(m_gamedata->m_selectedmenu>=OPTION_MAX)
        {
            m_gamedata->m_selectedmenu=0;
        }
    }
    return true;
}

void StateGameOverworld::Update(const int ticks, Game *game)
{
    m_tick=game->GetTicks();

    if(game->GetGameData().m_playerhealth<=0)
    {
        m_changestate=Game::STATE_GAMEOVER;
    }

    if(m_changestate>=0)
    {
        switch(m_changestate)
        {
        case Game::STATE_MAINMENU:
            game->ChangeState(m_changestate,game);
            break;
        case Game::STATE_GAMEMAP:
        case Game::STATE_GAMEQUESTJOURNAL:
        case Game::STATE_GAMECHARACTERSHEET:
        case Game::STATE_GAMEINVENTORY:
        case Game::STATE_GAMEOVER:
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
            if(QuestCache::Instance().GetCache(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_tempquest)==false)
            {
                GenerateQuest(game->GetTicks(),m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_gamedata,m_tempquest);
                QuestCache::Instance().AddCache(m_tempquest,m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_gamedata->m_ticks);
            }
            /*
            char town[32]="\0";
            GenerateTownName((m_gamedata->m_playerworldx << 32) | (m_gamedata->m_playerworldy),town,31);
            snprintf(global::buff,global::buffsize,"The town of %s is being harassed by monsters.\n\nThey need you to clear the surrounding area.\n\nDo you accept the quest?",town);
            m.SetText(global::buff);*/
            m_tempquest.GetQuestGiverDescription(global::buff,global::buffsize);
            m.SetText(global::buff);
            m.SetOption(0,"Yes");
            m.SetOption(1,"No");
            m.SetSelectedOption(1);
            m_towndialogtype=0;
        }
        else if(existingquest==true)
        {
            /*
            m.SetText("You need to complete the existing quest from this location before accepting a new one.");
            m.SetOption(0,"Continue");
            m.SetSelectedOption(0);
            m_towndialogtype=1;
            */
            m_showtowndialog=false;
        }
        else
        {
            m.SetText("You need to complete one of your existing quests before accepting a new one.");
            m.SetOption(0,"Continue");
            m.SetSelectedOption(0);
            m_towndialogtype=2;
        }
        if(m_showtowndialog==true)
        {
            game->ShowModalDialog(&ModalDialog::Instance());
            m_showingtowndialog=true;
            m_showtowndialog=false;
        }
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
            m_gamedata->QueueGameEvent(EVENT_ACCEPTQUEST,GameEventParam());
            m_gamedata->CheckAndAddQuestItems();  // add quest item if we've just accepted quest involving item
        }
        else
        {
            m_gamedata->QueueGameEvent(EVENT_DECLINEQUEST,GameEventParam());
        }
        m_showingtowndialog=false;
        m_showtowndialog=false;
    }
    if(m_showexitdialog==true)
    {
        ModalDialog &m=ModalDialog::Instance();
        m.Reset();
        m.SetCustomFont(&Font5x7::Instance());
        m.SetTextWidth(144);
        m.SetText("Are you sure you want to quit?  Any unsaved progress will be lost.");
        m.SetOption(0,"Yes");
        m.SetOption(1,"No");
        m.SetSelectedOption(1);
        game->ShowModalDialog(&ModalDialog::Instance());
        m_showingexitdialog=true;
        m_showexitdialog=false;
    }
    if(m_showingexitdialog==true && ModalDialog::Instance().Shown()==false)
    {
        m_showingexitdialog=false;
        m_showexitdialog=false;
        if(ModalDialog::Instance().SelectedOption()==0)
        {
            m_changestate=Game::STATE_MAINMENU;
        }
    }

    m_gamedata->DispatchGameEvents();       // dispatch game events before recycling - in case event references mob/obj
    m_gamedata->RecycleMobs();
    m_gamedata->RecycleGroundItems();

    for(int i=0; i<MAX_MOBS; i++)
    {
        if(m_gamedata->m_mobs[i].GetActive()==true)
        {
            m_gamedata->m_mobs[i].Update(1,game);
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
    int32_t i;
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());
    OutputStringStream ostr;
    constexpr int16_t viewsize=144;
    int64_t x;
    int64_t y;

    // draw terrain first
    for(y=0; y<9; y++)
    {
        for(x=0; x<9; x++)
        {
            const uint64_t wx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+x-4);
            const uint64_t wy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+y-4);
            m_gamedata->m_map.DrawTile(wx,wy,x*16,y*16);
        }
    }

    // TODO - draw ground items
    *DRAW_COLORS=(PALETTE_BROWN << 4);
    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
    for(y=0; y<9; y++)
    {
        for(x=0; x<9; x++)
        {
            const uint64_t wx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+x-4);
            const uint64_t wy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+y-4);
            for(i=0; i<MAX_GROUNDITEMS; i++)
            {
                if(m_gamedata->m_grounditem[i].GetActive()==true && m_gamedata->m_grounditemlocation[i][0]==wx && m_gamedata->m_grounditemlocation[i][1]==wy)
                {
                    LZ4Blitter::Instance().Blit(x*16,y*16,16,16,m_gamedata->m_grounditem[i].GetGlyphIndex()%16,m_gamedata->m_grounditem[i].GetGlyphIndex()/16,spriteitemFlags);
                }
            }
        }
    }

    // mob sprite may overlap surrounding terrain, so draw after entire terrain is drawn
    for(y=0; y<9; y++)
    {
        for(x=0; x<9; x++)
        {
            const uint64_t wx=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldx+x-4);
            const uint64_t wy=m_gamedata->m_map.WrapCoordinate(m_gamedata->m_playerworldy+y-4);

            for(i=0; i<MAX_MOBS; i++)
            {
                if(m_gamedata->m_mobs[i].GetActive()==true && m_gamedata->m_mobs[i].m_x==wx && m_gamedata->m_mobs[i].m_y==wy)
                {
                    const int16_t jx=m_gamedata->m_mobs[i].GetJitterX();
                    const int16_t jy=m_gamedata->m_mobs[i].GetJitterY();
                    blitMasked(spritecharacter,spritecharacterWidth,m_gamedata->m_mobs[i].GetSpriteIdxX(),m_gamedata->m_mobs[i].GetSpriteIdxY(),x*16+jx,y*16+jy,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags);
                    if(m_gamedata->m_mobs[i].m_health!=m_gamedata->m_mobs[i].GetMaxHealth())
                    {
                        DrawMobHealth(x*16,y*16,m_gamedata->m_mobs[i].m_health,m_gamedata->m_mobs[i].GetMaxHealth());
                    }
                }
            }

        }
    }

    // draw player
    {

        int16_t x=4;
        int16_t y=4;
        //if(wx==m_gamedata->m_playerworldx && wy==m_gamedata->m_playerworldy)
        {
            *DRAW_COLORS=(PALETTE_BROWN << 4);
            if(m_gamedata->m_map.GetTerrainType(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,true)==Tile::TERRAIN_LAND)
            {
                blitMasked(spritecharacter,spritecharacterWidth,3,0,x*16,y*16,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags);
            }
            else
            {
                if(m_gamedata->m_movedir==MOVE_LEFT)
                {
                    blitMasked(spritecharacter,spritecharacterWidth,2,0,x*16,y*16,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags|BLIT_FLIP_X);
                }
                else if(m_gamedata->m_movedir==MOVE_UP || m_gamedata->m_movedir==MOVE_NONE)
                {
                    blitMasked(spritecharacter,spritecharacterWidth,0,0,x*16,y*16,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags);
                }
                else if(m_gamedata->m_movedir==MOVE_RIGHT)
                {
                    blitMasked(spritecharacter,spritecharacterWidth,2,0,x*16,y*16,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags);
                }
                else if(m_gamedata->m_movedir==MOVE_DOWN)
                {
                    blitMasked(spritecharacter,spritecharacterWidth,1,0,x*16,y*16,PALETTE_BROWN << 4,PALETTE_WHITE << 4,spritecharacterFlags);
                }
            }
        }
    }

    // draw quest arrows
    {
        for(i=0; i<MAX_QUESTS; i++)
        {
            // TODO - check if quest is selected
            if(m_gamedata->m_quests[i].GetActive()==true && m_gamedata->m_quests[i].HasTargetLocation()==true)
            {
                LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
                *DRAW_COLORS=(PALETTE_WHITE << 4);

                const int64_t dx=m_gamedata->m_map.DeltaCoordinate(m_gamedata->m_playerworldx,m_gamedata->m_quests[i].GetCurrentTargetWorldX());
                const int64_t dy=m_gamedata->m_map.DeltaCoordinate(m_gamedata->m_playerworldy,m_gamedata->m_quests[i].GetCurrentTargetWorldY());
                if(dx!=0 || dy!=0)
                {
                    int16_t w=16;
                    int16_t h=16;
                    int8_t sx=0;
                    int8_t sy=1;
                    if(_abs(dx)>=_abs(dy))  // arrow on left or right of screen
                    {
                        x=(dx<0 ? -4 : viewsize-16+4);  // offset sprite by 16 pixels to the left/right
                        y=((static_cast<float>(dy)/static_cast<float>(_abs(dx)))*(viewsize/2))+(viewsize/2)-8;  // offset 16x16 sprite by 8 pixels top/bottom to center it
                        w=16;
                        h=(y+16>viewsize) ? 16-(y+16-viewsize) : 16;
                        sx=(dx<0 ? 3 : 1);
                    }
                    else    // arrow on top or bottom of screen
                    {
                        x=((static_cast<float>(dx)/static_cast<float>(_abs(dy)))*(viewsize/2))+(viewsize/2)-8;
                        y=(dy<0 ? -4 : viewsize-16+4);
                        w=(x+16>viewsize) ? 16-(x+16-viewsize) : 16;
                        h=16;
                        sx=(dy<0 ? 0 : 2);
                    }
                    LZ4Blitter::Instance().Blit(x,y,w,h,sx,sy,spriteitemFlags);
                }
                /*
                const float ang=m_gamedata->m_map.ComputeAngle(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,m_gamedata->m_quests[i].GetCurrentTargetWorldX(),m_gamedata->m_quests[i].GetCurrentTargetWorldY());
                // check for nan
                if(ang==ang)
                {
                    const float dx=_cos(ang);
                    const float dy=_sin(ang);
                    //line((16*4)+8,(16*4)+8,((16*4)+8)+(dx*20),((16*4)+8)+(dy*20));

                    // draw arrow icon pointing to quest location
                    *DRAW_COLORS=(PALETTE_WHITE << 4);
                    if(_dabs(dx)>=_dabs(dy))    // arrow on left or right of screen
                    {
                        const float scale=_dabs(static_cast<double>(viewsize)/(dx*2.0));
                        const int64_t xpos=(dx<0 ? -4 : viewsize-16+4);   // offset sprite by 16 pixels to the left
                        const int64_t ypos=(viewsize/2)+(scale*dy)-8;  // offset 16x16 sprite by 8 pixels
                        const int16_t h=(ypos+16>viewsize ? 16-(ypos+16-viewsize) : 16);
                        if(dx<0)
                        {
                            LZ4Blitter::Instance().Blit(xpos,ypos,16,h,3,1,spriteitemFlags);
                        }
                        else
                        {
                            LZ4Blitter::Instance().Blit(xpos,ypos,16,h,1,1,spriteitemFlags);
                        }
                    }
                    else    // arrow on top or bottom of screen
                    {
                        const float scale=_dabs(static_cast<double>(viewsize)/(dy*2.0));
                        const int64_t xpos=(viewsize/2)+(scale*dx)-8;
                        const int64_t ypos=(dy<0 ? -4 : viewsize-16+4);
                        const int16_t w=(xpos+16>viewsize ? 16-(xpos+16-viewsize) : 16);
                        if(dy<0)
                        {
                            LZ4Blitter::Instance().Blit(xpos,ypos,w,16,0,1,spriteitemFlags);
                        }
                        else
                        {
                            LZ4Blitter::Instance().Blit(xpos,ypos,w,16,2,1,spriteitemFlags);
                        }
                    }
                }
                */
            }
        }
    }

    // health
    DrawHealthBar(SCREEN_SIZE-15,1,6,30);
    //DrawManaBar(SCREEN_SIZE-7,1,6,20);
    DrawExpBar(SCREEN_SIZE-7,1,6,30);

    // level
    tp.Print("Lvl",SCREEN_SIZE-15,35,3,PALETTE_WHITE);
    ostr.Clear();
    ostr << m_gamedata->m_playerlevel;
    //tp.Print(ostr.Buffer(),SCREEN_SIZE-15,35,4,PALETTE_WHITE,TextPrinter);
    tp.PrintWrapped(ostr.Buffer(),0,45,4,SCREEN_SIZE,PALETTE_WHITE,TextPrinter::JUSTIFY_RIGHT);

    // draw ground items on left
    {
        int ic=0;
        for(i=0; i<MAX_GROUNDITEMS; i++)
        {
            if(m_gamedata->m_grounditem[i].GetActive()==true && m_gamedata->m_grounditemlocation[i][0]==m_gamedata->m_playerworldx && m_gamedata->m_grounditemlocation[i][1]==m_gamedata->m_playerworldy)
            {
                if(ic<2)
                {
                    if(m_gamedata->m_grounditem[i].GetQuestItem()==true)
                    {
                        *DRAW_COLORS=(PALETTE_BLUE << 4);
                    }
                    else if(m_gamedata->m_grounditem[i].GetEquipable()==true && m_gamedata->m_grounditem[i].GetMeleeAttack() > m_gamedata->GetEquippedMeleeAttack(m_gamedata->m_grounditem[i].GetEquipSlot()))
                    {
                        *DRAW_COLORS=(PALETTE_GREEN << 4);
                    }
                    else if(m_gamedata->m_grounditem[i].GetEquipable()==true && m_gamedata->m_grounditem[i].GetArmor() > m_gamedata->GetEquippedArmor(m_gamedata->m_grounditem[i].GetEquipSlot()))
                    {
                        *DRAW_COLORS=(PALETTE_GREEN << 4);
                    }
                    else if(m_gamedata->m_grounditem[i].GetEquipable()==true && m_gamedata->m_grounditem[i].GetHealth(Game::Instance().GetLevelMaxHealth(m_gamedata->m_playerlevel)) > m_gamedata->GetEquippedHealth(m_gamedata->m_grounditem[i].GetEquipSlot()))
                    {
                        *DRAW_COLORS=(PALETTE_GREEN << 4);
                    }
                    else
                    {
                        *DRAW_COLORS=(PALETTE_WHITE << 4);
                    }
                    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
                    LZ4Blitter::Instance().Blit(SCREEN_SIZE-16,SCREEN_SIZE-32-(ic*16),16,16,m_gamedata->m_grounditem[i].GetGlyphIndex()%16,m_gamedata->m_grounditem[i].GetGlyphIndex()/16,spriteitemFlags);
                }
                ic++;
            }
        }
        if(ic>2)
        {
            // TODO - draw + if more items
            tp.PrintCentered("+",SCREEN_SIZE-8,SCREEN_SIZE-48-10,1,PALETTE_WHITE);
        }
    }

    // print messages
    int16_t tpos=viewsize-(m_gamedata->GameMessageCount()*8);
    for(i=0; i<MAX_GAMEMESSAGE; i++)
    {
        if(m_gamedata->m_gamemessagedecay[i]>0 && m_gamedata->m_gamemessages[i])
        {
            tp.Print(m_gamedata->m_gamemessages[i],1,tpos,30,PALETTE_BROWN);
            tpos+=8;
        }
    }

    // bottom menu bar
    DrawMenuBar();

}

void StateGameOverworld::GetMenuSpriteSheetPos(const int16_t option, int16_t &x, int16_t &y)
{
    y=0;
    switch(option)
    {
    case OPTION_SAVE:
        x=(7*16);
        break;
    case OPTION_JOURNAL:
        x=(2*16);
        break;
    case OPTION_MAP:
        x=(3*16);
        break;
    case OPTION_INVENTORY:
        x=(9*16);
        break;
    case OPTION_REST:
        x=0;
        break;
    case OPTION_CHARACTER:
        x=(10*16);
        break;
    case OPTION_PICKUP:
        x=(4*16);
        break;
    case OPTION_HOME:
        x=(6*16);
        break;
    default:
        x=0;
    }
}

void StateGameOverworld::DrawMenuBar()
{
    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
    int16_t sx=0;
    int16_t sy=0;
    for(int i=0; i<OPTION_MAX; i++)
    {
        GetMenuSpriteSheetPos(i,sx,sy);
        *DRAW_COLORS=m_gamedata->m_selectedmenu==i ? (PALETTE_BROWN << 4 | PALETTE_WHITE) : (PALETTE_WHITE << 4 | PALETTE_BROWN);
        //blitSub(spriteitem,(i*16),SCREEN_SIZE-16,16,16,sx,sy,spriteitemWidth,spriteitemFlags);
        LZ4Blitter::Instance().Blit(i*16,SCREEN_SIZE-16,16,16,sx/16,sy/16,spriteitemFlags);
    }
}

void StateGameOverworld::DrawHealthBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h)
{
    const int16_t hph=(static_cast<double>(m_gamedata->m_playerhealth)/static_cast<double>(m_gamedata->GetPlayerMaxHealth()))*(h-4);

    *DRAW_COLORS=(PALETTE_WHITE << 4);
    rect(x,y,w,h);
    //if(hp>?? || (m_gamedata->m_ticks/10)%3<2)    // blink if low
    if(hph>0 && (hph>(h*2/10) || (m_gamedata->m_ticks/10)%3<2))
    {
        *DRAW_COLORS=(PALETTE_GREEN << 4);
        rect(x+2,y+2+((h-4)-hph),w-4,hph);
    }
}
/*
void StateGameOverworld::DrawManaBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h)
{
    int16_t mh=h-4;
    *DRAW_COLORS=(PALETTE_WHITE << 4);
    rect(x,y,w,h);
    *DRAW_COLORS=(PALETTE_BLUE << 4);
    rect(x+2,y+2+((h-4)-mh),w-4,mh);
}
*/
void StateGameOverworld::DrawExpBar(const int16_t x, const int16_t y, const int16_t w, const int16_t h)
{
    int32_t nextexp=Game::Instance().GetNextLevelExperience(m_gamedata->m_playerlevel);
    int16_t exph=(static_cast<double>(nextexp-m_gamedata->m_playerexpnextlevel)/static_cast<double>(nextexp))*(h-4);
    *DRAW_COLORS=(PALETTE_WHITE << 4);
    rect(x,y,w,h);
    if(exph>0)
    {
        *DRAW_COLORS=(PALETTE_BLUE << 4);
        rect(x+2,y+2+((h-4)-exph),w-4,exph);
    }
}

void StateGameOverworld::DrawMobHealth(const int16_t x, const int16_t y, const int16_t health, const int16_t maxhealth)
{
    const int16_t hpw=(static_cast<float>(health)/static_cast<double>(maxhealth))*14.0f;
    *DRAW_COLORS=(PALETTE_WHITE << 4) | PALETTE_WHITE;
    rect(x,y-2,16,3);
    *DRAW_COLORS=(PALETTE_GREEN << 4) | PALETTE_GREEN;
    rect(x+1,y-1,hpw,1);
}

int8_t StateGameOverworld::GetNextAvailableQuestIndex() const
{
    for(int32_t i=0; i<MAX_QUESTS; i++)
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
    return (GetExistingActiveQuest(wx,wy)>-1);
}

int8_t StateGameOverworld::GetExistingActiveQuest(const uint64_t wx, const uint64_t wy) const
{
    for(int32_t i=0; i<MAX_QUESTS; i++)
    {
        if(m_gamedata->m_quests[i].GetActive()==true && m_gamedata->m_quests[i].m_sourcex==wx && m_gamedata->m_quests[i].m_sourcey==wy)
        {
            return i;
        }
    }
    return -1;
}
