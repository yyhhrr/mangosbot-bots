#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "../values/RTSCValues.h"

namespace ai
{
    class SeeSpellAction : public MovementAction
    {
    public:
        SeeSpellAction(PlayerbotAI* ai, string name = "see spell") : MovementAction(ai, name) {}
        virtual bool Execute(Event& event);

        virtual bool isPossible() override { return true; }
        virtual bool isUseful() override;

        bool SelectSpell(Player* requester, WorldPosition& spellPosition);

        virtual bool MoveToSpell(Player* requester, WorldPosition& spellPosition, bool inFormation = true);
        void SetFormationOffset(Player* requester, WorldPosition& spellPosition);
    private:
        Creature* CreateWps(Player* wpOwner, float x, float y, float z, float o, uint32 entry, Creature* lastWp, bool important = false);
    };
}