#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class ChatCommandHandlerStrategy : public PassTroughStrategy
    {
    public:
        ChatCommandHandlerStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "chat"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "chat"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots respond to various chat commands.";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif
    };
}
