#pragma once

namespace ai
{
    class RpgActionMultiplier : public Multiplier
    {
    public:
        RpgActionMultiplier(PlayerbotAI* ai) : Multiplier(ai, "rpg action") {}

    public:
        virtual float GetValue(Action* action);
    };

    class RpgStrategy : public Strategy
    {
    public:
        RpgStrategy(PlayerbotAI* ai);
        virtual string getName() { return "rpg"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "rpg"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy makes bot move between npcs to automatically do various interaction.\n"
            "This is the main rpg strategy which make bots pickand move to various rpg targets.\n"
            "The interactions included in this strategy are limited to emotesand wait.";            
        }
        virtual vector<string> GetRelatedStrategies() { return {"rpg quest", "rpg vendor", "rpg explore", "rpg maintenance", "rpg guild", "rpg bg", "rpg player", "rpg craft"}; }
#endif
    public:
        //virtual NextAction** getDefaultActions();
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        void InitMultipliers(std::list<Multiplier*>& multipliers);
    };

    class RpgQuestStrategy : public RpgStrategy
    {
    public:
        RpgQuestStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg quest"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgVendorStrategy : public RpgStrategy
    {
    public:
        RpgVendorStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg vendor"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgExploreStrategy : public RpgStrategy
    {
    public:
        RpgExploreStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg explore"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgMaintenanceStrategy : public RpgStrategy
    {
    public:
        RpgMaintenanceStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg maintenance"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgGuildStrategy : public RpgStrategy
    {
    public:
        RpgGuildStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg guild"; }
    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgBgStrategy : public RpgStrategy
    {
    public:
        RpgBgStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg bg"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgPlayerStrategy : public RpgStrategy
    {
    public:
        RpgPlayerStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg player"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

    class RpgCraftStrategy : public RpgStrategy
    {
    public:
        RpgCraftStrategy(PlayerbotAI* ai) : RpgStrategy(ai) {};
        virtual string getName() { return "rpg craft"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
    };

}
