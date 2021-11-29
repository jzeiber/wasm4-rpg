#include "settings.h"
#include "gameio.h"

Settings::Settings()
{
    m_movedelay=15;
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
}

void Settings::LoadSettings(void *data)
{
    uint8_t *p=(uint8_t *)data;
    int16_t pos=0;

    m_movedelay=GameIO::read_int16_t(&p[pos]);
    pos+=2;
}

int16_t Settings::GetMoveDelay() const
{
    return m_movedelay;
}

void Settings::SetMoveDelay(const int16_t movedelay)
{
    m_movedelay=movedelay;
}
