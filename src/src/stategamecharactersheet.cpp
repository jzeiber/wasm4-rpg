#include "stategamecharactersheet.h"
#include "game.h"
#include "textprinter.h"
#include "font5x7.h"
#include "global.h"
#include "outputstringstream.h"

StateGameCharacterSheet::StateGameCharacterSheet():m_gamedata(nullptr),m_changestate(-1)
{

}

StateGameCharacterSheet::~StateGameCharacterSheet()
{

}

StateGameCharacterSheet &StateGameCharacterSheet::Instance()
{
    static StateGameCharacterSheet cs;
    return cs;
}

void StateGameCharacterSheet::StateChanged(const uint8_t prevstate, void *params)
{
    if(params)
    {
        m_gamedata=((GameData *)params);
    }
    m_changestate=-1;
}

bool StateGameCharacterSheet::HandleInput(const Input *input)
{

    if(input->GamepadButtonPress(1,BUTTON_LEFT))
    {

    }
    if(input->GamepadButtonPress(1,BUTTON_RIGHT))
    {

    }

    if(input->GamepadButtonPress(1,BUTTON_1) || input->GamepadButtonPress(1,BUTTON_2))
    {
        m_changestate=Game::STATE_GAMEOVERWORLD;
    }

    return true;
}

void StateGameCharacterSheet::Update(const int ticks, Game *game)
{
    if(m_changestate>=0)
    {
        game->ChangeState(m_changestate,m_gamedata);
    }
}

void StateGameCharacterSheet::Draw()
{
    OutputStringStream ostr;
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());

    tp.PrintCentered("Character",SCREEN_SIZE/2,1,128,PALETTE_WHITE);

    ostr << "Level " << m_gamedata->m_playerlevel;
    tp.Print(ostr.Buffer(),5,15,128,PALETTE_WHITE);
    ostr.Clear();
    ostr << m_gamedata->m_playerexpnextlevel << " Exp For Level Up";
    tp.Print(ostr.Buffer(),5,30,128,PALETTE_WHITE);
    ostr.Clear();
    ostr << "Health " << m_gamedata->m_playerhealth << " / " << m_gamedata->GetPlayerMaxHealth();
    tp.Print(ostr.Buffer(),5,45,128,PALETTE_WHITE);
    // calculate attack
    ostr.Clear();
    ostr << "Attack " << m_gamedata->GetPlayerMeleeAttack();
    tp.Print(ostr.Buffer(),5,60,128,PALETTE_WHITE);
    // calculate armor
    ostr.Clear();
    ostr << "Armor " << m_gamedata->GetPlayerArmor();
    tp.Print(ostr.Buffer(),5,75,128,PALETTE_WHITE);
}
