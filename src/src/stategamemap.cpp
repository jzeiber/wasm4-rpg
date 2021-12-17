#include "stategamemap.h"
#include "game.h"
#include "printf.h"
#include "global.h"
#include "wasm4.h"
#include "spriteitem.h"
#include "lz4blitter.h"

StateGameMap::StateGameMap():m_gamedata(nullptr),m_changestate(-1),m_x(0),m_y(0)
{

}

StateGameMap::~StateGameMap()
{

}

StateGameMap &StateGameMap::Instance()
{
    static StateGameMap gm;
    return gm;
}

void StateGameMap::StateChanged(const uint8_t prevstate, void *params)
{
    if(params)
    {
        m_gamedata=((GameData *)params);
        m_x=m_gamedata->m_playerworldx;
        m_y=m_gamedata->m_playerworldy;
    }
    m_changestate=-1;
}

bool StateGameMap::HandleInput(const Input *input)
{
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

    if(input->GamepadButtonPress(1,BUTTON_1) || input->GamepadButtonPress(1,BUTTON_2))
    {
        m_changestate=Game::STATE_GAMEOVERWORLD;
    }

    return true;
}

void StateGameMap::Update(const int ticks, Game *game)
{
    if(m_changestate>=0)
    {
        game->ChangeState(m_changestate,m_gamedata);
    }
}

void StateGameMap::Draw()
{
    LZ4Blitter::Instance().SetSheet((uint8_t **)spriteitem,spriteitemWidth,spriteitemRowHeight);
    int16_t dq[MAX_QUESTS][2];
    for(int i=0; i<MAX_QUESTS; i++)
    {
        dq[i][0]=-1;
        dq[i][1]=-1;
    }
    for(int64_t y=0; y<SCREEN_SIZE; y++)
    {
        for(int64_t x=0; x<SCREEN_SIZE; x++)
        {
            uint64_t wx=m_gamedata->m_map.WrapCoordinate(m_x+x-(SCREEN_SIZE/2));
            uint64_t wy=m_gamedata->m_map.WrapCoordinate(m_y+y-(SCREEN_SIZE/2));
            Tile t=m_gamedata->m_map.ComputeTile(wx,wy);
            uint8_t ter=t.GetTerrainType();
            if(ter==Tile::TerrainType::TERRAIN_LAND)
            {
                *DRAW_COLORS=PALETTE_GREEN;
            }
            else if(ter==Tile::TerrainType::TERRAIN_WATER)
            {
                *DRAW_COLORS=PALETTE_BLUE;
            }
            if(t.GetFeature()==Tile::Feature::FEATURE_TOWN1 || t.GetFeature()==Tile::Feature::FEATURE_TOWN2)
            {
                *DRAW_COLORS=PALETTE_BROWN;
            }
            line(x,y,x,y);
            for(int i=0; i<MAX_QUESTS; i++)
            {
                if(m_gamedata->m_quests[i].GetActive()==true && m_gamedata->m_quests[i].HasTargetLocation()==true && m_gamedata->m_quests[i].GetCurrentTargetWorldX()==wx && m_gamedata->m_quests[i].GetCurrentTargetWorldY()==wy)
                {
                    dq[i][0]=x;
                    dq[i][1]=y;
                }
            }
        }
    }

    for(int i=0; i<MAX_QUESTS; i++)
    {
        if(dq[i][0]>-1 && dq[i][1]>-1)
        {
            *DRAW_COLORS=(PALETTE_WHITE << 4);
            //blitSub(spriteitem,dq[i][0]-8,dq[i][1]-8,16,16,(3*16),(12*16),spriteitemWidth,spriteitemFlags);
            LZ4Blitter::Instance().Blit(dq[i][0]-8,dq[i][1]-8,16,16,3,12,spriteitemFlags);
        }
    }

    *DRAW_COLORS=PALETTE_WHITE;
    snprintf(global::buff,global::buffsize-1,"%5lld,%5lld",m_x,m_y);
    text(global::buff,1,1);

}
