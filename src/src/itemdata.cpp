#include "itemdata.h"
#include "gameio.h"
#include "outputstringstream.h"
#include "miscfuncs.h"
#include "wasmmath.h"

ItemData::ItemData()
{
    Reset();
}

ItemData::~ItemData()
{

}

void ItemData::Reset()
{
    m_flags=0;
    //m_type=0;
    //m_glyphidx=0;
    m_template=0;
    for(int i=0; i<4; i++)
    {
        m_data[i]=0;
    }
}

void ItemData::SetFlag(const uint16_t flag, const bool val)
{
    m_flags=(m_flags & ~flag) | (val ? flag : 0);
}

bool ItemData::GetFlag(const uint16_t flag) const
{
    return (m_flags & flag) == flag;
}

bool ItemData::GetActive() const
{
    return (m_flags & FLAG_ACTIVE)==FLAG_ACTIVE;
}

void ItemData::SetActive(const bool active)
{
    SetFlag(FLAG_ACTIVE,active);
}

bool ItemData::GetEquipped() const
{
    return (m_flags & FLAG_EQUIPPED)==FLAG_EQUIPPED;
}

void ItemData::SetEquipped(const bool equipped)
{
    SetFlag(FLAG_EQUIPPED,equipped);
}

bool ItemData::GetEquipable() const
{
    return (m_flags & FLAG_EQUIPABLE)==FLAG_EQUIPABLE;
}

void ItemData::SetEquipable(const bool equipable)
{
    SetFlag(FLAG_EQUIPABLE,equipable);
}


bool ItemData::GetConsumable() const
{
    return GetFlag(FLAG_CONSUMABLE);
}

void ItemData::SetConsumable(const bool consumable)
{
    SetFlag(FLAG_CONSUMABLE,consumable);
}

bool ItemData::GetQuestItem() const
{
    return GetFlag(FLAG_QUESTITEM);
}

void ItemData::SetQuestItem(const bool questitem)
{
    SetFlag(FLAG_QUESTITEM,questitem);
}

void ItemData::SetTemplate(const uint8_t templateidx)
{
    m_template=templateidx;
}

void ItemData::ApplyTemplate(const uint8_t templateidx)
{
    if(templateidx<m_itemtemplatecount)
    {
        m_template=templateidx;
        m_flags=m_itemtemplate[templateidx].flags;
    }
}

void ItemData::CreateFromTemplate(const uint8_t templateidx, const int16_t level)
{
    if(templateidx<m_itemtemplatecount)
    {
        Reset();
        ApplyTemplate(templateidx);
        // attack / armor
        if(GetType()==TYPE_MELEEWEAPON || GetType()==TYPE_PROJECTILEWEAPON)
        {
            m_data[0]=level;
        }
        if(GetType()==TYPE_SHIELD || GetType()==TYPE_HELMET || GetType()==TYPE_BODYARMOR || GetType()==TYPE_GAUNTLET || GetType()==TYPE_LEGARMOR || GetType()==TYPE_BOOT)
        {
            m_data[0]=_max(1,level/5);
        }
        if(GetAddArmor()==true)
        {
            m_data[0]=_max(1,_sqrt((level/10)));
        }
        if(GetAddMeleeAttack()==true)
        {
            m_data[0]=_max(1,_sqrt((level/10)));
        }
        if(GetAddHealth()==true)
        {
            m_data[0]=_max(1,level/5);
        }
    }
}

uint8_t ItemData::GetType() const
{
    //return m_type;
    if(m_template<m_itemtemplatecount)
    {
        return m_itemtemplate[m_template].type;
    }
    return 0;
}

uint8_t ItemData::TemplateFlags() const
{
    if(m_template<m_itemtemplatecount)
    {
        return m_itemtemplate[m_template].templateflags;
    }
    return 0;
}

uint8_t ItemData::GetGlyphIndex() const
{
    //return m_glyphidx;
    if(m_template<m_itemtemplatecount)
    {
        return m_itemtemplate[m_template].glyphidx;
    }
    return 0;
}

bool ItemData::GetAddHealth() const
{
    if(m_template>=0 && m_template<m_itemtemplatecount)
    {
        return (m_itemtemplate[m_template].templateflags & TEMPLATE_ADDHEALTH) == TEMPLATE_ADDHEALTH;
    }
    return false;
}

bool ItemData::GetAddArmor() const
{
    if(m_template>=0 && m_template<m_itemtemplatecount)
    {
        return (m_itemtemplate[m_template].templateflags & TEMPLATE_ADDARMOR) == TEMPLATE_ADDARMOR;
    }
    return false;
}

bool ItemData::GetAddMeleeAttack() const
{
    if(m_template>=0 && m_template<m_itemtemplatecount)
    {
        return (m_itemtemplate[m_template].templateflags & TEMPLATE_ADDMELEEATTACK) == TEMPLATE_ADDMELEEATTACK;
    }
    return false;
}

bool ItemData::GetRegenHealth() const
{
    if(m_template>=0 && m_template<m_itemtemplatecount)
    {
        return (m_itemtemplate[m_template].templateflags & TEMPLATE_ADDHEALTH) == TEMPLATE_ADDHEALTH;
    }
    return false;
}

/*
void ItemData::SetGlyphIndex(const uint8_t glyphindex)
{
    m_glyphidx=glyphindex;
}
*/

int8_t ItemData::GetEquipSlot() const
{
    switch(GetType())
    {
    case TYPE_HEALTHPOTION:
    case TYPE_MANAPOTION:
    case TYPE_EXPPOTION:
        return EQUIP_NONE;
        break;
    case TYPE_AMULET:
        return EQUIP_NECK;
        break;
    case TYPE_RING:
        return EQUIP_FINGER;
        break;
    case TYPE_MELEEWEAPON:
    case TYPE_PROJECTILEWEAPON:
        return EQUIP_WEAPONHAND;
        break;
    case TYPE_SHIELD:
        return EQUIP_SHIELDHAND;
        break;
    case TYPE_HELMET:
        return EQUIP_HEAD;
        break;
    case TYPE_BODYARMOR:
        return EQUIP_BODY;
        break;
    case TYPE_GAUNTLET:
        return EQUIP_HAND;
        break;
    case TYPE_LEGARMOR:
        return EQUIP_LEG;
        break;
    case TYPE_BOOT:
        return EQUIP_FOOT;
        break;
    }
    return EQUIP_NONE;
}

bool ItemData::WriteItemData(void *data) const
{
    int64_t pos=0;
    uint8_t *p=(uint8_t *)data;

    GameIO::write_uint8_t(&p[pos],m_flags);
    pos+=1;
    //GameIO::write_uint8_t(&p[pos],m_type);
    //pos+=1;
    //GameIO::write_uint8_t(&p[pos],m_glyphidx);
    //pos+=1;
    GameIO::write_uint8_t(&p[pos],m_template);
    pos+=1;
    for(int i=0; i<4; i++)
    {
        GameIO::write_uint8_t(&p[pos],m_data[i]);
        pos+=1;
    }
    return true;
}

bool ItemData::LoadItemData(void *data)
{
    int64_t pos=0;
    const uint8_t *p=(uint8_t *)data;

    m_flags=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    //m_type=GameIO::read_uint8_t(&p[pos]);
    //pos+=1;
    //m_glyphidx=GameIO::read_uint8_t(&p[pos]);
    //pos+=1;
    m_template=GameIO::read_uint8_t(&p[pos]);
    pos+=1;
    for(int i=0; i<4; i++)
    {
        m_data[i]=GameIO::read_uint8_t(&p[pos]);
        pos+=1;
    }
    return true;
}

int16_t ItemData::GetMeleeAttack() const
{
    if(GetType()==TYPE_MELEEWEAPON || GetAddMeleeAttack()==true)
    {
        return m_data[0];
    }
    return 0;
}

int16_t ItemData::GetArmor() const
{
    const uint8_t itemtype=GetType();
    if(itemtype==TYPE_HELMET || itemtype==TYPE_BODYARMOR || itemtype==TYPE_GAUNTLET || itemtype==TYPE_LEGARMOR || itemtype==TYPE_BOOT || itemtype==TYPE_SHIELD || GetAddArmor()==true)
    {
        return m_data[0];
    }
    return 0;
}

int16_t ItemData::GetHealth(const int16_t levelhealth) const
{
    if(m_template>=0 && m_template<m_itemtemplatecount)
    {
        if((m_itemtemplate[m_template].templateflags & TEMPLATE_ADDHEALTH) == TEMPLATE_ADDHEALTH)
        {
            if((m_itemtemplate[m_template].templateflags & TEMPLATE_RESTFULSLEEP) == TEMPLATE_RESTFULSLEEP)
            {
                return -(levelhealth/2);
            }
            return m_data[0];
        }
    }
    return 0;
}

/*
bool ItemData::HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param, const int8_t idx)
{
    if(GetActive()==true)
    {
        return true;
    }

    return false;
}
*/

/*
void ItemData::GetShortDescription(char *desc, const int16_t len)
{
    OutputStringStream ostr;
    ostr << m_itemtemplate[m_template].shortdesc;
    strncpy(desc,ostr.Buffer(),len-1);
}

void ItemData::GetDescription(char *desc, const int16_t len)
{
    OutputStringStream ostr;
    ostr << m_itemtemplate[m_template].desc;
    strncpy(desc,ostr.Buffer(),len-1);
}
*/

const char *ItemData::GetShortDescription() const
{
    if(m_template<m_itemtemplatecount)
    {
        return m_itemtemplate[m_template].shortdesc;
    }
    return nullptr;
}

const char *ItemData::GetShortDescription(uint8_t itemtemplate)
{
    if(itemtemplate<m_itemtemplatecount)
    {
        return m_itemtemplate[itemtemplate].shortdesc;
    }
    return nullptr;
}

const char *ItemData::GetDescription() const
{
    if(m_template<m_itemtemplatecount)
    {
        return m_itemtemplate[m_template].desc;
    }
    return nullptr;
}

bool ItemData::HandleGameEvent(int16_t eventtype, GameData *gamedata, GameEventParam param, const int8_t idx)
{
    if(GetActive()==true)
    {
        switch(eventtype)
        {
        case GameEvent::EVENT_PLAYERMOVE:
            break;
        }
    }
    return true;
}

bool ItemData::MatchTemplate(const uint8_t itemtype, const uint8_t equipslot, const uint8_t templateidx) const
{
    if(templateidx<m_itemtemplatecount)
    {
        if((itemtype==TYPE_ANY || itemtype==m_itemtemplate[templateidx].type) && (equipslot==EQUIP_ANY || equipslot==m_itemtemplate[templateidx].equipslot))
        {
            return true;
        }
    }
    return false;
}

uint8_t ItemData::RandomItemTypeFromDropType(RandomMT &rand, uint8_t droptype)
{
    uint8_t itemtype=0;
    uint8_t typecount=0;
    bool types[ItemData::TYPE_MAX];
    for(int i=0; i<ItemData::TYPE_MAX; i++)
    {
        types[i]=false;
    }
    if((droptype & ItemData::DROP_WEAPON)==ItemData::DROP_WEAPON)
    {
        types[ItemData::TYPE_MELEEWEAPON]=true;
        //types[ItemData::TYPE_PROJECTILEWEAPON]=true;
        typecount+=1;
    }
    if((droptype & ItemData::DROP_ARMOR)==ItemData::DROP_ARMOR)
    {
        types[ItemData::TYPE_SHIELD]=true;
        types[ItemData::TYPE_HELMET]=true;
        types[ItemData::TYPE_BODYARMOR]=true;
        types[ItemData::TYPE_GAUNTLET]=true;
        //types[ItemData::TYPE_LEGARMOR]=true;
        types[ItemData::TYPE_BOOT]=true;
        typecount+=5;
    }
    if((droptype & ItemData::DROP_RING)==ItemData::DROP_RING)
    {
        //trace("not impelemented");
        //types[ItemData::TYPE_RING]=true;
        //typecount+=1;
    }
    if((droptype & ItemData::DROP_AMULET)==ItemData::DROP_AMULET)
    {
        //trace("not impelemented");
        types[ItemData::TYPE_AMULET]=true;
        typecount+=1;
    }
    if((droptype & ItemData::DROP_POTION)==ItemData::DROP_POTION)
    {
        //trace("not impelemented");
        // potions not implemented yet
        //types[ItemData::TYPE_MANAPOTION]=true;
        types[ItemData::TYPE_HEALTHPOTION]=true;
        //types[ItemData::TYPE_EXPPOTION]=true;
        typecount+=1;
    }

    if(typecount>0)
    {
        int r=rand.Next()%typecount;
        uint8_t count=0;
        for(int i=0; i<ItemData::TYPE_MAX; i++)
        {
            if(types[i]==true)
            {
                count++;
                if(count>r)
                {
                    itemtype=i;
                    break;
                }
            }
        }
    }

    return itemtype;
}

bool ItemData::CreateRandom(RandomMT &rand, const uint8_t itemtype, const uint8_t equipslot, const int16_t level)
{
    int64_t total=0;
    for(int i=0; i<m_itemtemplatecount; i++)
    {
        if(MatchTemplate(itemtype,equipslot,i)==true)
        {
            total+=m_itemtemplate[i].probability;
        }
    }
    if(total>0)
    {
        int64_t r=rand.Next()%total;
        total=0;
        for(int i=0; i<m_itemtemplatecount; i++)
        {
            if(MatchTemplate(itemtype,equipslot,i)==true)
            {
                total+=m_itemtemplate[i].probability;
                if(total>r)
                {
                    CreateFromTemplate(i,level);
                    /*  moved into CreateFromTemplate
                    Reset();
                    ApplyTemplate(i);
                    // attack / armor
                    if(GetType()==TYPE_MELEEWEAPON || GetType()==TYPE_PROJECTILEWEAPON)
                    {
                        m_data[0]=level;
                    }
                    if(GetType()==TYPE_SHIELD || GetType()==TYPE_HELMET || GetType()==TYPE_BODYARMOR || GetType()==TYPE_GAUNTLET || GetType()==TYPE_LEGARMOR || GetType()==TYPE_BOOT)
                    {
                        m_data[0]=_max(1,level/5);
                    }
                    if(GetAddArmor()==true)
                    {
                        m_data[0]=_max(1,_sqrt((level/10)));
                    }
                    if(GetAddMeleeAttack()==true)
                    {
                        m_data[0]=_max(1,_sqrt((level/10)));
                    }
                    if(GetAddHealth()==true)
                    {
                        m_data[0]=_max(1,level/5);
                    }
                    */
                    return true;
                }
            }
        }
    }
    return false;
}
