#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class BGStrategy : public PassTroughStrategy
    {
    public:
        BGStrategy(PlayerbotAI* ai);
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "bg"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "bg"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots queue up for battle grounds remotely and join them when they get an invite.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground" }; }
#endif
    };

    class BattlegroundStrategy : public Strategy
    {
    public:
        BattlegroundStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "battleground"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "battleground"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy gives bots basic behavior inside battle grounds like checking and moving to objectives and getting ready at the start gates.";
        }
        virtual vector<string> GetRelatedStrategies() { return {"bg", "warsong" ,"arathi", "alterac", "eye", "isle",  "arena" }; }
#endif
    };

    class WarsongStrategy : public Strategy
    {
    public:
        WarsongStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "warsong"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "warsong"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior during a warsong gluch battleground like capturing/retaking flags and picking up buffs.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground", "bg" }; }
#endif
    };

    class AlteracStrategy : public Strategy
    {
    public:
        AlteracStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "alterac"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "alterac"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior during an alertac valley battleground.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground","bg" }; }
#endif
    };

    class ArathiStrategy : public Strategy
    {
    public:
        ArathiStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "arathi"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "arathi"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior during an arathi basin battleground.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground","bg" }; }
#endif
    };

    class EyeStrategy : public Strategy
    {
    public:
        EyeStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
        virtual string getName() { return "eye"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "eye"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior during an eye of the storm basin battleground.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground","bg" }; }
#endif
    };

    class IsleStrategy : public Strategy
    {
    public:
        IsleStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
        virtual string getName() { return "isle"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "isle"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior during an isle of conquest battleground.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground","bg" }; }
#endif
    };

    class ArenaStrategy : public Strategy
    {
    public:
        ArenaStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "arena"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "arena"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy controls the behavior arena fight.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "battleground","bg" }; }
#endif
    };
}
