#include "statemainmenu.h"
#include "wasm4.h"
#include "palette.h"
#include "game.h"
#include "modaldialog.h"
#include "spriteitem.h"
#include "font5x7.h"
#include "textprinter.h"
#include "lz4blitter.h"

StateMainMenu::StateMainMenu():m_changestate(0),m_selectedslot(0),m_showdialog(false),m_game(nullptr)
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
    m_changestate=0;
    if(params)
    {
        m_game=(Game *)params;
    }
}

bool StateMainMenu::HandleInput(const Input *input)
{
    int32_t mouseoption=-1;

    if(input->MouseX()>=16 && input->MouseX()<SCREEN_SIZE-16 && input->MouseY()>=53 && input->MouseY()<53+(30*3))
    {
        mouseoption=(input->MouseY()-53)/30;
        if(input->MouseMoved()==true)
        {
            m_selectedslot=mouseoption;
        }
    }

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
    if((input->GamepadButtonPress(1,BUTTON_1)==true || (input->MouseButtonClick(1)==true && mouseoption==m_selectedslot)) && m_selectedslot>=0 && m_selectedslot<2)
    {
        m_showdialog=true;
    }
    if((input->GamepadButtonPress(1,BUTTON_1)==true || (input->MouseButtonClick(1)==true && mouseoption==m_selectedslot)) && m_selectedslot==2)
    {
        m_changestate=Game::STATE_SETTINGS;
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
            d.SetupNewGame(game->GetTicks());

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
    TextPrinter tp;
    tp.PrintCentered("WASM-4 RPG",SCREEN_SIZE/2,15,128,PALETTE_WHITE);

    tp.PrintCentered("MENU",SCREEN_SIZE/2,40,128,PALETTE_WHITE);

    tp.Print("Slot 1",40,60,128,(m_selectedslot==0 ? PALETTE_GREEN : PALETTE_WHITE));
    tp.Print(m_game->SaveSlotUsed(0) ? "Continue" : "New Game",40,70,128,(m_selectedslot==0 ? PALETTE_GREEN : PALETTE_WHITE));
    tp.Print("Slot 2",40,90,128,(m_selectedslot==1 ? PALETTE_GREEN : PALETTE_WHITE));
    tp.Print(m_game->SaveSlotUsed(1) ? "Continue" : "New Game",40,100,128,(m_selectedslot==1 ? PALETTE_GREEN : PALETTE_WHITE));
    tp.Print("Settings",40,124,128,(m_selectedslot==2 ? PALETTE_GREEN : PALETTE_WHITE));

    if(m_selectedslot>=0 && m_selectedslot<3)
    {
        *DRAW_COLORS=(PALETTE_WHITE << 4);
        int16_t ax=20;
        int16_t ay=60+(m_selectedslot*30);
        //blitSub(spriteitem,ax,ay,16,16,(5*16),(15*16),spriteitemWidth,spriteitemFlags);
        LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
        LZ4Blitter::Instance().Blit(ax,ay,16,16,5,1,spriteitemFlags);
    }
}
