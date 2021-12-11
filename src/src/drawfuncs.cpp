#include "drawfuncs.h"
#include "wasm4.h"

// calacultes x,y on the sprite sheet based on rotations/flip flags
void getspritesheetcoords(const int32_t spritex, const int32_t spritey, const int16_t spritewidth, const int16_t spriteheight, const uint32_t flags, int32_t &sheetx, int32_t &sheety)
{
    sheetx=spritex;
    sheety=spritey;
    // rotate is counter clockwise, so we need to reverse that
    if((flags & BLIT_ROTATE)==BLIT_ROTATE)
    {
        sheetx=(spriteheight-1)-spritey;
        sheety=spritex;
    }
    if((flags & BLIT_FLIP_X)==BLIT_FLIP_X)
    {
        sheetx=(spritewidth-1)-sheetx;
    }
    if((flags & BLIT_FLIP_Y)==BLIT_FLIP_Y)
    {
        sheety=(spriteheight-1)-sheety;
    }
}

void blitOutlined(const uint8_t *spritesheet, const uint16_t sheetwidth, const uint8_t sheetx, const uint8_t sheety, const int16_t screenx, const int16_t screeny, const uint16_t forecolor, const uint16_t backcolor, const uint32_t flags)
{
    bool draw=false;
    *DRAW_COLORS=backcolor;
    for(int32_t y=static_cast<int32_t>(sheety)*16; y<(static_cast<int32_t>(sheety)*16)+16; y++)
    {
        for(int32_t x=static_cast<int32_t>(sheetx)*16; x<(static_cast<int32_t>(sheetx)*16)+16; x++)
        {
            draw=false;
            for(int32_t dy=y-1; dy<=y+1 && draw==false; dy++)
            {
                for(int32_t dx=x-1; dx<=x+1 && draw==false; dx++)
                {
                    if(dx>=(static_cast<int32_t>(sheetx)*16) && dx<(static_cast<int32_t>(sheetx)*16)+16 && dy>=(static_cast<int32_t>(sheety)*16) && dy<(static_cast<int32_t>(sheety)*16)+16)
                    {
                        int32_t sx=0;
                        int32_t sy=0;
                        getspritesheetcoords(dx-(static_cast<int32_t>(sheetx)*16),dy-(static_cast<int32_t>(sheety)*16),16,16,flags,sx,sy);
                        //const int32_t bitpos=(dy*sheetwidth)+(dx);
                        const int32_t bitpos=((sy+(static_cast<int32_t>(sheety)*16))*static_cast<int32_t>(sheetwidth))+(sx+(static_cast<int32_t>(sheetx)*16));
                        const int32_t byte=bitpos/8;
                        const int32_t bit=bitpos%8;
                        if(((spritesheet[byte] >> (7-bit)) & 0x01) == 0x01)
                        {
                            draw=true;
                        }
                    }
                }
            }
            if(draw==true)
            {
                line(screenx+(x-static_cast<uint32_t>(sheetx)*16),screeny+(y-static_cast<uint32_t>(sheety)*16),screenx+(x-static_cast<uint32_t>(sheetx)*16),screeny+(y-static_cast<uint32_t>(sheety)*16));
            }
        }
    }        

    *DRAW_COLORS=forecolor << 4;
    blitSub(spritesheet,screenx,screeny,16,16,sheetx*16,sheety*16,sheetwidth,flags);

}

void blitMasked(const uint8_t *spritesheet, const uint16_t sheetwidth, const uint8_t sheetx, const uint8_t sheety, const int16_t screenx, const int16_t screeny, const uint16_t forecolor, const uint16_t backcolor, const uint32_t flags)
{
    *DRAW_COLORS=backcolor;
    blitSub(spritesheet,screenx,screeny,16,16,(sheetx*16),((sheety+1)*16),sheetwidth,flags);
    *DRAW_COLORS=forecolor;
    blitSub(spritesheet,screenx,screeny,16,16,(sheetx*16),(sheety*16),sheetwidth,flags);
}
