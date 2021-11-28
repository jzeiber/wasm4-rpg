#include "statemainmenu.h"
#include "wasm4.h"
#include "palette.h"
#include "game.h"
#include "modaldialog.h"
#include "spriteitem.h"
#include "font5x7.h"
#include "textprinter.h"

StateMainMenu::StateMainMenu():m_changestate(0),m_selectedslot(0),m_selectedsubslot(0),m_showdialog(false),m_game(nullptr)
{

}

StateMainMenu::~StateMainMenu()
{

}

StateMainMenu &StateMainMenu::Instance()
{
    static StateMainMenu mm;
    return mm;
}

void StateMainMenu::StateChanged(const uint8_t prevstate, void *params)
{
    if(params)
    {
        m_game=(Game *)params;
    }
}

bool StateMainMenu::HandleInput(const Input *input)
{
    if(input->GamepadButtonPress(1,BUTTON_UP))
    {
        m_selectedslot--;
        if(m_selectedslot<0)
        {
            m_selectedslot=0;
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_DOWN))
    {
        m_selectedslot++;
        if(m_selectedslot>2)
        {
            m_selectedslot=2;
        }
    }
    if((input->GamepadButtonPress(1,BUTTON_1)==true || input->GamepadButtonPress(1,BUTTON_2)==true) && m_selectedslot>=0 && m_selectedslot<2)
    {
        m_showdialog=true;
    }
    return true;
}

void StateMainMenu::Update(const int ticks, Game *game=nullptr)
{
    if(m_changestate!=0)
    {
        game->ChangeState(m_changestate,nullptr);
    }
    else if(m_showdialog==true && game)
    {
        ModalDialog *modal=&ModalDialog::Instance();
        modal->Reset();
        modal->SetCustomFont(&Font5x7::Instance());
        modal->SetTextWidth(144);

        if(m_game->SaveSlotUsed(m_selectedslot))
        {
            modal->SetText("What do you want to do?");
            modal->SetOption(0,"Continue");
            modal->SetOption(1,"New Game");
            modal->SetOption(2,"Cancel");
            modal->SetOption(3,"Delete");
            modal->SetSelectedOption(0);
        }
        else
        {
            modal->SetText("Are you sure you want to start a new game?");
            modal->SetOption(0,"Yes");
            modal->SetOption(1,"No");
            modal->SetSelectedOption(1);
        }
        game->ShowModalDialog(modal);
        m_showdialog=false;
        m_showingdialog=true;
    }
    if(m_showingdialog==true && ModalDialog::Instance().Shown()==false)
    {
        m_showingdialog=false;
        bool newgame=false;
        bool continuegame=false;
        bool deletegame=false;
        const int8_t selectedoption=ModalDialog::Instance().SelectedOption();

        if(m_game->SaveSlotUsed(m_selectedslot))
        {
            if(selectedoption==0)
            {
                continuegame=true;
            }
            else if(selectedoption==1)
            {
                newgame=true;
            }
            else if(selectedoption==3)
            {
                deletegame=true;
            }
        }
        else
        {
            if(selectedoption==0)
            {
                newgame=true;
            }
        }

        // continue game first - in case there is an error reading game data - new game will then take place
        if(continuegame==true)
        {
            if(m_game->LoadGameData(m_selectedslot)==true)
            {
                GameData &d=game->GetGameData();
                d.m_saveslot=m_selectedslot;
                d.m_map.UpdateWorldPosition(d.m_playerworldx,d.m_playerworldy);
                game->ChangeState(Game::STATE_GAMEOVERWORLD,&d);
            }
            else
            {
                newgame=true;
            }
        }
        if(newgame==true)
        {
            GameData &d=game->GetGameData();
            d.m_saveslot=m_selectedslot;
            d.m_seed=game->GetTicks();
            d.m_map.SetSeed(d.m_seed);
            d.m_map.SetSize(16384);
            d.m_selectedmenu=0;
            d.m_movedir=MOVE_NONE;
            d.m_playerworldx=0;
            d.m_playerworldy=0;
            d.m_map.UpdateWorldPosition(d.m_playerworldx,d.m_playerworldy);

            d.m_questscompleted=0;
            for(int i=0; i<MAX_QUESTS; i++)
            {
                d.m_quests[i].SetActive(false);
            }

            game->ChangeState(Game::STATE_GAMEOVERWORLD,&d);
        }
        if(deletegame==true)
        {
            game->DeleteGameData(m_selectedslot);
        }
    }
}

void StateMainMenu::Draw()
{
    *DRAW_COLORS=PALETTE_WHITE;
    TextPrinter tp;
    tp.PrintCentered("WASM-4 RPG",SCREEN_SIZE/2,15,128);

    *DRAW_COLORS=PALETTE_WHITE;
    text("MENU",80-((4*8)/2),40);

    text("Slot 1",40,60);
    if(m_game->SaveSlotUsed(0))
    {
        text("Continue",40,70);
    }
    else
    {
        text("New Game",40,70);
    }
    text("Slot 2",40,90);
    if(m_game->SaveSlotUsed(1))
    {
        text("Continue",40,100);
    }
    else
    {
        text("New Game",40,100);
    }
    text("Settings",40,120);

    if(m_selectedslot>=0 && m_selectedslot<3)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        int16_t ax=20;
        int16_t ay=60+(m_selectedslot*30);
        blitSub(spriteitem,ax,ay,16,16,(5*16),(15*16),spriteitemWidth,spriteitemFlags);
    }
}
