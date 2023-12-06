#pragma once
#include "../../LootObjectStack.h"
#include "GenericActions.h"

namespace ai
{
    class LootStrategyAction : public ChatCommandAction
    {
    public:
        LootStrategyAction(PlayerbotAI* ai) : ChatCommandAction(ai, "ll") {}
        virtual bool Execute(Event& event) override;

    private:
        void TellLootList(Player* requester, const string& name);
        void TellGoList(Player* requester, const string& name);
    };
}
