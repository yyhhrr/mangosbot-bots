#include "botpch.h"
#include "../../playerbot.h"
#include "AttackersValue.h"
#include "PossibleTargetsValue.h"
#include "EnemyPlayerValue.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> AttackersValue::Calculate()
{
    list<ObjectGuid> result;
    if (!ai->AllowActivity(ALL_ACTIVITY))
        return result;

    if (!bot->IsInWorld())
        return result;

    if (bot->IsBeingTeleported())
        return result;

    if (bot->IsFlying() && WorldPosition(bot).currentHeight() > 10.0f)
        return result;

    
    // Try to get the value from nearby friendly bots.
    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
    for (auto& i : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(i);

        if (!player)
            continue;

        if (player == bot)
            continue;

        if (player->GetMapId() != bot->GetMapId())
            continue;

        if (sServerFacade.GetDistance2d(bot, player) > 10.0f)
            continue;

        PlayerbotAI* botAi = player->GetPlayerbotAI();

        if (!botAi)
            continue;

        string valueName = "attackers" + !qualifier.empty() ? "::" + qualifier : "";

        // Ignore bots without the value.
        if (!PHAS_AI_VALUE(valueName))
            continue;

        UntypedValue* pValue = botAi->GetAiObjectContext()->GetUntypedValue(valueName);

        AttackersValue* pAttackersValue = dynamic_cast<AttackersValue*>(pValue);

        if (!pAttackersValue)
            continue;

        // Ignore expired values.
        if (pAttackersValue->Expired())
            continue;

        if (pAttackersValue->calculatePos.sqDistance2d(bot) > 100.0f)
            continue;

        // Make the value expire at the same time as the copied value.
        lastCheckTime = pAttackersValue->lastCheckTime;

        calculatePos = pAttackersValue->calculatePos;

        result = PAI_VALUE(list<ObjectGuid>, valueName);

        vector<string> specificTargetNames = { "current target","old target","attack target","pull target" };
        Unit* target;

        //Remove bot specific targets of the other bot.
        for (auto& targetName : specificTargetNames)
        {
            target = (targetName == "attack target") ? ai->GetUnit(PAI_VALUE(ObjectGuid, targetName)) : PAI_VALUE(Unit*,  targetName);
            if (target)
                result.remove(target->GetObjectGuid());
        }

        //Add bot specific targets of this bot.
        for (auto& targetName : specificTargetNames)
        {
            target = (targetName == "attack target") ? ai->GetUnit(AI_VALUE(ObjectGuid, targetName)) : AI_VALUE(Unit*, targetName);
            if (target)
                result.push_back(target->GetObjectGuid());
        }
                   
        return result;
    }
    
    calculatePos = bot;

    set<Unit*> targets;

    // Check if we only need one attacker
    bool getOne = false;
    if (!qualifier.empty())
    {
        getOne = stoi(qualifier);
    }

    set<ObjectGuid> invalidTargets;

    // Add the targets of the bot
    AddTargetsOf(bot, targets, invalidTargets, getOne);

    // Don't check for group member targets if we only need one
    if (targets.empty() || !getOne)
    {
        // Add the targets of the members of the group
        Group* group = bot->GetGroup();
        if (group && !bot->InBattleGround())
        {
            AddTargetsOf(group, targets, invalidTargets, getOne);
        }

        Player* master = GetMaster();
        if (master && !bot->InBattleGround() && (!group || master->GetGroup() != group))
            AddTargetsOf(master, targets, invalidTargets, getOne);
    }

    // Convert the targets to guids
    for (Unit* target : targets)
    {
        result.push_back(target->GetObjectGuid());
    }

    return result;
}

void AttackersValue::AddTargetsOf(Group* group, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        // Only add group member targets that are alive and near the player
        Player* member = sObjectMgr.GetPlayer(itr->guid);
        if (member && (member != bot) &&
           sServerFacade.IsAlive(member) &&
           member->IsInWorld() &&
           !member->IsBeingTeleported() &&
           (member->GetMapId() == bot->GetMapId()) && 
           (sServerFacade.GetDistance2d(bot, member) <= GetRange()))
        {
            AddTargetsOf(member, targets, invalidTargets, getOne);

            // Finish early if we only need one target
            if (getOne && !targets.empty())
            {
                break;
            }
        }
    }
}

void AttackersValue::AddTargetsOf(Player* player, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne)
{
    // If the player is available
    if (ai->IsSafe(player))
    {
        set<Unit*> units;

        // If the player is a bot
        PlayerbotAI* playerBot = player->GetPlayerbotAI();
        if (playerBot)
        {
            // Get all the units around the player
            // NOTE: We don't validate the value here because it will be validated later on
            const string ignoreValidate = std::to_string(true);
            const string range = std::to_string((int32)GetRange());
            const vector<string> qualifiers = { range, ignoreValidate };
            const list<ObjectGuid> possibleTargets = PAI_VALUE2(list<ObjectGuid>, "possible targets", Qualified::MultiQualify(qualifiers, ":"));
            for (const ObjectGuid& guid : possibleTargets)
            {
                if (Unit* unit = ai->GetUnit(guid))
                {
                    units.insert(unit);
                }
            }

            // Add the current target
            Unit* currentTarget = PAI_VALUE(Unit*, "current target");
            if (currentTarget)
            {
                units.insert(currentTarget);
            }

            // Add the previous target
            Unit* oldTarget = PAI_VALUE(Unit*, "old target");
            if (oldTarget)
            {
                units.insert(oldTarget);
            }

            // Add the pull and attack targets (Only consider the owner bot)
            if (bot == player)
            {
                Unit* attackTarget = playerBot->GetUnit(PAI_VALUE(ObjectGuid, "attack target"));
                if (attackTarget)
                {
                    units.insert(attackTarget);
                }

                Unit* pullTarget = PAI_VALUE(Unit*, "pull target");
                if (pullTarget)
                {
                    units.insert(pullTarget);
                }
            }
        }

        // Get the current attackers of the player
        for (Unit* attacker : player->getAttackers())
        {
            units.insert(attacker);
        }

        // Add the duel opponent (Only consider the owner bot)
        if (bot == player && bot->duel && bot->duel->opponent)
        {
            units.insert(bot->duel->opponent);
        }

        // Add the pet attackers (if nearby)
        Pet* pet = player->GetPet();
        if (pet && sServerFacade.GetDistance2d(bot, pet) <= GetRange())
        {
            for (Unit* attacker : pet->getAttackers())
            {
                units.insert(attacker);
            }
        }

        // Filter the units that are valid
        for (Unit* unit : units)
        {
            // Prevent checking a target that has already been validated
            if((targets.find(unit) == targets.end()))
            {
                // Prevent checking a target that has already been invalidated
                if(InCombat(unit, player) || (invalidTargets.find(unit->GetObjectGuid()) == invalidTargets.end()))
                {
                    if (IsValid(unit, player, bot))
                    {
                        // Add the target to the list of combat targets
                        targets.insert(unit);

                        // Add the target's pet/guardian too
                        unit->CallForAllControlledUnits(AddGuardiansHelper(targets), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);

                        if (getOne)
                        {
                            break;
                        }
                    }
                    else
                    {
                        invalidTargets.insert(unit->GetObjectGuid());
                    }
                }
            }
        }
    }
}

bool AttackersValue::InCombat(Unit* target, Player* player, bool checkPullTargets)
{
    // Check if the the target is attacking the player
    bool inCombat = (target->getThreatManager().getThreat(player) > 0.0f) ||
                    (target->GetVictim() && (target->GetVictim() == player));

    // Check if the target is attacking the player's pet
    if(!inCombat)
    {
        Pet* pet = player->GetPet();
        if (pet)
        {
            inCombat = (target->getThreatManager().getThreat(pet) > 0.0f) ||
                       (target->GetVictim() && (target->GetVictim() == pet));
        }
    }

    if(!inCombat && checkPullTargets && player->GetPlayerbotAI())
    {
        inCombat = (PAI_VALUE(ObjectGuid, "attack target") == target->GetObjectGuid()) ||
                   (PAI_VALUE(Unit*, "pull target") == target);
    }

    return inCombat;
}

bool AttackersValue::IsValid(Unit* target, Player* player, Player* owner, bool checkInCombat, bool validatePossibleTarget)
{
    Player* playerToCheckAgainst = owner != nullptr ? owner : player;

    // Validate possible target
    if (validatePossibleTarget && !PossibleTargetsValue::IsValid(target, playerToCheckAgainst))
    {
        return false;
    }

    // This will be used on both enemy player and npc checks
    const bool inPvPProhibitedZone = sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(target));

    // If the target is a player
    Player* enemyPlayer = dynamic_cast<Player*>(target);
    if (enemyPlayer)
    {
        // Don't consider enemy players if pvp strategy is not set
        if (playerToCheckAgainst->GetPlayerbotAI() && !playerToCheckAgainst->GetPlayerbotAI()->HasStrategy("pvp", BotState::BOT_STATE_COMBAT))
        {
            return false;
        }

        // If the enemy player is in a PVP Prohibited zone
        if (inPvPProhibitedZone)
        {
            return false;
        }

        // Don't check distance on duel opponents
        if (!player->duel || (player->duel && (player->duel->opponent != target)))
        {
            // If the enemy player is not within sight distance
            if (!enemyPlayer->IsWithinDist(playerToCheckAgainst, EnemyPlayerValue::GetMaxAttackDistance(playerToCheckAgainst), false))
            {
                return false;
            }
        }
    }
    // If the target is a NPC
    else
    {
        // If the target is not fighting the player (and if the owner bot is not pulling the target)
        if (checkInCombat && !InCombat(target, player, (player == owner)))
        {
            return false;
        }

        // If the target is a player's pet and in a PvP prohibited zone
        if (target->GetObjectGuid().IsPet() && inPvPProhibitedZone)
        {
            return false;
        }

        // If the target is evading
        const Creature* creature = dynamic_cast<Creature*>(target);
        if (creature)
        {
            if (creature->GetCombatManager().IsInEvadeMode())
            {
                return false;
            }
        }

        if (WorldPosition(player).isOverworld() &&  target->AI() && target->AI()->IsPreventingDeath())
        {
            return false;
        }
    }

    return true;
}

string AttackersValue::Format()
{
    ostringstream out;

    for (auto& target : value)
    {
        if(target != value.front())
        out << ",";

        WorldObject* wo = ai->GetWorldObject(target);

        if (wo)
            out << wo->GetName();
        else
            out << target;      
    }

    return out.str().c_str();
}

