#pragma once

#include <stdint.h>
#include "ifont.h"

class TextPrinter
{
public:
    TextPrinter();
    ~TextPrinter();

    void SetCustomFont(IFont *font);
    void SetSystemFont();
    //void SetCustomFont(uint8_t *customsprites, const int16_t spritesheetwidth, const int8_t charwidth, const int8_t charheight);

    void Print(const char *text, const int16_t x, const int16_t y, const int16_t len) const;
    void PrintWrapped(const char *text, const int16_t x, const int16_t y, const int16_t len, const int16_t maxwidth) const;
    void PrintCentered(const char *text, const int16_t cx, const int16_t y, const int16_t len) const;
    int16_t WrapPos(const char *text, const int16_t maxwidth) const;
    int16_t LineHeight() const;

private:
    /*
    int8_t m_charwidth;
    int8_t m_charheight;
    uint8_t *m_customsprites;
    int16_t m_spritesheetwidth;
    */
    IFont *m_font;

    void PutChar(const char c, const int16_t x, const int16_t y) const;
};