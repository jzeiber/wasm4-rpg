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

    void Print(const char *text, const int16_t x, const int16_t y, const int16_t len) const;
    void PrintWrapped(const char *text, const int16_t x, const int16_t y, const int16_t len, const int16_t maxwidth) const;
    void PrintCentered(const char *text, const int16_t cx, const int16_t y, const int16_t len) const;
    int16_t WrapPos(const char *text, const int16_t maxwidth) const;
    int16_t LineHeight() const;

private:
    IFont *m_font;

    void PutChar(const char c, const int16_t x, const int16_t y) const;
};