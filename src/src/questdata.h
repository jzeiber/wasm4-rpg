#pragma once

#include <stdint.h>

class QuestData
{
public:
    QuestData();
    ~QuestData();

    enum Flags
    {
        FLAG_ACTIVE   =0b00000001,
        FLAG_SELECTED =0b00000010
    };

    void Reset();

    bool GetActive() const;
    bool GetSelected() const;

    void SetActive(const bool active);
    void SetSelected(const bool selected);

    bool WriteQuestData(void *data) const;
    bool LoadQuestData(void *data);

    // 25 bytes of quest data
    uint8_t m_flags;    // active, selected
    uint8_t m_type;
    uint32_t m_sourcex;
    uint32_t m_sourcey;
    uint32_t m_destx;
    uint32_t m_desty;
    uint8_t m_progress; // could be qty if killing # of beasts
    uint8_t m_data[6];  // data related to quest - count,reward,etc.

private:

};