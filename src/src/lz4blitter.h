#pragma once

#include <stdint.h>

class LZ4Blitter
{
public:
    LZ4Blitter();
    ~LZ4Blitter();

    static LZ4Blitter &Instance();

    void SetSheet(uint8_t **sheet, const int16_t width, const int16_t rowheight);

    void Blit(int32_t x, int32_t y, uint32_t width, uint32_t height, uint16_t sheetidxx, uint16_t sheetidxy, uint32_t flags);

private:
    uint8_t **m_sheet;
    int16_t m_width;
    int16_t m_rowheight;
    char m_buff[512];
    int16_t m_lastrow;
};
