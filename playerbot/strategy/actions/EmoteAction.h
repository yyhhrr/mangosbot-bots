#pragma once

#include "../Action.h"

namespace ai
{
    class EmoteActionBase : public Action
    {
    public:
        EmoteActionBase(PlayerbotAI* ai, string name);
        static uint32 GetNumberOfEmoteVariants(TextEmotes emote, uint8 race, uint8 gender);

    protected:
        bool Emote(Unit* target, uint32 type, bool textEmote = false);
        bool ReceiveEmote(Player* requester, Player* source, uint32 emote, bool verbal = false);
        Unit* GetTarget();
        void InitEmotes();
        static map<string, uint32> emotes;
        static map<string, uint32> textEmotes;

    };

    class EmoteAction : public EmoteActionBase, public Qualified
    {
    public:
        EmoteAction(PlayerbotAI* ai);
        virtual bool Execute(Event& event);
        bool isUseful();
    };

    class TalkAction : public EmoteActionBase
    {
    public:
        TalkAction(PlayerbotAI* ai) : EmoteActionBase(ai, "talk") {}
        virtual bool Execute(Event& event);

        static uint32 GetRandomEmote(Unit* unit, bool textEmote = false);
    };

    class MountAnimAction : public Action
    {
    public:
        MountAnimAction(PlayerbotAI* ai, string name = "mount anim") : Action(ai, name) {}

        virtual bool isUseful();
        virtual bool Execute(Event& event);
    };
}
