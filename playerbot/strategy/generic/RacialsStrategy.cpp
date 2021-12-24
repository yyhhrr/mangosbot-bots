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
        "low health", 
        NextAction::array(0, new NextAction("gift of the naaru", 71.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe",
        NextAction::array(0, new NextAction("war stomp", 71.0f), NULL)));

    /*triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("war stomp", 71.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana", 
        NextAction::array(0, new NextAction("arcane torrent", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "medium mana",
        NextAction::array(0, new NextAction("mana tap", ACTION_EMERGENCY + 6), NULL)));

        triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("stoneform", ACTION_EMERGENCY + 6),
            new NextAction("desperate prayer", ACTION_EMERGENCY + 6),
            new NextAction("elune's grace", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "loot available",
        NextAction::array(0, new NextAction("cannibalize", ACTION_HIGH + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("berserking", ACTION_HIGH + 6),
            new NextAction("blood fury", ACTION_HIGH + 6),
            new NextAction("starshards", ACTION_HIGH + 6),
            new NextAction("touch of weakness", ACTION_HIGH + 6),
            new NextAction("devouring plague", ACTION_HIGH + 6),
            new NextAction("hex of weakness", ACTION_HIGH + 6),
            new NextAction("war stomp", ACTION_HIGH + 6),
            NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("fear ward", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "fear ward",
        NextAction::array(0, new NextAction("fear ward", ACTION_NORMAL + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "fear ward on party",
        NextAction::array(0, new NextAction("fear ward on party", ACTION_NORMAL + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "panic",
        NextAction::array(0, new NextAction("shadowmeld", ACTION_EMERGENCY + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy player is attacking",
        NextAction::array(0, new NextAction("shadowguard", ACTION_EMERGENCY + 6), NULL)));*/
}

RacialsStrategy::RacialsStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new RacialsStrategyActionNodeFactory());
}
