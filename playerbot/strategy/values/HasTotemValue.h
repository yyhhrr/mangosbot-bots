#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "../../LootObjectStack.h"

namespace ai
{
    class HasTotemValue : public BoolCalculatedValue, public Qualified
	{
	public:
        HasTotemValue(PlayerbotAI* ai, string name = "has totem") : BoolCalculatedValue(ai, name), Qualified() {}

        bool Calculate()
        {
            list<ObjectGuid> units = *context->GetValue<list<ObjectGuid> >("nearest npcs");
            for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); i++)
            {
                Unit* unit = ai->GetUnit(*i);
                if (!unit)
                    continue;

                Creature* creature = dynamic_cast<Creature*>(unit);
                if (!creature || !creature->IsTotem())
                    continue;

                if (strstri(creature->GetName(), qualifier.c_str()) && sServerFacade.GetDistance2d(bot, creature) <= ai->GetRange("spell"))
                    return true;
            }

            return false;
        }
    };
}
