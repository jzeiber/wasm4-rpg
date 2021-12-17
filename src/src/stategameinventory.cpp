#include "stategameinventory.h"
#include "game.h"
#include "textprinter.h"
#include "font5x7.h"
#include "global.h"
#include "outputstringstream.h"
#include "spriteitem.h"
#include "lz4blitter.h"

StateGameInventory::StateGameInventory():m_gamedata(nullptr),m_changestate(-1),m_selecteditem(0),m_selectedmenu(0)
{

}

StateGameInventory::~StateGameInventory()
{

}

StateGameInventory &StateGameInventory::Instance()
{
    static StateGameInventory gi;
    return gi;
}

void StateGameInventory::StateChanged(const uint8_t prevstate, void *params)
{
    if(params)
    {
        m_gamedata=((GameData *)params);
    }
    m_changestate=-1;
    m_selecteditem=0;
    m_selectedmenu=0;
}

bool StateGameInventory::HandleInput(const Input *input)
{

    if(input->GamepadButtonPress(1,BUTTON_LEFT))
    {
        SelectItemOffset(-1);
    }
    if(input->GamepadButtonPress(1,BUTTON_RIGHT))
    {
        SelectItemOffset(1);
    }
    if(input->GamepadButtonPress(1,BUTTON_UP))
    {
        SelectItemOffset(-10);
    }
    if(input->GamepadButtonPress(1,BUTTON_DOWN))
    {
        SelectItemOffset(10);
    }

    /*
    if(input->GamepadButtonPress(1,BUTTON_1) || input->GamepadButtonPress(1,BUTTON_2))
    {
        m_changestate=Game::STATE_GAMEOVERWORLD;
    }
    */
   if(input->GamepadButtonPress(1,BUTTON_1) && m_selectedmenu==OPTION_EXIT)
   {
       m_changestate=Game::STATE_GAMEOVERWORLD;
   }
   if(input->GamepadButtonPress(1,BUTTON_1) && m_selectedmenu==OPTION_DROP && m_gamedata->m_inventory[m_selecteditem].GetActive()==true)
   {
       // TODO - can't drop quest items
       for(int i=0; i<MAX_GROUNDITEMS; i++)
       {
           // only keep item on land
           if(m_gamedata->m_map.GetTerrainType(m_gamedata->m_playerworldx,m_gamedata->m_playerworldy,true)!=Tile::TERRAIN_WATER)
           {
            int8_t idx=m_gamedata->ClearAndGetGroundSlot();
            m_gamedata->m_grounditem[idx]=m_gamedata->m_inventory[m_selecteditem];
            m_gamedata->m_grounditem[idx].SetEquipped(false);
            m_gamedata->m_grounditemlocation[idx][0]=m_gamedata->m_playerworldx;
            m_gamedata->m_grounditemlocation[idx][1]=m_gamedata->m_playerworldy;
           }
           m_gamedata->m_inventory[m_selecteditem].Reset();
       }
   }
   if(input->GamepadButtonPress(1,BUTTON_1) && m_selectedmenu==OPTION_MOVELEFT)
   {
       int8_t idx=m_selecteditem;
       SelectItemOffset(-1);
       ItemData temp=m_gamedata->m_inventory[idx];
       m_gamedata->m_inventory[idx]=m_gamedata->m_inventory[m_selecteditem];
       m_gamedata->m_inventory[m_selecteditem]=temp;
   }
   if(input->GamepadButtonPress(1,BUTTON_1) && m_selectedmenu==OPTION_MOVERIGHT)
   {
       int8_t idx=m_selecteditem;
       SelectItemOffset(1);
       ItemData temp=m_gamedata->m_inventory[idx];
       m_gamedata->m_inventory[idx]=m_gamedata->m_inventory[m_selecteditem];
       m_gamedata->m_inventory[m_selecteditem]=temp;
   }
   if(input->GamepadButtonPress(1,BUTTON_1) && m_selectedmenu==OPTION_EQUIP)
   {
       m_gamedata->ToggleInventoryEquipped(m_selecteditem);
   }

   if(input->GamepadButtonPress(1,BUTTON_2))
   {
       m_selectedmenu++;
       if(m_selectedmenu>=OPTION_MAX)
       {
           m_selectedmenu=0;
       }
   }

    return true;
}

void StateGameInventory::Update(const int ticks, Game *game)
{
    if(m_changestate>=0)
    {
        game->ChangeState(m_changestate,m_gamedata);
    }
}

void StateGameInventory::Draw()
{
    OutputStringStream ostr;
    TextPrinter tp;
    tp.SetCustomFont(&Font5x7::Instance());

    tp.PrintCentered("Inventory",SCREEN_SIZE/2,1,128,PALETTE_WHITE);

    *DRAW_COLORS=PALETTE_WHITE;
    line(0,15,SCREEN_SIZE-1,15);
    line(0,48,SCREEN_SIZE-1,48);
    for(int i=0; i<MAX_INVENTORY; i++)
    {
        if(m_gamedata->m_inventory[i].GetActive()==true)
        {
            LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
            const uint8_t idx=m_gamedata->m_inventory[i].GetGlyphIndex();
            const int16_t gx=static_cast<int16_t>(idx%16)*16;
            const int16_t gy=static_cast<int16_t>(idx/16)*16;

            if(m_gamedata->m_inventory[i].GetEquipped()==true)
            {
                *DRAW_COLORS=(PALETTE_GREEN << 4);
            }
            else
            {
                *DRAW_COLORS=(PALETTE_WHITE << 4);
            }
            //blitSub(spriteitem,(i%10)*16,((i/10)*16)+16,16,16,gx,gy,spriteitemWidth,spriteitemFlags);

            LZ4Blitter::Instance().Blit((i%10)*16,((i/10)*16)+16,16,16,gx/16,gy/16,BLIT_1BPP);
        }
        if(m_selecteditem==i)
        {
            *DRAW_COLORS=(PALETTE_BLUE << 4);
            //blitSub(spriteitem,(i%10)*16,((i/10)*16)+16,16,16,1*16,12*16,spriteitemWidth,spriteitemFlags);
            LZ4Blitter::Instance().Blit((i%10)*16,((i/10)*16)+16,16,16,1,12,spriteitemFlags);
        }
    }

    if(m_selecteditem>=0 && m_selecteditem<MAX_INVENTORY && m_gamedata->m_inventory[m_selecteditem].GetActive()==true)
    {
        // TODO - show item stats
        OutputStringStream ostr;
        ostr << m_gamedata->m_inventory[m_selecteditem].GetShortDescription();
        tp.Print(ostr.Buffer(),1,50,128,PALETTE_WHITE);
        ostr.Clear();

        if(m_gamedata->m_inventory[m_selecteditem].GetEquipped()==true)
        {
            tp.PrintWrapped("Equipped",1,50,128,SCREEN_SIZE-1,PALETTE_GREEN,TextPrinter::JUSTIFY_RIGHT); 
        }

        ostr << m_gamedata->m_inventory[m_selecteditem].GetDescription();
        int16_t lines=tp.PrintWrapped(ostr.Buffer(),1,60,128,SCREEN_SIZE-2,PALETTE_WHITE);
        int16_t ypos=60+(lines*10);

        if(m_gamedata->m_inventory[m_selecteditem].GetEquipable()==true)
        {
            ypos+=15;
            int16_t comparey=ypos-10;
            bool comparetext=false;
            const int16_t equippedmelee=m_gamedata->GetEquippedMeleeAttack(m_gamedata->m_inventory[m_selecteditem].GetEquipSlot());
            const int16_t selectedmelee=m_gamedata->m_inventory[m_selecteditem].GetMeleeAttack();
            const int16_t equippedarmor=m_gamedata->GetEquippedArmor(m_gamedata->m_inventory[m_selecteditem].GetEquipSlot());
            const int16_t selectedarmor=m_gamedata->m_inventory[m_selecteditem].GetArmor();

            if(selectedmelee>0 || equippedmelee>0)
            {
                tp.Print("Attack",1,ypos,128,PALETTE_BLUE);
                ostr.Clear();
                ostr << selectedmelee;
                tp.Print(ostr.Buffer(),50,ypos,128,PALETTE_WHITE);

                if(m_gamedata->m_inventory[m_selecteditem].GetEquipped()==false && equippedmelee>=0)
                {
                    //tp.Print("Comparison",90,ypos-10,128,PALETTE_GREEN);
                    ostr.Clear();
                    ostr << equippedmelee;
                    tp.Print(ostr.Buffer(),130,ypos,128,PALETTE_WHITE);
                    comparetext=true;
                }

                ypos+=10;
            }

            if(selectedarmor>0 || equippedarmor>0)
            {
                tp.Print("Armor",1,ypos,128,PALETTE_BLUE);
                ostr.Clear();
                ostr << selectedarmor;
                tp.Print(ostr.Buffer(),50,ypos,128,PALETTE_WHITE);

                if(m_gamedata->m_inventory[m_selecteditem].GetEquipped()==false && equippedarmor>=0)
                {
                    ostr.Clear();
                    ostr << equippedarmor;
                    tp.Print(ostr.Buffer(),130,ypos,128,PALETTE_WHITE);
                    comparetext=true;
                }

                ypos+=10;
            }

            if(comparetext==true)
            {
                tp.Print("This",45,comparey,128,PALETTE_BLUE);
                tp.Print("Equipped",100,comparey,128,PALETTE_BLUE);
            }

        }

    }
    else
    {
        tp.PrintCentered("No Item Selected",SCREEN_SIZE/2,90,128,PALETTE_WHITE);
    }

    for(int i=0; i<OPTION_MAX; i++)
    {
        uint8_t idxx=1;
        uint8_t idxy=0;
        switch(i)
        {
        case OPTION_EXIT:
            idxx=1;
            idxy=0;
            break;
        case OPTION_EQUIP:
            idxx=11;
            idxy=0;
            break;
        case OPTION_DROP:
            idxx=5;
            idxy=0;
            break;
        case OPTION_CONSUME:
            idxx=8;
            idxy=0;
            break;
        case OPTION_MOVELEFT:
            idxx=3;
            idxy=1;
            break;
        case OPTION_MOVERIGHT:
            idxx=1;
            idxy=1;
            break;
        }
        *DRAW_COLORS=((i==m_selectedmenu) ? ((PALETTE_BROWN << 4) | PALETTE_WHITE) : ((PALETTE_WHITE << 4) | PALETTE_BROWN));
        LZ4Blitter::Instance().Blit((i*16),SCREEN_SIZE-16,16,16,idxx,idxy,spriteitemFlags);
    }


}

void StateGameInventory::SelectItemOffset(int16_t offset)
{
    int16_t i=m_selecteditem;
    i+=offset;
    while(i<0)
    {
        i+=MAX_INVENTORY;
    }
    while(i>=MAX_INVENTORY)
    {
        i-=MAX_INVENTORY;
    }
    m_selecteditem=i;
}
