#include "../botpch.h"
#include "playerbot.h"
#include "ChatHelper.h"
#include "AiFactory.h"
#include "strategy/values/ItemUsageValue.h"
#include <numeric>
#include <iomanip>

using namespace ai;
using namespace std;

map<string, uint32> ChatHelper::consumableSubClasses;
map<string, uint32> ChatHelper::tradeSubClasses;
map<string, uint32> ChatHelper::itemQualities;
map<string, uint32> ChatHelper::projectileSubClasses;
map<string, pair<uint32, uint32>> ChatHelper::itemClasses;
map<string, uint32> ChatHelper::slots;
map<string, uint32> ChatHelper::skills;
map<string, ChatMsg> ChatHelper::chats;
map<uint8, string> ChatHelper::classes;
map<uint8, string> ChatHelper::races;
map<uint8, map<uint8, string> > ChatHelper::specs;
unordered_map<string, vector<uint32>> ChatHelper::spellIds;

template<class T>
static bool substrContainsInMap(string searchTerm, map<string, T> searchIn)
{
    for (typename map<string, T>::iterator i = searchIn.begin(); i != searchIn.end(); ++i)
    {
		string term = i->first;
		if (term.size() > 1 && searchTerm.find(term) != string::npos)
            return true;
    }

    return false;
}

ChatHelper::ChatHelper(PlayerbotAI* ai) : PlayerbotAIAware(ai)
{
    itemQualities["poor"] = ITEM_QUALITY_POOR;
    itemQualities["gray"] = ITEM_QUALITY_POOR;
    itemQualities["normal"] = ITEM_QUALITY_NORMAL;
    itemQualities["white"] = ITEM_QUALITY_NORMAL;
    itemQualities["uncommon"] = ITEM_QUALITY_UNCOMMON;
    itemQualities["green"] = ITEM_QUALITY_UNCOMMON;
    itemQualities["rare"] = ITEM_QUALITY_RARE;
    itemQualities["blue"] = ITEM_QUALITY_RARE;
    itemQualities["epic"] = ITEM_QUALITY_EPIC;
    itemQualities["violet"] = ITEM_QUALITY_EPIC;
    itemQualities["legendary"] = ITEM_QUALITY_LEGENDARY;
    itemQualities["yellow"] = ITEM_QUALITY_LEGENDARY;

    consumableSubClasses["potion"] = ITEM_SUBCLASS_POTION;
    consumableSubClasses["elixir"] = ITEM_SUBCLASS_ELIXIR;
    consumableSubClasses["flask"] = ITEM_SUBCLASS_FLASK;
    consumableSubClasses["scroll"] = ITEM_SUBCLASS_SCROLL;
    consumableSubClasses["food"] = ITEM_SUBCLASS_FOOD;
    consumableSubClasses["bandage"] = ITEM_SUBCLASS_BANDAGE;
    consumableSubClasses["enchant"] = ITEM_SUBCLASS_CONSUMABLE_OTHER;

    projectileSubClasses["arrows"] = ITEM_SUBCLASS_ARROW;
    projectileSubClasses["bullets"] = ITEM_SUBCLASS_BULLET;

    //tradeSubClasses["cloth"] = ITEM_SUBCLASS_CLOTH;
    //tradeSubClasses["leather"] = ITEM_SUBCLASS_LEATHER;
    //tradeSubClasses["metal"] = ITEM_SUBCLASS_METAL_STONE;
    //tradeSubClasses["stone"] = ITEM_SUBCLASS_METAL_STONE;
    //tradeSubClasses["ore"] = ITEM_SUBCLASS_METAL_STONE;
    //tradeSubClasses["meat"] = ITEM_SUBCLASS_MEAT;
    //tradeSubClasses["herb"] = ITEM_SUBCLASS_HERB;
    //tradeSubClasses["elemental"] = ITEM_SUBCLASS_ELEMENTAL;
    //tradeSubClasses["disenchants"] = ITEM_SUBCLASS_ENCHANTING;
    //tradeSubClasses["enchanting"] = ITEM_SUBCLASS_ENCHANTING;
    //tradeSubClasses["gems"] = ITEM_SUBCLASS_JEWELCRAFTING;
    //tradeSubClasses["jewels"] = ITEM_SUBCLASS_JEWELCRAFTING;
    //tradeSubClasses["jewelcrafting"] = ITEM_SUBCLASS_JEWELCRAFTING;

    itemClasses["1h axe"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_AXE };
    itemClasses["2h axe"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_AXE2 };
    itemClasses["bow"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_BOW };
    itemClasses["gun"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_GUN };
    itemClasses["1h mace"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_MACE };
    itemClasses["2h mace"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_MACE2 };
    itemClasses["polearm"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_POLEARM };
    itemClasses["1h sword"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_SWORD };
    itemClasses["2h sword"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_SWORD2 };
    itemClasses["staff"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_STAFF };
    itemClasses["first"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_FIST };
    itemClasses["dagger"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_DAGGER };
    itemClasses["thrown"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_THROWN };
    itemClasses["crossbow"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_CROSSBOW };
    itemClasses["wand"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_WAND };
    itemClasses["fishing pole"] = { ITEM_CLASS_WEAPON, ITEM_SUBCLASS_WEAPON_FISHING_POLE };

    itemClasses["bag"] = { ITEM_CLASS_CONTAINER, ITEM_SUBCLASS_CONTAINER };

    itemClasses["cloth"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_CLOTH };
    itemClasses["leather"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_LEATHER };
    itemClasses["mail"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_MAIL };
    itemClasses["plate"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_PLATE };
    itemClasses["shield"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_SHIELD };
    itemClasses["libram"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_LIBRAM };
    itemClasses["idol"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_IDOL };
    itemClasses["totem"] = { ITEM_CLASS_ARMOR, ITEM_SUBCLASS_ARMOR_TOTEM };

    slots["head"] = EQUIPMENT_SLOT_HEAD;
    slots["neck"] = EQUIPMENT_SLOT_NECK;
    slots["shoulder"] = EQUIPMENT_SLOT_SHOULDERS;
    slots["shirt"] = EQUIPMENT_SLOT_BODY;
    slots["chest"] = EQUIPMENT_SLOT_CHEST;
    slots["waist"] = EQUIPMENT_SLOT_WAIST;
    slots["legs"] = EQUIPMENT_SLOT_LEGS;
    slots["feet"] = EQUIPMENT_SLOT_FEET;
    slots["wrist"] = EQUIPMENT_SLOT_WRISTS;
    slots["hands"] = EQUIPMENT_SLOT_HANDS;
    slots["finger 1"] = EQUIPMENT_SLOT_FINGER1;
    slots["finger 2"] = EQUIPMENT_SLOT_FINGER2;
    slots["trinket 1"] = EQUIPMENT_SLOT_TRINKET1;
    slots["trinket 2"] = EQUIPMENT_SLOT_TRINKET2;
    slots["back"] = EQUIPMENT_SLOT_BACK;
    slots["main hand"] = EQUIPMENT_SLOT_MAINHAND;
    slots["off hand"] = EQUIPMENT_SLOT_OFFHAND;
    slots["ranged"] = EQUIPMENT_SLOT_RANGED;
    slots["tabard"] = EQUIPMENT_SLOT_TABARD;

    skills["first aid"] = SKILL_FIRST_AID;
    skills["fishing"] = SKILL_FISHING;
    skills["cooking"] = SKILL_COOKING;
    skills["alchemy"] = SKILL_ALCHEMY;
    skills["enchanting"] = SKILL_ENCHANTING;
    skills["engineering"] = SKILL_ENGINEERING;
    skills["leatherworking"] = SKILL_LEATHERWORKING;
    skills["blacksmithing"] = SKILL_BLACKSMITHING;
    skills["tailoring"] = SKILL_TAILORING;
    skills["herbalism"] = SKILL_HERBALISM;
    skills["mining"] = SKILL_MINING;
    skills["skinning"] = SKILL_SKINNING;

    chats["party"] = CHAT_MSG_PARTY;
    chats["p"] = CHAT_MSG_PARTY;
    chats["guild"] = CHAT_MSG_GUILD;
    chats["g"] = CHAT_MSG_GUILD;
    chats["raid"] = CHAT_MSG_RAID;
    chats["r"] = CHAT_MSG_RAID;
    chats["whisper"] = CHAT_MSG_WHISPER;
    chats["w"] = CHAT_MSG_WHISPER;

    classes[CLASS_DRUID] = "druid";
    specs[CLASS_DRUID][0] = "balance";
    specs[CLASS_DRUID][1] = "feral combat";
    specs[CLASS_DRUID][2] = "restoration";

    classes[CLASS_HUNTER] = "hunter";
    specs[CLASS_HUNTER][0] = "beast mastery";
    specs[CLASS_HUNTER][1] = "marksmanship";
    specs[CLASS_HUNTER][2] = "survival";

    classes[CLASS_MAGE] = "mage";
    specs[CLASS_MAGE][0] = "arcane";
    specs[CLASS_MAGE][1] = "fire";
    specs[CLASS_MAGE][2] = "frost";

    classes[CLASS_PALADIN] = "paladin";
    specs[CLASS_PALADIN][0] = "holy";
    specs[CLASS_PALADIN][1] = "protection";
    specs[CLASS_PALADIN][2] = "retribution";

    classes[CLASS_PRIEST] = "priest";
    specs[CLASS_PRIEST][0] = "discipline";
    specs[CLASS_PRIEST][1] = "holy";
    specs[CLASS_PRIEST][2] = "shadow";

    classes[CLASS_ROGUE] = "rogue";
    specs[CLASS_ROGUE][0] = "assasination";
    specs[CLASS_ROGUE][1] = "combat";
    specs[CLASS_ROGUE][2] = "subtlety";

    classes[CLASS_SHAMAN] = "shaman";
    specs[CLASS_SHAMAN][0] = "elemental";
    specs[CLASS_SHAMAN][1] = "enhancement";
    specs[CLASS_SHAMAN][2] = "restoration";

    classes[CLASS_WARLOCK] = "warlock";
    specs[CLASS_WARLOCK][0] = "affliction";
    specs[CLASS_WARLOCK][1] = "demonology";
    specs[CLASS_WARLOCK][2] = "destruction";

    classes[CLASS_WARRIOR] = "warrior";
    specs[CLASS_WARRIOR][0] = "arms";
    specs[CLASS_WARRIOR][1] = "fury";
    specs[CLASS_WARRIOR][2] = "protection";

#ifdef MANGOSBOT_TWO
    classes[CLASS_DEATH_KNIGHT] = "dk";
    specs[CLASS_DEATH_KNIGHT][0] = "blood";
    specs[CLASS_DEATH_KNIGHT][1] = "frost";
    specs[CLASS_DEATH_KNIGHT][2] = "unholy";
#endif

    races[RACE_DWARF] = "Dwarf";
    races[RACE_GNOME] = "Gnome";
    races[RACE_HUMAN] = "Human";
    races[RACE_NIGHTELF] = "Night Elf";
    races[RACE_ORC] = "Orc";
    races[RACE_TAUREN] = "Tauren";
    races[RACE_TROLL] = "Troll";
    races[RACE_UNDEAD] = "Undead";
#ifndef MANGOSBOT_ZERO
    races[RACE_BLOODELF] = "Blood Elf";
    races[RACE_DRAENEI] = "Draenei";
#endif
}

string ChatHelper::formatMoney(uint32 copper)
{
    ostringstream out;
	if (!copper)
	{
		out << "0";
		return out.str();
	}

    uint32 gold = uint32(copper / 10000);
    copper -= (gold * 10000);
    uint32 silver = uint32(copper / 100);
    copper -= (silver * 100);

    bool space = false;
    if (gold > 0)
    {
        out << gold <<  "g";
        space = true;
    }

    if (silver > 0 && gold < 50)
    {
        if (space) out << " ";
        out << silver <<  "s";
        space = true;
    }

	if (copper > 0 && gold < 10)
	{
        if (space) out << " ";
		out << copper <<  "c";
	}

    return out.str();
}

uint32 ChatHelper::parseMoney(string& text)
{
    // if user specified money in ##g##s##c format
    string acum = "";
    uint32 copper = 0;
    for (uint8 i = 0; i < text.length(); i++)
    {
        if (text[i] == 'g')
        {
            copper += (atol(acum.c_str()) * 100 * 100);
            acum = "";
        }
        else if (text[i] == 'c')
        {
            copper += atol(acum.c_str());
            acum = "";
        }
        else if (text[i] == 's')
        {
            copper += (atol(acum.c_str()) * 100);
            acum = "";
        }
        else if (text[i] == ' ')
            break;
        else if (text[i] >= 48 && text[i] <= 57)
            acum += text[i];
        else
        {
            copper = 0;
            break;
        }
    }
    return copper;
}

ItemIds ChatHelper::parseItems(string& text)
{
    ItemIds itemIds;

    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hitem:", pos);
        if (i == -1)
            break;
        pos = i + 6;
        int endPos = text.find(':', pos);
        if (endPos == -1)
            break;
        string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            itemIds.insert(id);
    }

    return itemIds;
}

set<string> ChatHelper::parseItemQualifiers(string& text)
{
    set<string> qualifiers;

    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hitem:", pos);
        if (i == -1)
            break;
        pos = i + 6;
        int endPos = text.find('|', pos);
        if (endPos == -1)
            break;

        string qualifierString = text.substr(pos, endPos - pos);

        qualifiers.insert(qualifierString);
    }

    return qualifiers;
}

string ChatHelper::formatQuest(Quest const* quest)
{
    ostringstream out;
    int loc_idx = sPlayerbotTextMgr.GetLocalePriority();
    std::string title = quest->GetTitle();
    sObjectMgr.GetQuestLocaleStrings(quest->GetQuestId(), loc_idx, &title);
    out << "|cFFFFFF00|Hquest:" << quest->GetQuestId() << ':' << quest->GetQuestLevel() << "|h[" << title << "]|h|r";
    return out.str();
}

string ChatHelper::formatGameobject(GameObject* go)
{
    ostringstream out;
    int loc_idx = sPlayerbotTextMgr.GetLocalePriority();
    std::string name = go->GetGOInfo()->name;
    if (loc_idx >= 0)
    {
        GameObjectLocale const* gl = sObjectMgr.GetGameObjectLocale(go->GetEntry());
        if (gl)
        {
            if ((int32)gl->Name.size() > loc_idx && !gl->Name[loc_idx].empty())
                name = gl->Name[loc_idx];
        }
    }
    out << "|cFFFFFF00|Hfound:" << go->GetObjectGuid().GetRawValue() << ":" << go->GetEntry() << ":" <<  "|h[" << name << "]|h|r";
    return out.str();
}

string ChatHelper::formatWorldobject(WorldObject* wo)
{
    ostringstream out;
    int loc_idx = sPlayerbotTextMgr.GetLocalePriority();
    std::string name = (wo->IsGameObject() ? ((GameObject*)wo)->GetGOInfo()->name : wo->GetName());
    if (loc_idx >= 0 && wo->IsGameObject())
    {
        GameObjectLocale const* gl = sObjectMgr.GetGameObjectLocale(wo->GetEntry());
        if (gl)
        {
            if ((int32)gl->Name.size() > loc_idx && !gl->Name[loc_idx].empty())
                name = gl->Name[loc_idx];
        }
    }
    out << "|cFFFFFF00|Hfound:" << wo->GetObjectGuid().GetRawValue() << ":" << wo->GetEntry() << ":" << "|h[" << name << "]|h|r";
    return out.str();
}

string ChatHelper::formatWorldEntry(int32 entry)
{
    CreatureInfo const* cInfo = NULL;
    GameObjectInfo const* gInfo = NULL;

    if (entry > 0)
        cInfo = ObjectMgr::GetCreatureTemplate(entry);
    else
        gInfo = ObjectMgr::GetGameObjectInfo(entry * -1);

    ostringstream out;
    out << "|cFFFFFF00|Hentry:" << abs(entry) << ":" << "|h[";

    int loc_idx = sPlayerbotTextMgr.GetLocalePriority();
    std::string name;
    if (entry < 0 && gInfo)
        name = gInfo->name;
    else if (entry > 0 && cInfo)
        name = cInfo->Name;
    else
        name = "unknown";

    if (loc_idx >= 0 && entry < 0)
    {
        GameObjectLocale const* gl = sObjectMgr.GetGameObjectLocale(entry);
        if (gl)
        {
            if ((int32)gl->Name.size() > loc_idx && !gl->Name[loc_idx].empty())
                name = gl->Name[loc_idx];
        }
    }
    if (loc_idx >= 0 && entry > 0)
    {
        char const* tname = "";
        sObjectMgr.GetCreatureLocaleStrings(entry, loc_idx, &tname);
        if (*tname)
        {
            name = *tname;
        }
    }
    
    out << name;
    
    out << "]|h|r";
    return out.str();
}

string ChatHelper::formatSpell(SpellEntry const *sInfo)
{
    ostringstream out;
    out << "|cffffffff|Hspell:" << sInfo->Id << "|h[" << sInfo->SpellName[LOCALE_enUS] << "]|h|r";
    return out.str();
}

string ChatHelper::formatItem(ItemQualifier& itemQualifier, int count, int total)
{
    char color[32];
    ItemPrototype const* proto = itemQualifier.GetProto();
    sprintf(color, "%x", ItemQualityColors[proto->Quality]);

    ostringstream out;
    int loc_idx = sPlayerbotTextMgr.GetLocalePriority();
    std::string name = proto->Name1;
    if (loc_idx >= 0)
    {
        std::string tname;
        sObjectMgr.GetItemLocaleStrings(itemQualifier.GetId(), loc_idx, &tname);
        if (!tname.empty())
            name = tname;
    }

    if (itemQualifier.GetRandomPropertyId())
    {
        ItemRandomPropertiesEntry const* item_rand = sItemRandomPropertiesStore.LookupEntry(abs(itemQualifier.GetRandomPropertyId()));

        if (item_rand)
        {
            if (loc_idx < 0)
                loc_idx = 0;
            string suffix = item_rand->nameSuffix[loc_idx];
            name += " " + suffix;
        }
    }

    out << "|c" << color << "|Hitem:" << itemQualifier.GetLinkQualifier() << "|h[" << name
        << "]|h|r";

    if (count > 1)
        out << "x" << count;

    if (total > 0)
        out << " (" << total << ")";

    return out.str();
}

string ChatHelper::formatItem(ItemPrototype const* proto, int count, int total)
{
    ItemQualifier ItemQualifier(proto->ItemId);
    return formatItem(ItemQualifier, count, total);
}

string ChatHelper::formatItem(Item* item, int count, int total)
{
    ItemQualifier itemQualifier(item);
    return formatItem(itemQualifier, count, total);
}

string ChatHelper::formatQItem(uint32 itemId)
{
    char color[32];
    sprintf(color, "%x", ItemQualityColors[0]);

    ostringstream out;
    out << "|c" << color << "|Hitem:" << itemId
        << ":0:0:0:0:0:0:0" << "|h[item"
        << "]|h|r";

    return out.str();
}

ChatMsg ChatHelper::parseChat(string& text)
{
    if (chats.find(text) != chats.end())
        return chats[text];

    return CHAT_MSG_SYSTEM;
}

string ChatHelper::formatChat(ChatMsg chat)
{
    switch (chat)
    {
    case CHAT_MSG_GUILD:
        return "guild";
    case CHAT_MSG_PARTY:
        return "party";
    case CHAT_MSG_WHISPER:
        return "whisper";
    case CHAT_MSG_RAID:
        return "raid";
    }

    return "unknown";
}


uint32 ChatHelper::parseSpell(string& text)
{
    PlayerbotChatHandler handler(ai->GetBot());
    return handler.extractSpellId(text);
}

list<ObjectGuid> ChatHelper::parseGameobjects(string& text)
{
    list<ObjectGuid> gos;
    //    Link format
    //    |cFFFFFF00|Hfound:" << guid << ':'  << entry << ':'  <<  "|h[" << gInfo->name << "]|h|r";
    //    |cFFFFFF00|Hfound:9582:1731|h[Copper Vein]|h|r

    uint8 pos = 0;
    while (true)
    {
        // extract GO guid
        int i = text.find("Hfound:", pos);     // base H = 11
        if (i == -1)     // break if error
            break;

        pos = i + 7;     //start of window in text 11 + 7 = 18
        int endPos = text.find(':', pos);     // end of window in text 22
        if (endPos == -1)     //break if error
            break;
        istringstream stream(text.substr(pos, endPos - pos));
        uint64 guid; stream >> guid;

        // extract GO entry
        pos = endPos + 1;
        endPos = text.find(':', pos);     // end of window in text
        if (endPos == -1)     //break if error
            break;

        std::string entryC = text.substr(pos, endPos - pos);     // get string within window i.e entry
        uint32 entry = atol(entryC.c_str());     // convert ascii to float

        ObjectGuid lootCurrent = ObjectGuid(guid);

        if (guid)
            gos.push_back(lootCurrent);
    }

    return gos;
}

list<int32> ChatHelper::parseWorldEntries(string& text)
{
    list<int32> entries;
    //    Link format
    //    |cFFFFFF00|Hentry:" << entry << ':'  <<  "|h[" << gInfo->name << "]|h|r";
    //    |cFFFFFF00|Hfound:9582:1731|h[Copper Vein]|h|r

    uint8 pos = 0;
    while (true)
    {
        // extract GO guid
        int i = text.find("Hentry:", pos);     // base H = 11
        if (i == -1)     // break if error
            break;

        pos = i + 7;     //start of window in text 11 + 7 = 18
        int endPos = text.find(':', pos);     // end of window in text 22
        if (endPos == -1)     //break if error
            break;
        std::string entryC = text.substr(pos, endPos - pos);     // get string within window i.e entry        
        uint32 entry = atol(entryC.c_str());     // convert ascii to float

        if (entry)
            entries.push_back(entry);
    }

    return entries;
}

string ChatHelper::formatQuestObjective(string name, int available, int required)
{
    ostringstream out;
    out << "|cFFFFFFFF" << name << (available >= required ? "|c0000FF00: " : "|c00FF0000: ")
        << available << "/" << required << "|r";

    return out.str();
}

string ChatHelper::formatValue(string type, string topicCode, string topicName, string color)
{
    ostringstream out;
    out << "|c"<< color << "|Hvalue:" << type << ":" << topicCode << "|h[" << topicName << "]|h|r";
    return out.str();
}

string ChatHelper::parseValue(string type, string& text) {
    string retString;

    std::string pattern = "Hvalue:" + type + ":";

    int pos = text.find(pattern, 0);
    if (pos == -1)
        return retString;

    pos += pattern.size();

    int endPos = text.find('|', pos);
    if (endPos == -1)
        return retString;

    retString = text.substr(pos, endPos - pos);
    return retString;
}

uint32 ChatHelper::parseItemQuality(string text)
{
    if (itemQualities.find(text) == itemQualities.end())
        return MAX_ITEM_QUALITY;

    return itemQualities[text];
}

bool ChatHelper::parseItemClass(string text, uint32 *itemClass, uint32 *itemSubClass)
{
    if (text == "questitem")
    {
        *itemClass = ITEM_CLASS_QUEST;
        *itemSubClass = ITEM_SUBCLASS_QUEST;
        return true;
    }

    if (consumableSubClasses.find(text) != consumableSubClasses.end())
    {
        *itemClass = ITEM_CLASS_CONSUMABLE;
        *itemSubClass = consumableSubClasses[text];
        return true;
    }

    if (tradeSubClasses.find(text) != tradeSubClasses.end())
    {
        *itemClass = ITEM_CLASS_TRADE_GOODS;
        *itemSubClass = tradeSubClasses[text];
        return true;
    }

    if (projectileSubClasses.find(text) != projectileSubClasses.end())
    {
        *itemClass = ITEM_CLASS_PROJECTILE;
        *itemSubClass = projectileSubClasses[text];
        return true;
    }

    if (itemClasses.find(text) != itemClasses.end())
    {
        *itemClass = itemClasses[text].first;
        *itemSubClass = itemClasses[text].second;
        return true;
    }

    return false;
}

uint32 ChatHelper::parseSlot(string text)
{
    if (slots.find(text) != slots.end())
        return slots[text];

    return EQUIPMENT_SLOT_END;
}

bool ChatHelper::parseable(string text)
{
    return text.find("|H") != string::npos ||
        text.find("[h:") != string::npos ||
            text == "questitem" ||
            text == "ammo" ||
            substrContainsInMap<uint32>(text, consumableSubClasses) ||
            substrContainsInMap<uint32>(text, tradeSubClasses) ||
            substrContainsInMap<uint32>(text, itemQualities) ||
            (substrContainsInMap<uint32>(text, slots) && text.find("rtsc ") == string::npos) ||
            (substrContainsInMap<ChatMsg>(text, chats) && text.find(" on party") == string::npos) ||
            substrContainsInMap<uint32>(text, skills) ||
            parseMoney(text) > 0;
}

string ChatHelper::specName(Player* player)
{
    return specs[player->getClass()][AiFactory::GetPlayerSpecTab(player)]; 
}

string ChatHelper::formatClass(Player* player, int spec)
{
    uint8 cls = player->getClass();

    ostringstream out;
    out << specs[cls][spec] << " (";

    map<uint32, int32> tabs = AiFactory::GetPlayerSpecTabs(player);
    int c0 = (int)tabs[0];
    int c1 = (int)tabs[1];
    int c2 = (int)tabs[2];

    out << (c0 ? "|h|cff00ff00" : "") << c0 << "|h|cffffffff/";
    out << (c1 ? "|h|cff00ff00" : "") << c1 << "|h|cffffffff/";
    out << (c2 ? "|h|cff00ff00" : "") << c2 << "|h|cffffffff";

    out << ")|r " << classes[cls];
    return out.str();
}

string ChatHelper::formatClass(uint8 cls)
{
    return classes[cls];
}

string ChatHelper::formatRace(uint8 race)
{
    return races[race];
}

uint32 ChatHelper::parseSkill(string& text)
{
    if (skills.find(text) != skills.end())
        return skills[text];

    return SKILL_NONE;
}

string ChatHelper::formatSkill(uint32 skill)
{
    for (map<string, uint32>::iterator i = skills.begin(); i != skills.end(); ++i)
    {
        if (i->second == skill)
            return i->first;
    }

    return "";
}

string ChatHelper::formatAngle(float angle)
{
    vector<string> headings = { "north", "north west", "west", "south west", "south", "south east", "east", "north east" };

    float headingAngle = angle / M_PI_F * 180;

    return headings[int32(round(headingAngle / 45)) % 8];
}

string ChatHelper::formatWorldPosition(WorldPosition& pos)
{
    ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << pos.getX() << "," << pos.getY() << "," << pos.getZ();
    if (pos.getO())
        out << " facing " << formatAngle(pos.getO());
    if (pos.getMap())
        out << " in " << pos.getMap()->GetMapName();
    else
        out << " map:" << pos.getMapId();

    return out.str();
}


string ChatHelper::formatGuidPosition(GuidPosition& guidP)
{
    ostringstream out;
    if (guidP.GetWorldObject())
        out << guidP.GetWorldObject()->GetName();
    else if (guidP.GetCreatureTemplate())
        out << guidP.GetCreatureTemplate()->Name;
    else if (guidP.GetGameObjectInfo())
        out << guidP.GetGameObjectInfo()->name;
    else
        out << guidP.GetRawValue();

    if (WorldPosition(guidP))
        out << " " << formatWorldPosition(guidP);

    return out.str();
}




string ChatHelper::formatBoolean(bool flag)
{
    return flag ? "|cff00ff00ON|r" : "|cffffff00OFF|r";
}

void ChatHelper::eraseAllSubStr(std::string& mainStr, const std::string& toErase)
{
    size_t pos = std::string::npos;
    // Search for the substring in string in a loop untill nothing is found
    while ((pos = mainStr.find(toErase)) != std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

inline std::string toInitCap(const std::string& str) {
    std::string result = str;
    bool isFirstChar = true;
    for (char& c : result) {
        if (isFirstChar && std::isalpha(c)) {
            c = std::toupper(c);
            isFirstChar = false;
        }
        else if (std::isspace(c)) {
            isFirstChar = true;
        }
        else {
            c = std::tolower(c);
        }
    }
    return result;
}

void ChatHelper::PopulateSpellNameList()
{
    for (uint32 i = 0; i < GetSpellStore()->GetMaxEntry(); ++i)
    {
        SpellEntry const* tempSpell = GetSpellStore()->LookupEntry<SpellEntry>(i);

        if (!tempSpell)
            continue;

        string lowerSpellName = tempSpell->SpellName[0];
        strToLower(lowerSpellName);

        spellIds[tempSpell->SpellName[0]].push_back(tempSpell->Id);
        spellIds[lowerSpellName].push_back(tempSpell->Id);
    }
}

vector<uint32> ChatHelper::SpellIds(const string& name)
{
    auto spells = spellIds.find(name);

    if (spells == spellIds.end())
        return {};

    return spells->second;
}