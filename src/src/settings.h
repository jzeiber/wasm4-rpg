#pragma once

#include <stdint.h>

class Settings
{
public:
    Settings();
    ~Settings();

    static Settings &Instance();

    void WriteSettings(void *data);
    void LoadSettings(void *data);

    int16_t GetMoveDelay() const;
    void SetMoveDelay(const int16_t movedelay);

    uint8_t GetGamepad() const;
    void SetGamepad(const uint8_t gamepad);

private:
    int16_t m_movedelay;
    uint8_t m_gamepad;
};