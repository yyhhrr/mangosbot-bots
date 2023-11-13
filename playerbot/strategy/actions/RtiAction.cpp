#include "botpch.h"
#include "../../playerbot.h"
#include "RtiAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/RtiTargetValue.h"

using namespace ai;

bool RtiAction::Execute(Event& event)
{
    string text = event.getParam();
    string type = "rti";
    if (text.find("cc ") == 0)
    {
        type = "rti cc";
        text = text.substr(3);
    }
    else if (text.empty() || text == "?")
    {
        ostringstream outRti; outRti << "rti" << ": ";
        AppendRti(outRti, "rti");
        ai->TellPlayer(GetMaster(), outRti);

        ostringstream outRtiCc; outRtiCc << "rti cc" << ": ";
        AppendRti(outRtiCc, "rti cc");
        ai->TellPlayer(GetMaster(), outRtiCc);
        return true;
    }

    context->GetValue<string>(type)->Set(text);
    ostringstream out; out << type << " set to: ";
    AppendRti(out, type);
    ai->TellPlayer(GetMaster(), out);
    return true;
}

void RtiAction::AppendRti(ostringstream & out, string type)
{
    out << AI_VALUE(string, type);

    ostringstream n; n << type << " target";
    Unit* target = AI_VALUE(Unit*, n.str());
    if (target)
        out << " (" << target->GetName() << ")";

}

bool MarkRtiAction::Execute(Event& event)
{
    Group *group = bot->GetGroup();
    if (!group) return false;

    if (bot->InBattleGround())
        return false;

    Unit* target = NULL;
    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible attack targets")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit)
            continue;

        // do not mark players
        if (unit->IsPlayer())
            continue;

        bool marked = false;
        for (int i = 0; i < 8; i++)
        {
            ObjectGuid guid = group->GetTargetIcon(i);
            if (guid == unit->GetObjectGuid())
            {
                marked = true;
                break;
            }
        }

        if (marked) continue;

        if (!target || (int)target->GetHealth() > (int)unit->GetHealth()) target = unit;
    }

    if (!target) return false;

    string rti = AI_VALUE(string, "rti");

    // Add the default rti if the bot is setup to ignore rti targets
    if (rti == "none")
    {
        rti = "skull";
    }

    int index = RtiTargetValue::GetRtiIndex(rti);
#ifndef MANGOSBOT_TWO
    group->SetTargetIcon(index, target->GetObjectGuid());
#else
    group->SetTargetIcon(index, bot->GetObjectGuid(), target->GetObjectGuid());
#endif
    return true;
}

