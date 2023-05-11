#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{

    class ItemForSpellValue : public CalculatedValue<Item*>, public Qualified
	{
	public:
        ItemForSpellValue(PlayerbotAI* ai, string name = "item for spell") : CalculatedValue<Item*>(ai, name), Qualified() {}
        virtual Item* Calculate();

    private:
        Item* ItemForSpellValue::GetItemFitsToSpellRequirements(Item* itemForSpell, SpellEntry const* spellInfo);
        Item* GetItemFitsToSpellRequirements(uint8 slot, SpellEntry const *spellInfo);
    };
}
