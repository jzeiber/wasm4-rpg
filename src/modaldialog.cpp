#include "modaldialog.h"
#include "wasm4.h"
#include "palette.h"
#include "textprinter.h"
#include "fontsystem.h"

#include <stddef.h>

ModalDialog::ModalDialog():m_text{"\0"},m_options{{"\0"},{"\0"},{"\0"},{"\0"}},m_shown(false),m_selectedoption(-1),m_textwidth(128),m_font(&FontSystem::Instance())
{

}

ModalDialog::~ModalDialog()
{

}

ModalDialog &ModalDialog::Instance()
{
    static ModalDialog md;
    return md;
}

bool ModalDialog::HandleInput(const Input *input)
{
    if(input->GamepadButtonPress(1,BUTTON_1)==true)
    {
        m_closedialog=true;
    }
    if(input->GamepadButtonPress(1,BUTTON_UP) || input->GamepadButtonPress(1,BUTTON_LEFT))
    {
        for(int i=m_selectedoption-1; i>=0; i--)
        {
            if(m_options[i][0]!='\0')
            {
                m_selectedoption=i;
                i=-1;
            }
        }
    }
    if(input->GamepadButtonPress(1,BUTTON_DOWN) || input->GamepadButtonPress(1,BUTTON_RIGHT))
    {
        for(int i=m_selectedoption+1; i<4; i++)
        {
            if(m_options[i][0]!='\0')
            {
                m_selectedoption=i;
                i=4;
            }
        }
    }
    return true;
}

void ModalDialog::Update(const int ticks, Game *game)
{
    if(m_closedialog==true)
    {
        m_shown=false;
        m_closedialog=false;
    }
}

void ModalDialog::Draw()
{
    //cross hatch background
    *DRAW_COLORS=PALETTE_BROWN;
    for(int16_t y=0; y<SCREEN_SIZE; y++)
    {
        for(int16_t x=0; x<SCREEN_SIZE; x++)
        {
            if((x+y)%2==0)
            {
                line(x,y,x,y);
            }
        }
    }

    *DRAW_COLORS=PALETTE_BROWN | (PALETTE_WHITE << 4);
    rect(TextStartXPos()-4,TextStartYPos()-4,m_textwidth+8,EntireHeight()+8);

    *DRAW_COLORS=PALETTE_WHITE;
    TextPrinter tp;
    tp.SetCustomFont(m_font);
    int16_t pos=0;
    int16_t len=0;
    int16_t ypos=TextStartYPos();
    int16_t xpos=TextStartXPos();
    for(int i=0; i<15 && m_text[pos]; i++)
    {
        if(m_wrappos[i]>=0)
        {
            len=(m_wrappos[i]-pos)+1;
        }
        else
        {
            len=192;
        }
        tp.Print(&m_text[pos],xpos,ypos,len);
        pos+=len;
        ypos+=tp.LineHeight();
    }

    ypos=OptionsStartYPos();
    xpos=((SCREEN_SIZE-m_textwidth)/2)+(m_textwidth/4);
    for(int i=0; i<4; i++)
    {
        if(m_options[i][0]!='\0')
        {
            tp.PrintCentered(m_options[i],xpos,ypos,10);
            xpos+=(m_textwidth/2);
        }
        if(i==1)
        {
            ypos+=tp.LineHeight()+(tp.LineHeight()/2);
            xpos=((SCREEN_SIZE-m_textwidth)/2)+(m_textwidth/4);
        }
    }

    if(m_selectedoption>=0 && m_selectedoption<4 && m_options[m_selectedoption])
    {
        int16_t xpos=TextStartXPos()+(m_selectedoption%2==1 ? (m_textwidth/2) : 0);
        int16_t ypos=OptionsStartYPos()+(m_selectedoption>1 ? tp.LineHeight()+(tp.LineHeight()/2) : 0);
        *DRAW_COLORS=PALETTE_WHITE << 4;
        rect(xpos-2,ypos-2,(m_textwidth/2)+4,tp.LineHeight()+4);
    }

}

void ModalDialog::Reset()
{
    m_text[0]='\0';
    for(int i=0; i<4; i++)
    {
        m_options[i][0]='\0';
    }
    for(int i=0; i<15; i++)
    {
        m_wrappos[i]=-1;
    }
    m_shown=false;
    m_closedialog=false;
    m_selectedoption=-1;
    m_font=&FontSystem::Instance();
}

void ModalDialog::SetCustomFont(IFont *font)
{
    m_font=font;
}

void ModalDialog::SetText(const char *text)
{
    size_t pos=0;
    while(pos<192 && text[pos])
    {
        m_text[pos]=text[pos];
        pos++;
    }
    m_text[pos]='\0';

    for(int i=0; i<15; i++)
    {
        m_wrappos[i]=-1;
    }

    TextPrinter tp;
    tp.SetCustomFont(m_font);
    int16_t wp=tp.WrapPos(text,m_textwidth);
    pos=wp;
    m_wrappos[0]=wp;
    int8_t wi=1;
    while(text[pos+1] && wi<15)
    {
        pos++;
        wp=tp.WrapPos(&text[pos],m_textwidth);
        if(wp>=0)
        {
            pos+=wp;
            m_wrappos[wi]=pos;
            wi++;
        }
    }

}

void ModalDialog::SetOption(const int8_t opt, const char *optval)
{
    if(opt>=0 && opt<4)
    {
        size_t pos=0;
        while(pos<9 && optval[pos])
        {
            m_options[opt][pos]=optval[pos];
            pos++;
        }
        m_options[opt][pos]='\0';
    }
}

void ModalDialog::SetTextWidth(const int16_t textwidth)
{
    m_textwidth=textwidth;
}

bool ModalDialog::AllowParentDraw() const
{
    return true;
}

bool ModalDialog::AllowParentUpdate() const
{
    return false;
}

bool ModalDialog::AllowParentInput() const
{
    return false;
}

void ModalDialog::Show()
{
    m_shown=true;
    m_closedialog=false;
}

bool ModalDialog::Shown() const
{
    return m_shown;
}

int8_t ModalDialog::SelectedOption() const
{
    return m_selectedoption;
}

void ModalDialog::SetSelectedOption(const int8_t selectedoption)
{
    m_selectedoption=selectedoption;
}

int16_t ModalDialog::TextStartXPos() const
{
    return (SCREEN_SIZE-m_textwidth)/2;
}

int16_t ModalDialog::TextStartYPos() const
{
    return (SCREEN_SIZE-EntireHeight())/2;
}

int16_t ModalDialog::OptionsStartYPos() const
{
    TextPrinter tp;
    tp.SetCustomFont(m_font);
    return TextStartYPos()+TextHeight()+(tp.LineHeight()/2);
}

int16_t ModalDialog::TextHeight() const
{
    TextPrinter tp;
    tp.SetCustomFont(m_font);
    int16_t lc=0;
    int8_t i=0;
    while(m_wrappos[i]>=0 && ++i<15)
    {
        lc++;
    }
    return lc*tp.LineHeight();
}

int16_t ModalDialog::OptionsHeight() const
{
    TextPrinter tp;
    tp.SetCustomFont(m_font);
    int8_t lc=0;
    if(m_options[0][0] || m_options[1][0])
    {
        lc++;
    }
    if(m_options[2][0] || m_options[3][0])
    {
        lc++;
    }

    return (lc*tp.LineHeight())+(lc==2 ? tp.LineHeight()/2 : 0);
}

int16_t ModalDialog::EntireHeight() const
{
    TextPrinter tp;
    tp.SetCustomFont(m_font);
    int16_t th=TextHeight();
    int16_t oh=OptionsHeight();
    return th+oh+((th>0 && oh>0) ? tp.LineHeight()/2 : 0);
}
