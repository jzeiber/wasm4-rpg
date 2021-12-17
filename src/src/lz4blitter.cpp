#include "lz4blitter.h"
#include "lz4.h"
#include "wasm4.h"

LZ4Blitter::LZ4Blitter():m_sheet(nullptr),m_width(0),m_rowheight(0),m_lastrow(-1)
{

}

LZ4Blitter::~LZ4Blitter()
{

}

LZ4Blitter &LZ4Blitter::Instance()
{
    static LZ4Blitter lb;
    return lb;
}

void LZ4Blitter::SetSheet(uint8_t **sheet, const int16_t width, const int16_t rowheight)
{
    if(sheet!=m_sheet || width!=m_width || rowheight!=m_rowheight)
    {
        m_sheet=sheet;
        m_width=width;
        m_rowheight=rowheight;
        m_lastrow=-1;
    }
}

void LZ4Blitter::Blit(int32_t x, int32_t y, uint32_t width, uint32_t height, uint16_t sheetidxx, uint16_t sheetidxy, uint32_t flags)
{
    if(sheetidxy!=m_lastrow)
    {
        LZ4_decompress_fast((const char *)m_sheet[sheetidxy],m_buff,(m_width*m_rowheight)/8 < 512 ? (m_width*m_rowheight)/8 : 512);
        m_lastrow=sheetidxy;
    }
    blitSub((const uint8_t *)m_buff,x,y,width,height,sheetidxx*16,0,m_width,flags);
}
