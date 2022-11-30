#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotHelpMgr.h"


#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

#ifdef GenerateBotHelp
#include <iomanip>

#include "strategy/priest/PriestAiObjectContext.h"
#include "strategy/mage/MageAiObjectContext.h"
#include "strategy/warlock/WarlockAiObjectContext.h"
#include "strategy/warrior/WarriorAiObjectContext.h"
#include "strategy/shaman/ShamanAiObjectContext.h"
#include "strategy/paladin/PaladinAiObjectContext.h"
#include "strategy/druid/DruidAiObjectContext.h"
#include "strategy/hunter/HunterAiObjectContext.h"
#include "strategy/rogue/RogueAiObjectContext.h"
#include "strategy/deathknight/DKAiObjectContext.h"
#endif

PlayerbotHelpMgr::PlayerbotHelpMgr()
{
}

PlayerbotHelpMgr::~PlayerbotHelpMgr()
{
}



#ifdef GenerateBotHelp
string PlayerbotHelpMgr::formatFloat(float num)
{
    ostringstream out;
    out << std::fixed << std::setprecision(3);
    out << num;
    return out.str().c_str();
}


void PlayerbotHelpMgr::replace(string& text, const string what, const string with)
{
    size_t start_pos = 0;
    while ((start_pos = text.find(what, start_pos)) != std::string::npos) {
        text.replace(start_pos, what.size(), with);
    }
}

string PlayerbotHelpMgr::makeList(vector<string>const parts, string partFormat, uint32 maxLength)
{
    string retString = "";
    string currentLine = "";

    for (auto part : parts)
    {
        if (part.length() + currentLine.length() > maxLength)
        {
            currentLine.pop_back();
            retString += currentLine + "\n";
            currentLine.clear();
        }
        string subPart = partFormat;

        replace(partFormat, "<part>", part);

        currentLine += part + " ";
    }

    return retString + currentLine;
}

void PlayerbotHelpMgr::SaveTemplates()
{
    for (auto text : botHelpText)
    {
        if (!text.second.m_templateChanged)
            continue;

        if(text.second.m_new)
            PlayerbotDatabase.PExecute("INSERT INTO `ai_playerbot_help_texts` (`name`, `template_text`, `template_changed`, `text`) VALUES ('%s', '%s', 1, '%s')", text.first, text.second.m_templateText, text.second.m_templateText);
        else
            PlayerbotDatabase.PExecute("UPDATE `ai_playerbot_help_texts` set  `template_text` = '%s',  `template_changed` = 1 where `name` = '%s'", text.second.m_templateText, text.first);
    }
}

void PlayerbotHelpMgr::LoadStrategies(string className, AiObjectContext* context)
{
    vector<string> stratLinks;
    for (auto strategyName : context->GetSupportedStrategies())
    {
        if (strategyName == "custom")
            continue;

        Strategy* strategy = context->GetStrategy(strategyName);

        if (std::find_if(classMap["generic"].begin(), classMap["generic"].end(), [strategyName](const std::pair<Strategy *, triggerMap>& v) {return v.first->getName() == strategyName;}) != classMap["generic"].end())
            continue;

        std::list<TriggerNode*> triggers;
        strategy->InitTriggers(triggers);

        if (!triggers.empty())
        {
            for (auto& triggerNode : triggers)
            {
                Trigger* trigger = context->GetTrigger(triggerNode->getName());

                if (trigger)
                {
                    triggerNode->setTrigger(trigger);

                    NextAction** nextActions = triggerNode->getHandlers();

                    for (int32 i = 0; i < NextAction::size(nextActions); i++)
                    {
                        NextAction* nextAction = nextActions[i];

                        classMap[className][strategy][trigger][nextAction] = nextAction->getRelevance();
                    }
                }
            }
        }

        if (strategy->getDefaultActions())
        {
            for (int32 i = 0; i < NextAction::size(strategy->getDefaultActions()); i++)
            {
                NextAction* nextAction = strategy->getDefaultActions()[i];

                classMap[className][strategy][nullptr][nextAction] = nextAction->getRelevance();
            }
        }
    }
}

void PlayerbotHelpMgr::LoadAllStrategies()
{
    AiObjectContext* genericContext = new AiObjectContext(ai);
    LoadStrategies("generic", genericContext);

    classContext["warrior"] = new WarriorAiObjectContext(ai);
    classContext["paladin"] = new PaladinAiObjectContext(ai);
    classContext["hunter"] = new HunterAiObjectContext(ai);
    classContext["rogue"] = new RogueAiObjectContext(ai);
    classContext["priest"] = new PriestAiObjectContext(ai);
#ifdef MANGOSBOT_TWO
    classContext["deathknight"] = new DKAiObjectContext(ai);
#endif
    classContext["shaman"] = new ShamanAiObjectContext(ai);
    classContext["mage"] = new MageAiObjectContext(ai);
    classContext["warlock"] = new WarlockAiObjectContext(ai);
    classContext["druid"] = new DruidAiObjectContext(ai);

    classMap.clear();

    for (auto cc : classContext)
    {
        LoadStrategies(cc.first, cc.second);
    }

    classContext["generic"] = genericContext;
}

string PlayerbotHelpMgr::GetStrategyBehaviour(string className, Strategy* strategy)
{
    string behavior;

    AiObjectContext* context = classContext[className];

    if (!classMap[className][strategy].empty())
    {
        behavior += "\nBehavior:";

        for (auto trig : classMap[className][strategy])
        {
            string line;

            Trigger* trigger = trig.first;

            if (!trig.first)
                line = "\nDefault:";
            else
                line = "On: [h:trigger|" + trigger->getName() + "] do";

            for (auto act : trig.second)
            {

                line += "[h:action|" + act.first->getName() + "] (" + formatFloat(act.second) + ")";
            }

            behavior += "\n" + line;
        }
    }

    return behavior;
}

void PlayerbotHelpMgr::GenerateStrategyHelp()
{
    for (auto& strategyClass : classMap)
    {
        string className = strategyClass.first;

        vector<string> stratLinks;

        for (auto& strat : strategyClass.second)
        {
            Strategy* strategy = strat.first;

            string strategyName = strategy->getName();
            string linkName = strategyName;

            if (className != "generic")
                linkName = className + " " + strategyName;

            stratLinks.push_back("[h:strategy:" + linkName + "|" + strategyName + "]");

            string helpTemplate = botHelpText["template:strategy"].m_templateText;

            string description = strategy->GetHelpDescription();
            string related = makeList(strategy->GetRelatedStrategies(), "[h:strategy|<part>]");

            if (!related.empty())
                related = "\nRelated strategies:\n" + related;

            string behavior = GetStrategyBehaviour(className, strategy);

            replace(helpTemplate, "<name>", strategyName);
            replace(helpTemplate, "<description>", description);
            replace(helpTemplate, "<related>", related);
            replace(helpTemplate, "<behavior>", behavior);

            if (botHelpText["strategy:" + linkName].m_templateText != helpTemplate)
                botHelpText["strategy:" + linkName].m_templateChanged = true;
            botHelpText["strategy:" + linkName].m_templateText = helpTemplate;
        }

        std::sort(stratLinks.begin(), stratLinks.end());

        botHelpText["list:" + className + " strategy"].m_templateText = className + " strategies : \n" + makeList(stratLinks);
    }
}



void PlayerbotHelpMgr::GenerateHelp()
{
    ai = new PlayerbotAI();

    LoadAllStrategies();

    GenerateStrategyHelp();

    //SaveTemplates();
}
#endif

void PlayerbotHelpMgr::FormatHelpTopic(string& text)
{
    //[h:subject:topic|name]
    size_t start_pos = 0;
    while ((start_pos = text.find("[h:", start_pos)) != std::string::npos) {
        size_t end_pos = text.find("]", start_pos);

        if (end_pos == std::string::npos)
            break;

        string oldLink = text.substr(start_pos, end_pos - start_pos + 1);

        if (oldLink.find("|") != std::string::npos)
        {
            string topicCode = oldLink.substr(3, oldLink.find("|") - 3);
            std::string topicName = oldLink.substr(oldLink.find("|") + 1);
            topicName.pop_back();

            if (topicCode.find(":") == std::string::npos)
                topicCode += ":" + topicName;

            string newLink = ChatHelper::formatValue("help", topicCode, topicName);

            text.replace(start_pos, oldLink.length(), newLink);
            start_pos += newLink.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        else
            start_pos= end_pos;
    }

    //[c:command|name]
    start_pos = 0;
    while ((start_pos = text.find("[c:", start_pos)) != std::string::npos) {
        size_t end_pos = text.find("]", start_pos);

        if (end_pos == std::string::npos)
            break;

        string oldLink = text.substr(start_pos, end_pos - start_pos + 1);

        if (oldLink.find("|") != std::string::npos)
        {
            string topicCode = oldLink.substr(3, oldLink.find("|") - 3);
            std::string topicName = oldLink.substr(oldLink.find("|") + 1);
            topicName.pop_back();                                                                                      
            string newLink = ChatHelper::formatValue("command", topicCode, topicName, "3a34ebff");

            text.replace(start_pos, oldLink.length(), newLink);
            start_pos += newLink.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        else
            start_pos = end_pos;
    }

}

void PlayerbotHelpMgr::FormatHelpTopics()
{
    for (auto& helpText : botHelpText)
    {
        if (helpText.second.m_text.empty())
            helpText.second.m_text = helpText.second.m_templateText;

        FormatHelpTopic(helpText.second.m_text);

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            FormatHelpTopic(helpText.second.m_text_locales[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))]);
        }
    }
}

void PlayerbotHelpMgr::LoadBotHelpTexts()
{
    sLog.outBasic("Loading playerbot texts...");
    QueryResult* results = PlayerbotDatabase.PQuery("SELECT `name`, `template_text`, `text`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` FROM `ai_playerbot_help_texts`");
    int count = 0;
    if (results)
    {
        do
        {
            std::string text, templateText;
            std::map<int32, std::string> text_locale;
            Field* fields = results->Fetch();
            string name = fields[0].GetString();
            templateText = fields[1].GetString();
            text = fields[2].GetString();

            for (uint8 i = 1; i < MAX_LOCALE; ++i)
            {
                text_locale[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] = fields[i + 2].GetString();
            }

            botHelpText[name] = BotHelpEntry(templateText, text, text_locale);

            count++;
        } while (results->NextRow());

        delete results;
    }
    sLog.outBasic("%d playerbot helptexts loaded", count);

#ifdef GenerateBotHelp
    GenerateHelp();
#endif

    FormatHelpTopics();
}

// general texts

string PlayerbotHelpMgr::GetBotText(string name)
{
    if (botHelpText.empty())
    {
        sLog.outError("Can't get bot help text %s! No bots help texts loaded!", name);
        return "";
    }
    if (botHelpText.find(name) == botHelpText.end())
    {
        sLog.outDetail("Can't get bot help text %s! No bots help texts for this name!", name);
        return "";
    }

    BotHelpEntry textEntry = botHelpText[name];
    int32 localePrio = sPlayerbotTextMgr.GetLocalePriority();
    if (localePrio == -1)
        return textEntry.m_text;
    else
    {
        if (!textEntry.m_text_locales[localePrio].empty())
            return textEntry.m_text_locales[localePrio];
        else
            return textEntry.m_text;
    }
}

bool PlayerbotHelpMgr::GetBotText(string name, string &text)
{
    text = GetBotText(name);
    return !text.empty();
}