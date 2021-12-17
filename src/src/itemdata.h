#pragma once

#include <stdint.h>
#include "igameeventhandler.h"
#include "cppfuncs.h"
#include "randommt.h"

class ItemData/*:public IGameEventHandler*/
{
public:
    ItemData();
    ~ItemData();

    enum Flags
    {
        FLAG_ACTIVE   =0b00000001,
        FLAG_EQUIPPED =0b00000010,
        FLAG_EQUIPABLE=0b00000100
    };

    enum ItemType
    {
        TYPE_NONE=0,
        TYPE_HEALTHPOTION,
        TYPE_MANAPOTION,
        TYPE_EXPPOTION,
        TYPE_AMULET,
        TYPE_RING,
        TYPE_MELEEWEAPON,
        TYPE_PROJECTILEWEAPON,
        TYPE_SHIELD,
        TYPE_HELMET,
        TYPE_BODYARMOR,
        TYPE_GAUNTLET,
        TYPE_LEGARMOR,
        TYPE_BOOT,
        TYPE_MISC,
        TYPE_MAX,
        TYPE_ANY        // any after MAX
    };

    enum EquipSlot
    {
        EQUIP_NONE=0,
        EQUIP_HEAD=1,
        EQUIP_BODY=2,
        EQUIP_HAND=3,
        EQUIP_LEG=4,
        EQUIP_FOOT=5,
        EQUIP_NECK=6,
        EQUIP_FINGER=7,
        EQUIP_WEAPONHAND=8,
        EQUIP_SHIELDHAND=9,
        EQUIP_ANY               // not assigned to equipment, used when creating new to specify any
    };

    void Reset();

    bool GetActive() const;
    void SetActive(const bool active);

    bool GetEquipped() const;
    void SetEquipped(const bool equipped);

    bool GetEquipable() const;
    void SetEquipable(const bool equipable);

    void SetTemplate(const uint8_t templateidx);

    uint8_t GetType() const;

    uint8_t GetGlyphIndex() const;
    //void SetGlyphIndex(const uint8_t glyphindex);

    bool WriteItemData(void *data) const;
    bool LoadItemData(void *data);

    int16_t GetMeleeAttack() const;
    int16_t GetArmor() const;

    int8_t GetEquipSlot() const;

    //bool HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param);

    //void GetShortDescription(char *desc, const int16_t len);
    //void GetDescription(char *desc, const int16_t len);
    const char *GetShortDescription() const;
    const char *GetDescription() const;

    bool CreateRandom(RandomMT &rand, const uint8_t itemtype, const uint8_t equipslot, const int16_t level);

    static constexpr int16_t SaveDataLength() { return 6; }

    // 6 bytes of item
    uint8_t m_flags;    // active, equipped, equipable
    //uint8_t m_type;
    //uint8_t m_glyphidx; // index into 16x16 item sprite sheet (max 256)
    uint8_t m_template;
    uint8_t m_data[4];  // data related to item - modifiers/etc. (1st byte for weapon will be damage, armor)

private:

    struct Data
    {
        uint8_t type;
        uint8_t flags;
        uint8_t equipslot;      // melee weapon/armor/ring/amulet/potion/etc.
        uint8_t probability;    // random probability - all probabilities for type added together when creating random item 
        uint8_t glyphidx;
        const char *shortdesc;
        const char *desc;
    };

    // ideas - 
    /* amulet of rest - no monter attack when resting */
    /* ring of regeneration - regenerate hp every x moves */

    static constexpr struct Data m_itemtemplate[]={
    /*   type                 , flags                           , equip slot          ,prb,gidx, shortdesc     , desc */
/* 0 */ {TYPE_NONE            , 0                               , 0                   ,  0,   0, nullptr, nullptr                   },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 112, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 113, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 114, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 115, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 116, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 117, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 118, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 119, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 120, "Sword"    , "A long sword"        },
/*10 */ {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 121, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 122, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 123, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 128, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 129, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 130, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 131, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 132, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE                  , EQUIP_WEAPONHAND    , 64, 133, "Sword"    , "A long sword"        },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  32, "Shield"   , "A round shield"      },
/*20 */ {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  33, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  34, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  35, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  36, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  37, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  38, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  39, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  40, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  41, "Shield"   , "A kite shield"       },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  42, "Shield"   , "A tower shield"      },
/*30 */ {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  43, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE                  , EQUIP_SHIELDHAND    , 64,  48, "Shield"   , "A wankel shield"     },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE                  , EQUIP_HAND          , 64,  24, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE                  , EQUIP_HAND          , 64,  25, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE                  , EQUIP_HAND          , 64,  26, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE                  , EQUIP_HAND          , 64,  27, "Gauntlets", "A pair of gauntlets" },
        {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  54, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  55, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  56, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  57, "Helmet"   , "A helmet"            },
/*40 */ {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  58, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE                  , EQUIP_HEAD          , 64,  59, "Helmet"   , "A helmet"            },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE                  , EQUIP_BODY          , 64,  49, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE                  , EQUIP_BODY          , 64,  50, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE                  , EQUIP_BODY          , 64,  51, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE                  , EQUIP_BODY          , 64,  52, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE                  , EQUIP_BODY          , 64,  53, "Armor"    , "A breastplate"       },
        {TYPE_BOOT            , FLAG_EQUIPABLE                  , EQUIP_FOOT          , 64,  88, "Boots"    , "A pair of boots"     },
        {TYPE_BOOT            , FLAG_EQUIPABLE                  , EQUIP_FOOT          , 64,  89, "Boots"    , "A pair of boots"     },
        {TYPE_BOOT            , FLAG_EQUIPABLE                  , EQUIP_FOOT          , 64,  90, "Boots"    , "A pair of boots"     },
        {TYPE_BOOT            , FLAG_EQUIPABLE                  , EQUIP_FOOT          , 64,  91, "Boots"    , "A pair of boots"     }
    };

    static constexpr size_t m_itemtemplatecount=countof(m_itemtemplate);

    void SetFlag(const uint16_t flag, const bool val);
    bool GetFlag(const uint16_t flag) const;

    bool MatchTemplate(const uint8_t itemtype, const uint8_t equipslot, const uint8_t templateidx) const;

};