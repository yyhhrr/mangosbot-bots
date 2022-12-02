#include "../generic/NonCombatStrategy.h"
#pragma once

namespace ai
{
    class AttackEnemyPlayersStrategy : public Strategy
    {
    public:
        AttackEnemyPlayersStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual string getName() { return "pvp"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "pvp"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy detect nearby enemy players and makes the bot attack them.";
        }
        virtual vector<string> GetRelatedStrategies() { return {}; }
#endif
    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

}
