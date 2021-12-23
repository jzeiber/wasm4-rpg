#pragma once

#include <stdint.h>
#include "igameeventhandler.h"

class QuestData:public IGameEventHandler
{
public:
    QuestData();
    ~QuestData();

    enum Flags
    {
        FLAG_ACTIVE   =0b00000001,
        FLAG_SELECTED =0b00000010,
        FLAG_SOURCELOC=0b00000100,
        FLAG_TUTORIAL =0b00001000,
        FLAG_COMPLETED=0b00010000
    };

    enum QuestType
    {
        TYPE_NONE=0,
        TYPE_TRAVELAREA=1,
        TYPE_TRAVELLOCATION=2,
        TYPE_TRAVELDISTANCE=3,
        TYPE_DELIVERY=4,
        TYPE_KILLAREAMONSTERS=5,
        TYPE_VISITANYTOWN=6,
        TYPE_ACCEPTQUESTS=7,
        TYPE_RETRIEVEITEM=8
        // TYPE_PATROL
    };


    void Reset();

    bool GetActive() const;
    bool GetSelected() const;

    void SetActive(const bool active);
    void SetSelected(const bool selected);

    bool WriteQuestData(void *data) const;
    bool LoadQuestData(void *data);

    void SetHasSourceLocation(const bool hassourceloc);
    bool HasSourceLocation() const;

    void SetTutorial(const bool tutorial);
    bool Tutorial() const;

    void SetCompleted(const bool completed);
    bool GetCompleted() const;

    bool HasTargetLocation() const;
    int64_t GetCurrentTargetWorldX() const;
    int64_t GetCurrentTargetWorldY() const;

    bool HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param, const int8_t idx);

    void GetQuestGiverDescription(char *desc, const int16_t len);
    void GetDescription(char *desc, const int16_t len);
    bool GetTargetLocationDistance(char *dist, const int16_t len, GameData *gamedata, const int64_t sourcex, const int64_t sourcey);
    bool GetTargetLocationDirection(char *direction, const int16_t len, GameData *gamedata, const int64_t sourcex, const int64_t sourcey);

    static constexpr int16_t SaveDataLength() { return 25; }

    // 25 bytes of quest data
    uint8_t m_flags;    // active, selected, source location
    uint8_t m_type;
    uint32_t m_sourcex;
    uint32_t m_sourcey;
    uint32_t m_destx;   // dest vars may be reused for something else if type of quest doesn't have a destination
    uint32_t m_desty;
    uint8_t m_progress; // could be qty if killing # of beasts
    uint8_t m_data[6];  // data related to quest - count,reward,etc. (m_data[0]=count, m_data[1]=equip drop)

private:

    void SetFlag(const uint16_t flag, const bool val);
    bool GetFlag(const uint16_t flag) const;

};