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

    uint8_t m_flags;    // active, selected
    uint8_t m_type;
    uint32_t m_sourcex;
    uint32_t m_sourcey;
    uint32_t m_destx;
    uint32_t m_desty;
    uint8_t m_progress; // could be qty if killing # of beasts
    uint8_t m_data[6];  // data related to quest - count,reward,etc.

private:
    /*
    Quest (25 bytes)
    uint32_t sourcex;
    uint32_t sourcey;
    uint32_t destx;
    uing32_t desty;
    uint8_t type;
    uint8_t flags;
    uint8_t progress;	// could be qty if killing # of beasts
    uint8_t data1;		// data related to quest - count,reward,etc.
    uint8_t data2;
    uint8_t data3;
    uint8_t data4;
    uint8_t data5;
    uint8_t data6;
    */
};