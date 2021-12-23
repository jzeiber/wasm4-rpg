#pragma once

#include <stdint.h>
#include "igameeventhandler.h"
#include "cppfuncs.h"
#include "randommt.h"

class ItemData:public IGameEventHandler
{
public:
    ItemData();
    ~ItemData();

    enum Flags
    {
        FLAG_ACTIVE     =0b00000001,
        FLAG_EQUIPPED   =0b00000010,
        FLAG_EQUIPABLE  =0b00000100,
        FLAG_CONSUMABLE =0b00001000,
        FLAG_QUESTITEM  =0b00010000
    };

    enum TemplateFlags
    {
        TEMPLATE_ADDHEALTH       = 0b00000001,
        TEMPLATE_ADDMELEEATTACK  = 0b00000010,
        TEMPLATE_ADDARMOR        = 0b00000100,
        TEMPLATE_REGENHEALTH     = 0b00001000,
        TEMPLATE_RESTFULSLEEP    = 0b00010000
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

    enum DropFlags
    {
        DROP_NONE       = 0b00000000,
        DROP_WEAPON     = 0b00000001,
        DROP_ARMOR      = 0b00000010,
        DROP_AMULET     = 0b00000100,
        DROP_RING       = 0b00001000,
        DROP_POTION     = 0b00010000
    };

    void Reset();

    bool GetActive() const;
    void SetActive(const bool active);

    bool GetEquipped() const;
    void SetEquipped(const bool equipped);

    bool GetEquipable() const;
    void SetEquipable(const bool equipable);

    bool GetConsumable() const;
    void SetConsumable(const bool consumable);

    bool GetQuestItem() const;
    void SetQuestItem(const bool questitem);

    void SetTemplate(const uint8_t templateidx);    // sets only template index
    void ApplyTemplate(const uint8_t templateidx);  // sets template and applies default template flags

    uint8_t GetType() const;
    uint8_t TemplateFlags() const;

    uint8_t GetGlyphIndex() const;
    //void SetGlyphIndex(const uint8_t glyphindex);

    bool WriteItemData(void *data) const;
    bool LoadItemData(void *data);

    int16_t GetMeleeAttack() const;
    int16_t GetArmor() const;
    int16_t GetHealth(const int16_t levelhealth) const;        // if this item adds/subtracts health - return it here

    bool GetAddHealth() const;
    bool GetAddArmor() const;
    bool GetAddMeleeAttack() const;
    bool GetRegenHealth() const;

    int8_t GetEquipSlot() const;

    bool HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param, const int8_t idx);

    //void GetShortDescription(char *desc, const int16_t len);
    //void GetDescription(char *desc, const int16_t len);
    const char *GetShortDescription() const;
    static const char *GetShortDescription(uint8_t itemtemplate);
    const char *GetDescription() const;

    static uint8_t RandomItemTypeFromDropType(RandomMT &rand, uint8_t droptype);

    bool CreateRandom(RandomMT &rand, const uint8_t itemtype, const uint8_t equipslot, const int16_t level);

    static constexpr int16_t SaveDataLength() { return 6; }
    static constexpr size_t ItemTemplateCount() { return m_itemtemplatecount; }

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
        uint8_t flags;          // flags copied to item - use for flags that may change as item is used
        uint8_t templateflags;  // flags defined only on template - use for fixed flags
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
    /*   type                 , flags                    , template flags                           , equip slot          ,prb,gidx, shortdesc     , desc */
/* 0 */ {TYPE_NONE            , 0                        , 0                                        , 0                   ,  0,   0, nullptr, nullptr                   },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 112, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 113, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 114, "Sword"    , "A short sword"       },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 115, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 116, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 117, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 118, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 119, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 120, "Sword"    , "A long sword"        },
/*10 */ {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 121, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 122, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 123, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 128, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 129, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 130, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 131, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 132, "Sword"    , "A long sword"        },
        {TYPE_MELEEWEAPON     , FLAG_EQUIPABLE           , 0                                        , EQUIP_WEAPONHAND    , 10, 133, "Sword"    , "A long sword"        },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  32, "Shield"   , "A round shield"      },
/*20 */ {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  33, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  34, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  35, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  36, "Shield"   , "A round shield"      },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  37, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  38, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  39, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  40, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  41, "Shield"   , "A kite shield"       },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  42, "Shield"   , "A tower shield"      },
/*30 */ {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  43, "Shield"   , "A wankel shield"     },
        {TYPE_SHIELD          , FLAG_EQUIPABLE           , 0                                        , EQUIP_SHIELDHAND    , 10,  48, "Shield"   , "A wankel shield"     },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE           , 0                                        , EQUIP_HAND          , 10,  24, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE           , 0                                        , EQUIP_HAND          , 10,  25, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE           , 0                                        , EQUIP_HAND          , 10,  26, "Gauntlets", "A pair of gauntlets" },
        {TYPE_GAUNTLET        , FLAG_EQUIPABLE           , 0                                        , EQUIP_HAND          , 10,  27, "Gauntlets", "A pair of gauntlets" },
        {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  54, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  55, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  56, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  57, "Helmet"   , "A helmet"            },
/*40 */ {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  58, "Helmet"   , "A helmet"            },
        {TYPE_HELMET          , FLAG_EQUIPABLE           , 0                                        , EQUIP_HEAD          , 10,  59, "Helmet"   , "A helmet"            },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE           , 0                                        , EQUIP_BODY          , 10,  49, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE           , 0                                        , EQUIP_BODY          , 10,  50, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE           , 0                                        , EQUIP_BODY          , 10,  51, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE           , 0                                        , EQUIP_BODY          , 10,  52, "Armor"    , "A breastplate"       },
        {TYPE_BODYARMOR       , FLAG_EQUIPABLE           , 0                                        , EQUIP_BODY          , 10,  53, "Armor"    , "A breastplate"       },
        {TYPE_BOOT            , FLAG_EQUIPABLE           , 0                                        , EQUIP_FOOT          , 10,  88, "Boots"    , "A pair of boots"     },
        {TYPE_BOOT            , FLAG_EQUIPABLE           , 0                                        , EQUIP_FOOT          , 10,  89, "Boots"    , "A pair of boots"     },
        {TYPE_BOOT            , FLAG_EQUIPABLE           , 0                                        , EQUIP_FOOT          , 10,  90, "Boots"    , "A pair of boots"     },
/*50 */ {TYPE_BOOT            , FLAG_EQUIPABLE           , 0                                        , EQUIP_FOOT          , 10,  91, "Boots"    , "A pair of boots"     },
        {TYPE_HEALTHPOTION    , FLAG_CONSUMABLE          , 0                                        , EQUIP_NONE          , 20, 160, "Health Potion", "A health potion that restores up to 50% of your health" },
        {TYPE_AMULET          , FLAG_EQUIPABLE           , TEMPLATE_ADDARMOR                        , EQUIP_NECK          ,  1, 100, "Amulet of Defense", "An amulet that adds armor" },
        {TYPE_AMULET          , FLAG_EQUIPABLE           , TEMPLATE_ADDMELEEATTACK                  , EQUIP_NECK          ,  1, 101, "Amulet of Power", "An amulet that adds attack strengh" },
        {TYPE_AMULET          , FLAG_EQUIPABLE           , TEMPLATE_ADDHEALTH                       , EQUIP_NECK          ,  1, 102, "Amulet of Vitality", "An amulet that increases total hit points"},
        {TYPE_AMULET          , FLAG_EQUIPABLE           , TEMPLATE_ADDHEALTH|TEMPLATE_RESTFULSLEEP , EQUIP_NECK          ,  1, 103, "Amulet of Protection", "Allows you to rest without being interrupted by monsters, but reduces your hit points"},
        {TYPE_MISC            , 0                        , 0                                        , EQUIP_NONE          , 10, 176, "Belt"     , "A belt"              },
        {TYPE_MISC            , 0                        , 0                                        , EQUIP_NONE          , 10, 177, "Cloak"    , "A cloak"             },
        {TYPE_MISC            , 0                        , 0                                        , EQUIP_NONE          , 10, 178, "Hood"     , "A hood"              },
        {TYPE_MISC            , 0                        , 0                                        , EQUIP_NONE          ,  1, 179, "Coin"     , "A gold coin"         },
        {TYPE_MISC            , 0                        , 0                                        , EQUIP_NONE          ,  1, 180, "Diamond"  , "A diamond"           }
    };

    static constexpr size_t m_itemtemplatecount=countof(m_itemtemplate);

    void SetFlag(const uint16_t flag, const bool val);
    bool GetFlag(const uint16_t flag) const;

    bool MatchTemplate(const uint8_t itemtype, const uint8_t equipslot, const uint8_t templateidx) const;

};
