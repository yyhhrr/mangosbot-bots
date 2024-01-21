#include "botpch.h"
#include "../../playerbot.h"
#include "TargetValue.h"

#include "../../ServerFacade.h"
#include "RtiTargetValue.h"
#include "Unit.h"
#include "LastMovementValue.h"
#include "../values/Formations.h"

using namespace ai;

Unit* TargetValue::FindTarget(FindTargetStrategy* strategy)
{
    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible attack targets")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit)
            continue;

        ThreatManager &threatManager = sServerFacade.GetThreatManager(unit);
        strategy->CheckAttacker(unit, &threatManager);
    }

    return strategy->GetResult();
}

bool FindNonCcTargetStrategy::IsCcTarget(Unit* attacker)
{
    Group* group = ai->GetBot()->GetGroup();
    if (group)
    {
        Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *player = sObjectMgr.GetPlayer(itr->guid);
            if (!player || !sServerFacade.IsAlive(player) || ai->IsSafe(player))
                continue;

            if (player->GetPlayerbotAI())
            {
                if (PAI_VALUE(Unit*,"rti cc target") == attacker)
                    return true;

                string rti = PAI_VALUE(string,"rti cc");
                int index = RtiTargetValue::GetRtiIndex(rti);
                if (index != -1)
                {
                    uint64 guid = group->GetTargetIcon(index);
                    if (guid && attacker->GetObjectGuid() == ObjectGuid(guid))
                        return true;
                }
            }
        }

        uint64 guid = group->GetTargetIcon(4);
        if (guid && attacker->GetObjectGuid() == ObjectGuid(guid))
            return true;
    }

    return false;
}

void FindTargetStrategy::GetPlayerCount(Unit* creature, int* tankCount, int* dpsCount)
{
    Player* bot = ai->GetBot();
    if (tankCountCache.find(creature) != tankCountCache.end())
    {
        *tankCount = tankCountCache[creature];
        *dpsCount = dpsCountCache[creature];
        return;
    }

    *tankCount = 0;
    *dpsCount = 0;

    Unit::AttackerSet attackers(creature->getAttackers());
    for (set<Unit*>::const_iterator i = attackers.begin(); i != attackers.end(); i++)
    {
        Unit* attacker = *i;
        if (!attacker || !sServerFacade.IsAlive(attacker) || attacker == bot)
            continue;

        Player* player = dynamic_cast<Player*>(attacker);
        if (!player)
            continue;

        if (ai->IsTank(player))
            (*tankCount)++;
        else
            (*dpsCount)++;
    }

    tankCountCache[creature] = *tankCount;
    dpsCountCache[creature] = *dpsCount;
}

WorldPosition LastLongMoveValue::Calculate()
{
    LastMovement& lastMove = *context->GetValue<LastMovement&>("last movement");

    if (lastMove.lastPath.empty())
        return WorldPosition();

    return lastMove.lastPath.getBack();
}

WorldPosition HomeBindValue::Calculate()
{
    float x, y, z;
    uint32 mapId;
    bot->GetHomebindLocation(x, y, z, mapId);
    return WorldPosition(mapId, x, y, z, 0.0);
}

void PullTargetValue::Set(Unit* unit)
{
    guid = unit ? unit->GetObjectGuid() : ObjectGuid();
}

Unit* PullTargetValue::Get()
{
    Unit* unit = nullptr;
    if (!guid.IsEmpty())
    {
        unit = sObjectAccessor.GetUnit(*bot, guid);
    }
    
    return unit;
}

Unit* FollowTargetValue::Calculate()
{
    Unit* followTarget = AI_VALUE(GuidPosition, "manual follow target").GetUnit();
    if (followTarget == nullptr)
    {
        Formation* formation = AI_VALUE(Formation*, "formation");
        if (formation && !formation->GetTargetName().empty())
        {
            followTarget = AI_VALUE(Unit*, formation->GetTargetName());
        }
        else
        {
            followTarget = AI_VALUE(Unit*, "master target");
        }
    }

    return followTarget;
}

Unit* ClosestAttackerTargetingMeTargetValue::Calculate()
{
    Unit* result = nullptr;
    float closest = 9999.0f;

    const std::list<ObjectGuid>& attackers = AI_VALUE(list<ObjectGuid>, "attackers targeting me");
    for (const ObjectGuid& attackerGuid : attackers)
    {
        Unit* attacker = ai->GetUnit(attackerGuid);
        if (attacker)
        {
            const float distance = bot->GetDistance(attacker, true, DIST_CALC_COMBAT_REACH);
            if (distance < closest)
            {
                closest = distance;
                result = attacker;
            }
        }
    }

    return result;
}
