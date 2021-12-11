#include "settings.h"
#include "gameio.h"

Settings::Settings()
{
    m_movedelay=15;
    m_moverepeat=1;
    m_gamepad=1;
}

Settings::~Settings()
{

}

Settings &Settings::Instance()
{
    static Settings settings;
    return settings;
}

void Settings::WriteSettings(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;

    GameIO::write_int16_t(&p[pos],m_movedelay);
    pos+=2;
    GameIO::write_int16_t(&p[pos],m_moverepeat);
    pos+=2;
    GameIO::write_uint8_t(&p[pos],m_gamepad);
    pos+=1;
}

void Settings::LoadSettings(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;

    m_movedelay=GameIO::read_int16_t(&p[pos]);
    pos+=2;
    m_moverepeat=GameIO::read_int16_t(&p[pos]);
    pos+=2;
    m_gamepad=GameIO::read_uint8_t(&p[pos]);
    pos+=1;

    if(m_movedelay<0)
    {
        m_movedelay=0;
    }
    if(m_movedelay>120)
    {
        m_movedelay=120;
    }
    if(m_moverepeat<1)
    {
        m_moverepeat=1;
    }
    if(m_moverepeat>30)
    {
        m_moverepeat=30;
    }
}

int16_t Settings::GetMoveDelay() const
{
    return m_movedelay;
}

void Settings::SetMoveDelay(const int16_t movedelay)
{
    m_movedelay=movedelay;
}

int16_t Settings::GetMoveRepeat() const
{
    return m_moverepeat;
}

void Settings::SetMoveRepeat(const int16_t moverepeat)
{
    m_moverepeat=moverepeat;
}

uint8_t Settings::GetGamepad() const
{
    return m_gamepad;
}

void Settings::SetGamepad(const uint8_t gamepad)
{
    m_gamepad=gamepad;
}
