#include "botpch.h"
#include "../../playerbot.h"
#include "SetHomeAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool SetHomeAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    ObjectGuid selection = bot->GetSelectionGuid();
    bool isRpgAction = AI_VALUE(GuidPosition, "rpg target") == selection;

    if (!isRpgAction)
    {
        if (requester)
        {
            selection = requester->GetSelectionGuid();
        }
        else
        {
            return false;
        }
    }

    if (selection)
    {
        Unit* unit = ai->GetUnit(selection);
        if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_INNKEEPER))
        {
            if (isRpgAction)
            {
                Creature* creature = ai->GetCreature(selection);                   
                bot->GetSession()->SendBindPoint(creature);
                ai->TellPlayer(requester, "This inn is my new home", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
                RESET_AI_VALUE(WorldPosition, "home bind");
                return true;
            }
            else
            {
                Creature* creature = ai->GetCreature(selection);
                bot->GetSession()->SendBindPoint(creature);
                ai->TellPlayer(requester, "This inn is my new home", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
                RESET_AI_VALUE(WorldPosition, "home bind");
                return true;
            }
        }
    }

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Creature *unit = bot->GetNPCIfCanInteractWith(*i, UNIT_NPC_FLAG_INNKEEPER);
        if (!unit)
            continue;

        bot->GetSession()->SendBindPoint(unit);
        ai->TellPlayer(requester, "This inn is my new home", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
        RESET_AI_VALUE(WorldPosition, "home bind");
        return true;
    }

    ai->TellPlayer(requester, "Can't find any innkeeper around");
    return false;
}
