#include "botpch.h"
#include "../../playerbot.h"
#include "RacialsStrategy.h"

using namespace ai;


class RacialsStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    RacialsStrategyActionNodeFactory()
    {    
        creators["will of the forsaken"] = &will_of_the_forsaken; //undead
        creators["war stomp"] = &war_stomp;//tauren
        creators["berserking"] = &berserking;//troll
        creators["blood fury"] = &blood_fury;//orc
        creators["cannibalize"] = &cannibalize;//undead
        creators["shadowmeld"] = &shadowmeld;//Nnightelf
        creators["escape artist"] = &escape_artist;//gnome
        creators["stoneform"] = &stoneform;//dwarf
        creators["perception"] = &perception;//human
#ifndef MANGOSBOT_ZERO
        creators["arcane torrent"] = &arcane_torrent;//bloodelf 
        creators["mana tap"] = &mana_tap;//bloodelf
        creators["gift of the naaru"] = &gift_of_the_naaru;//dranei
#endif 
#ifdef MANGOSBOT_TWO
        creators["every man for himself"] = &every_man_for_himself; //human
#endif    
    }
private:

    static ActionNode* will_of_the_forsaken(PlayerbotAI* ai)
    {
        return new ActionNode("will of the forsaken",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* war_stomp(PlayerbotAI* ai)
    {
        return new ActionNode("war stomp",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* berserking(PlayerbotAI* ai)
    {
        return new ActionNode("berserking",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* blood_fury(PlayerbotAI* ai)
    {
        return new ActionNode("blood fury",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* cannibalize(PlayerbotAI* ai)
    {
        return new ActionNode("cannibalize",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* shadowmeld(PlayerbotAI* ai)
    {
        return new ActionNode("shadowmeld",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* escape_artist(PlayerbotAI* ai)
    {
        return new ActionNode("escape artist",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* stoneform(PlayerbotAI* ai)
    {
        return new ActionNode("stoneform",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* perception(PlayerbotAI* ai)
    {
        return new ActionNode("perception",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
#ifndef MANGOSBOT_ZERO
    static ActionNode* arcane_torrent(PlayerbotAI* ai)
    {
        return new ActionNode("arcane torrent",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* mana_tap(PlayerbotAI* ai)
    {
        return new ActionNode("mana tap",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* gift_of_the_naaru(PlayerbotAI* ai)
    {
        return new ActionNode("gift of the naaru",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
#endif
#ifdef MANGOSBOT_TWO
    static ActionNode* every_man_for_himself(PlayerbotAI* ai)
    {
        return new ActionNode("every man for himself",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
#endif
};

void RacialsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "stoneform", 
        NextAction::array(0, new NextAction("stoneform", ACTION_DISPEL), NULL)));

    triggers.push_back(new TriggerNode(
        "war stomp",
        NextAction::array(0, new NextAction("war stomp", ACTION_INTERRUPT + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "berserking",
        NextAction::array(0, new NextAction("berserking", 71.0f), NULL)));

        triggers.push_back(new TriggerNode(
        "blood fury",
        NextAction::array(0, new NextAction("blood fury", 71.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "arcane torrent", 
        NextAction::array(0, new NextAction("arcane torrent", ACTION_INTERRUPT + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "mana tap",
        NextAction::array(0, new NextAction("mana tap", ACTION_INTERRUPT + 6), NULL)));
     
    triggers.push_back(new TriggerNode(
        "cannibalize",
        NextAction::array(0, new NextAction("cannibalize", ACTION_MEDIUM_HEAL + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "WOtF",
        NextAction::array(0, new NextAction("WOtF", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "EMfH",
        NextAction::array(0, new NextAction("EMfH", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "escape artist",
        NextAction::array(0, new NextAction("escape artist", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "perception",
        NextAction::array(0, new NextAction("perception", ACTION_HIGH + 6), NULL)));
}

RacialsStrategy::RacialsStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new RacialsStrategyActionNodeFactory());
}
