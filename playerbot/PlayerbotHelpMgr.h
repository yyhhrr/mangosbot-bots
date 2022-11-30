#ifndef _PlayerbotHelpMgr_H
#define _PlayerbotHelpMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"

#define BOT_HELP(name) sPlayerbotHelpMgr.GetBotText(name)

#ifndef GenerateBotHelp
#define GenerateBotHelp
#endif

using namespace std;

struct BotHelpEntry
{
    BotHelpEntry() { m_new = true; }
    BotHelpEntry(std::string templateText, std::string text, std::map<int32, std::string> text_locales) : m_templateText(templateText), m_text(text), m_text_locales(text_locales) {}
    std::string m_templateText;
    bool m_new = false;
    bool m_templateChanged = false;
    std::string m_text;
    std::map<int32, std::string> m_text_locales;
};

class PlayerbotHelpMgr
{
    public:
        PlayerbotHelpMgr();
        virtual ~PlayerbotHelpMgr();
        static PlayerbotHelpMgr& instance()
        {
            static PlayerbotHelpMgr instance;
            return instance;
        }

	public:
#ifdef GenerateBotHelp
        PlayerbotAI* ai;
        typedef unordered_map<NextAction*, float> actionMap;
        typedef unordered_map<Trigger*, actionMap> triggerMap;
        typedef unordered_map<Strategy*, triggerMap> strategyMap;
        unordered_map<string, strategyMap> classMap;
        unordered_map<string, AiObjectContext*> classContext;

        static string formatFloat(float num);
        static void replace(string& text, const string what, const string with);
        static string makeList(vector<string>const parts, string partFormat = "<part> ", uint32 maxLength = 2048);

        void LoadStrategies(string className, AiObjectContext* context);
        void LoadAllStrategies();

        string GetStrategyBehaviour(string className, Strategy* strategy);
        void GenerateStrategyHelp();

        void SaveTemplates();
        
        void GenerateHelp();
#endif
        static void FormatHelpTopic(string& text);
        void FormatHelpTopics();
        void LoadBotHelpTexts();

        string GetBotText(string name);
        bool GetBotText(string name, string& text);

    private:
        unordered_map<string, BotHelpEntry> botHelpText;
};

#define sPlayerbotHelpMgr PlayerbotHelpMgr::instance()

#endif
