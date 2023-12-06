#include "../botpch.h"
#include "LootObjectStack.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "ServerFacade.h"
#include "strategy/values/SharedValueContext.h"

using namespace ai;
using namespace std;

#define MAX_LOOT_OBJECT_COUNT 10

LootTarget::LootTarget(ObjectGuid guid) : guid(guid), asOfTime(time(0))
{
}

LootTarget::LootTarget(LootTarget const& other)
{
    guid = other.guid;
    asOfTime = other.asOfTime;
}

LootTarget& LootTarget::operator=(LootTarget const& other)
{
    if((void*)this == (void*)&other)
        return *this;

    guid = other.guid;
    asOfTime = other.asOfTime;

    return *this;
}

bool LootTarget::operator< (const LootTarget& other) const
{
    return guid < other.guid;
}

void LootTargetList::shrink(time_t fromTime)
{
    for (set<LootTarget>::iterator i = begin(); i != end(); )
    {
        if (i->asOfTime <= fromTime)
            erase(i++);
		else
			++i;
    }
}

LootObject::LootObject(Player* bot, ObjectGuid guid)
	: guid(), skillId(SKILL_NONE), reqSkillValue(0), reqItem(0)
{
    Refresh(bot, guid);
}

void LootObject::Refresh(Player* bot, ObjectGuid guid)
{
    skillId = SKILL_NONE;
    reqSkillValue = 0;
    reqItem = 0;
    this->guid = ObjectGuid();

    PlayerbotAI* ai = bot->GetPlayerbotAI();
    Creature *creature = ai->GetCreature(guid);
    if (creature && sServerFacade.GetDeathState(creature) == CORPSE)
    {
        if (creature->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
            this->guid = guid;

        if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
        {
            skillId = creature->GetCreatureInfo()->GetRequiredLootSkill();
            uint32 targetLevel = creature->GetLevel();
            reqSkillValue = targetLevel < 10 ? 1 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;
            if (ai->HasSkill((SkillType)skillId) && bot->GetSkillValue(skillId) >= reqSkillValue)
                this->guid = guid;
        }

        return;
    }

    GameObject* go = ai->GetGameObject(guid);
    if (go && sServerFacade.isSpawned(go) 
#ifdef CMANGOS
        && !go->IsInUse() 
#endif
        && (go->GetGoState() == GO_STATE_READY || go->GetGoState() == GO_STATE_ACTIVE))
    {
        bool isQuestItemOnly = false;

#ifdef MANGOSBOT_TWO
        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; i++)
        {
            int itemId = go->GetGOInfo()->questItems[i];

            if (IsNeededForQuest(bot, itemId))
            {
                this->guid = guid;
                return;
            }
            isQuestItemOnly |= itemId > 0;
        }
#else
        /*if (!guid.IsEmpty())
        {
            for (auto& entry : GAI_VALUE2(list<int32>, "item drop list", -go->GetEntry()))
            {
                if (IsNeededForQuest(bot, entry))
                {
                    this->guid = guid;
                    return;
                }
                isQuestItemOnly |= entry > 0;
            }
        }*/
#endif

        if (isQuestItemOnly)
            return;

        uint32 goId = go->GetGOInfo()->id;
        set<uint32>& skipGoLootList = ai->GetAiObjectContext()->GetValue<set<uint32>& >("skip go loot list")->Get();
        if (skipGoLootList.find(goId) != skipGoLootList.end()) return;

        uint32 lockId = go->GetGOInfo()->GetLockId();
        LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
        if (!lockInfo)
            return;

        for (int i = 0; i < 8; ++i)
        {
            switch (lockInfo->Type[i])
            {
            case LOCK_KEY_ITEM:
                if (lockInfo->Index[i] > 0)
                {
                    reqItem = lockInfo->Index[i];
                    this->guid = guid;
                }
                break;
            case LOCK_KEY_SKILL:
                if (goId == 13891 || goId == 19535) // Serpentbloom
                {
                    this->guid = guid;
                }
                else if (SkillByLockType(LockType(lockInfo->Index[i])) > 0)
                {
                    skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                    reqSkillValue = max((uint32)1, lockInfo->Skill[i]);
                    this->guid = guid;
                }
                break;
            case LOCK_KEY_NONE:
                this->guid = guid;
                break;
            }
        }
    }
}

bool LootObject::IsNeededForQuest(Player* bot, uint32 itemId)
{
    for (int qs = 0; qs < MAX_QUEST_LOG_SIZE; ++qs)
    {
        uint32 questId = bot->GetQuestSlotQuestId(qs);
        if (questId == 0)
            continue;

        QuestStatusData& qData = bot->getQuestStatusMap()[questId];
        if (qData.m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questId);
        if (!qInfo)
            continue;

        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
        {
            if (!qInfo->ReqItemCount[i] || (qInfo->ReqItemCount[i] - qData.m_itemcount[i]) <= 0)
                continue;

            if (qInfo->ReqItemId[i] != itemId)
                continue;

            return true;
        }
    }

    return false;
}

WorldObject* LootObject::GetWorldObject(Player* bot)
{
    Refresh(bot, guid);

    PlayerbotAI* ai = bot->GetPlayerbotAI();

    Creature *creature = ai->GetCreature(guid);
    if (creature && sServerFacade.GetDeathState(creature) == CORPSE)
        return creature;

    GameObject* go = ai->GetGameObject(guid);
    if (go && sServerFacade.isSpawned(go))
        return go;

    return NULL;
}

LootObject::LootObject(const LootObject& other)
{
    guid = other.guid;
    skillId = other.skillId;
    reqSkillValue = other.reqSkillValue;
    reqItem = other.reqItem;
}

bool LootObject::IsLootPossible(Player* bot)
{
    if (IsEmpty() || !GetWorldObject(bot))
        return false;

    PlayerbotAI* ai = bot->GetPlayerbotAI();

    if (reqItem && !bot->HasItemCount(reqItem, 1))
        return false;

    if (guid.IsCreature())
    {
        Creature* creature = ai->GetCreature(guid);
        if (creature && sServerFacade.GetDeathState(creature) == CORPSE)
        {
            if (creature->m_loot && skillId != SKILL_SKINNING)
                if (!creature->m_loot->CanLoot(bot))
                    return false;
        }
    }

    AiObjectContext* context = ai->GetAiObjectContext();

    if (!AI_VALUE2_LAZY(bool, "should loot object", to_string(guid.GetRawValue())))
        return false;


    if (guid.IsGameObject())
    {
        GameObject* go = ai->GetGameObject(guid);
        if (go)
        {
            if (sObjectMgr.IsGameObjectForQuests(guid.GetEntry())) //If object has quest loot bot needs the quest.
                if (!go->ActivateToQuest(bot))
                    return false;
        }
    }

    if (skillId == SKILL_NONE)
        return true;

    if (skillId == SKILL_FISHING)
        return false;

    if (!ai->HasSkill((SkillType)skillId))
        return false;

    if (!reqSkillValue)
        return true;

    uint32 skillValue = uint32(bot->GetSkillValue(skillId));
    if (reqSkillValue > skillValue)
        return false;

    if (skillId == SKILL_MINING && !bot->HasItemCount(2901, 1))
        return false;

    if (skillId == SKILL_SKINNING && !bot->HasItemCount(7005, 1))
        return false;

    return true;
}

bool LootObjectStack::Add(ObjectGuid guid)
{
    if (!availableLoot.insert(guid).second)
        return false;

    if (availableLoot.size() < MAX_LOOT_OBJECT_COUNT)
        return true;

    vector<LootObject> ordered = OrderByDistance();
    for (size_t i = MAX_LOOT_OBJECT_COUNT; i < ordered.size(); i++)
        Remove(ordered[i].guid);

    return true;
}

void LootObjectStack::Remove(ObjectGuid guid)
{
    LootTargetList::iterator i = availableLoot.find(guid);
    if (i != availableLoot.end())
        availableLoot.erase(i);
}

void LootObjectStack::Clear()
{
    availableLoot.clear();
}

bool LootObjectStack::CanLoot(float maxDistance)
{
    vector<LootObject> ordered = OrderByDistance(maxDistance);
    return !ordered.empty();
}

LootObject LootObjectStack::GetLoot(float maxDistance)
{
    vector<LootObject> ordered = OrderByDistance(maxDistance);
    return ordered.empty() ? LootObject() : *ordered.begin();
}

vector<LootObject> LootObjectStack::OrderByDistance(float maxDistance)
{
    availableLoot.shrink(time(0) - 30);

    map<float, LootObject> sortedMap;
    LootTargetList safeCopy(availableLoot);
    for (LootTargetList::iterator i = safeCopy.begin(); i != safeCopy.end(); i++)
    {
        ObjectGuid guid = i->guid;
        LootObject lootObject(bot, guid);
        if (!lootObject.IsLootPossible(bot))
            continue;

        float distance = sqrt(bot->GetDistance(lootObject.GetWorldObject(bot)));
        if (!maxDistance || distance <= maxDistance)
            sortedMap[distance] = lootObject;
    }

    vector<LootObject> result;
    for (map<float, LootObject>::iterator i = sortedMap.begin(); i != sortedMap.end(); i++)
        result.push_back(i->second);
    return result;
}

