#include "textprinter.h"
#include "wasm4.h"
#include "fontsystem.h"

TextPrinter::TextPrinter():m_font(&FontSystem::Instance())
{

}

TextPrinter::~TextPrinter()
{

}

void TextPrinter::SetCustomFont(IFont *font)
{
    m_font=font;
}

void TextPrinter::SetSystemFont()
{
    m_font=&FontSystem::Instance();
}

int16_t TextPrinter::LineHeight() const
{
    return m_font->LineHeight();
}

void TextPrinter::Print(const char *text, const int16_t x, const int16_t y, const int16_t len, const uint16_t color) const
{
    if(text)
    {
        int16_t xpos=x;
        int16_t ypos=y;
        for(int i=0; i<len; i++)
        {
            if(text[i])
            {
                if(text[i]=='\r' || text[i]=='\n')
                {
                    xpos=x;
                    ypos+=LineHeight();
                }
                else
                {
                    PutChar(text[i],xpos,ypos,color);
                    xpos+=m_font->CharWidth(text[i]);
                }
            }
            else
            {
                i=len;
            }
        }
    }
}

void TextPrinter::PrintWrapped(const char *text, const int16_t x, const int16_t y, const int16_t len, const int16_t maxwidth, const uint16_t color) const
{
    int16_t yp=y;
    int16_t pos=0;
    int16_t wp=WrapPos(text,maxwidth);
    while(text[pos] && wp>=0)
    {
        Print(&text[pos],x,yp,wp+1,color);
        pos+=(wp+1);
        yp+=LineHeight();
        wp=WrapPos(&text[pos],maxwidth);
    }
}

void TextPrinter::PrintCentered(const char *text, const int16_t cx, const int16_t y, const int16_t len, const uint16_t color) const
{
    char buff[32];
    int16_t tlen=0;
    while(text[tlen] && tlen<32)
    {
        buff[tlen]=text[tlen];
        tlen++;
    }
    tlen=(tlen<len ? tlen : len);
    buff[tlen]='\0';
    Print(text,cx-(m_font->TextWidth(buff)/2),y,len,color);
}

void TextPrinter::PutChar(const char c, const int16_t x, const int16_t y, const uint16_t color) const
{
    m_font->PutChar(c,x,y,color);
}

int16_t TextPrinter::WrapPos(const char *text, const int16_t maxwidth) const
{
    int16_t textpos=0;
    int16_t currentwidth=0;
    int16_t lastspace=-1;

    if(!text || text[0]=='\0')
    {
        return 0;
    }

    // can't fit any chars in the output
    if(m_font->CharWidth(text[0])>maxwidth)
    {
        return 0;
    }

    while(text[textpos])
    {
        currentwidth+=m_font->CharWidth(text[textpos]);
        if(text[textpos]=='\r' || text[textpos]=='\n')
        {
            return textpos;
        }
        // group all spaces together
        if(text[textpos]==' ')
        {
            lastspace=textpos;
            while(text[textpos+1]==' ')
            {
                textpos++;
                lastspace=textpos;
                currentwidth+=m_font->CharWidth(text[textpos]);
            }
        }
        if(currentwidth>maxwidth)
        {
            return lastspace>-1 ? lastspace : textpos-1;
        }
        textpos++;
    }

    return textpos-1;
}
