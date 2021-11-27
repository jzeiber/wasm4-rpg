#pragma once

#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "ifont.h"

class ModalDialog:public IUpdatable,public IDrawable,public IInputHandler
{
public:
    ModalDialog();
    virtual ~ModalDialog();

    static ModalDialog &Instance();

    bool HandleInput(const Input *input);
    void Update(const int ticks, Game *game);
    void Draw();

    virtual void Reset();
    virtual void SetText(const char *text);
    virtual void SetOption(const int8_t opt, const char *optval);

    virtual bool AllowParentDraw() const;
    virtual bool AllowParentUpdate() const;
    virtual bool AllowParentInput() const;

    void Show();
    bool Shown() const;
    int8_t SelectedOption() const;
    void SetSelectedOption(const int8_t selectedoption);
    void SetTextWidth(const int16_t textwidth);
    void SetCustomFont(IFont *font);

private:
    char m_text[192];
    char m_options[4][10];
    int16_t m_wrappos[15];
    bool m_shown;
    bool m_closedialog;
    int8_t m_selectedoption;
    int16_t m_textwidth;
    IFont *m_font;

    int16_t TextHeight() const;
    int16_t OptionsHeight() const;
    int16_t EntireHeight() const;
    int16_t TextStartXPos() const;
    int16_t TextStartYPos() const;
    int16_t OptionsStartYPos() const;

};
