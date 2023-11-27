#pragma once
#include "../../LootObjectStack.h"
#include "EquipAction.h"

namespace ai
{
    class OutfitAction : public EquipAction 
    {
    public:
        OutfitAction(PlayerbotAI* ai) : EquipAction(ai, "outfit") {}
        virtual bool Execute(Event& event) override;

    private:
        void List(Player* requester);
        void Save(string name, ItemIds outfit);
        void Update(string name);
    };
}
