#include "settings.h"

Settings::Settings()
{
    m_movedelay=10;
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
    p[0]=(m_movedelay >> 8) & 0xff;
    p[1]=(m_movedelay >> 0) & 0xff;
}

void Settings::LoadSettings(void *data)
{
    uint8_t *p=(uint8_t *)data;
    m_movedelay=(static_cast<int16_t>(p[0]) << 8);
    m_movedelay|=(static_cast<int16_t>(p[1]) << 0);
}

int16_t Settings::GetMoveDelay() const
{
    return m_movedelay;
}

void Settings::SetMoveDelay(const int16_t movedelay)
{
    m_movedelay=movedelay;
}
