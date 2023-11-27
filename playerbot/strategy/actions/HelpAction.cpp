#include "botpch.h"
#include "../../playerbot.h"
#include "HelpAction.h"
#include "ChatActionContext.h"
#include "PlayerbotHelpMgr.h"

using namespace ai;

HelpAction::HelpAction(PlayerbotAI* ai) : ChatCommandAction(ai, "help")
{
    chatContext = new ChatActionContext();
}

HelpAction::~HelpAction()
{
    delete chatContext;
}

bool HelpAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string param = event.getParam();
    string helpTopic;

    if(param.find("Hvalue:help") != string::npos)
    {
        helpTopic = ChatHelper::parseValue("help",param);
    }
    else if (param.find("[h:") != string::npos)
    {
        helpTopic = param.substr(3,param.size()-4);
    }

    if (param.empty())
    {
        helpTopic = "help:main";
    }

    ostringstream out;
    string helpTekst = sPlayerbotHelpMgr.GetBotText(helpTopic);

    if (helpTopic.empty() && !param.empty())
    {
        vector<string> list = sPlayerbotHelpMgr.FindBotText(param);

        if(list.size() == 1)
            helpTekst = sPlayerbotHelpMgr.GetBotText(list.front());
        else if (!list.empty())
        {
            std::sort(list.begin(), list.end());
            helpTekst = sPlayerbotHelpMgr.GetBotText("help:search");

            string topics = sPlayerbotHelpMgr.makeList(list, "[h:<part>|<part>]");

            if (!helpTekst.empty())
                sPlayerbotHelpMgr.replace(helpTekst, "<found>", topics);
            else
                helpTekst = topics; //Fallback for old help table.

            sPlayerbotHelpMgr.FormatHelpTopic(helpTekst);
        }
    }

    if (!helpTekst.empty())
    {
        vector<string> lines = Qualified::getMultiQualifiers(helpTekst, "\n");

        for (auto& line : lines)
        {
            ai->TellPlayerNoFacing(requester, line, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, true, false);
        }

        return true;
    }
    
    TellChatCommands(requester);
    TellStrategies(requester);

    return true;
}

void HelpAction::TellChatCommands(Player* requester)
{
    ostringstream out;
    out << "Whisper any of: ";
    out << CombineSupported(chatContext->supports());
    out << ", [item], [quest] or [object] link";
    ai->TellPlayer(requester, out.str());
}

void HelpAction::TellStrategies(Player* requester)
{
    ostringstream out;
    out << "Possible strategies (co/nc/dead commands): ";
    out << CombineSupported(ai->GetAiObjectContext()->GetSupportedStrategies());
    ai->TellPlayer(requester, out.str());
}

string HelpAction::CombineSupported(set<string> commands)
{
    ostringstream out;

    for (set<string>::iterator i = commands.begin(); i != commands.end(); )
	{
        out << *i;
		if (++i != commands.end())
			out << ", ";
	}

    return out.str();
}
