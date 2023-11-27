#include "botpch.h"
#include "../../playerbot.h"
#include "GuildManagementActions.h"
#include "../../ServerFacade.h"

using namespace std;
using namespace ai;

Player* GuidManageAction::GetPlayer(Event event)
{
    Player* player = nullptr;
    ObjectGuid guid = event.getObject();

    if (guid)
    {
        player = sObjectMgr.GetPlayer(guid);

        if (player)
            return player;
    }

    string text = event.getParam();

    if (!text.empty())
    {
        if (normalizePlayerName(text))
        {
            player = ObjectAccessor::FindPlayerByName(text.c_str());

            if (player)
                return player;
        }

        return nullptr;
    }
        
    Player* master = GetMaster();
    if (master && master == event.getOwner())
        guid = bot->GetSelectionGuid();
    
    player = sObjectMgr.GetPlayer(guid);

    if (player)
        return player;

    player = event.getOwner();

    if (player)
       return player;
    
    return nullptr;
}

bool GuidManageAction::Execute(Event& event)
{
    Player* player = GetPlayer(event);

    if (!player || !PlayerIsValid(player) || player == bot)
        return false;

    WorldPacket data = GetPacket(player);

    SendPacket(data, event);

    return true;
}

bool GuildManageNearbyAction::Execute(Event& event)
{
    uint32 found = 0;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    MemberSlot* botMember = guild->GetMemberSlot(bot->GetObjectGuid());

    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
    for (auto& guid : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(guid);

        if (!player || bot == player)
            continue;

        if (player->isDND())
            continue;


        if(player->GetGuildId()) //Promote or demote nearby members based on chance.
        {          
            MemberSlot* member = guild->GetMemberSlot(player->GetObjectGuid());
            uint32 dCount = AI_VALUE(uint32, "death count");

            if (!urand(0, 30) && dCount < 2 && guild->HasRankRight(botMember->RankId, GR_RIGHT_PROMOTE))
            {
                if (sPlayerbotAIConfig.guildFeedbackRate && frand(0, 100) <= sPlayerbotAIConfig.guildFeedbackRate && bot->GetGuildId() && !urand(0, 10) && sRandomPlayerbotMgr.IsFreeBot(bot))
                {
                    map<string, string> placeholders;
                    placeholders["%name"] = player->GetName();

                    guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("Good job %name. You deserver this.", placeholders), LANG_UNIVERSAL);
                }

                ai->DoSpecificAction("guild promote", Event("guild management", guid), true);
                continue;
            }

            if (!urand(0, 30) && dCount > 2 && guild->HasRankRight(botMember->RankId, GR_RIGHT_DEMOTE))
            {
                if (sPlayerbotAIConfig.guildFeedbackRate && frand(0, 100) <= sPlayerbotAIConfig.guildFeedbackRate && bot->GetGuildId() && !urand(0, 10) && sRandomPlayerbotMgr.IsFreeBot(bot))
                {
                    map<string, string> placeholders;
                    placeholders["%name"] = player->GetName();

                    guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("That was awefull %name. I hate to do this but...", placeholders), LANG_UNIVERSAL);
                }

                ai->DoSpecificAction("guild demote", Event("guild management", guid), true);
                continue;
            }

            continue;
        }

        if (!sPlayerbotAIConfig.randomBotGuildNearby)
            return false;

        if (guild->GetMemberSize() > 1000)
            return false;

        if (guild->HasRankRight(botMember->RankId, GR_RIGHT_INVITE))
            continue;

        if (player->GetGuildIdInvited())
            continue;

        if (!sPlayerbotAIConfig.randomBotInvitePlayer && player->isRealPlayer())
            continue;

        PlayerbotAI* botAi = player->GetPlayerbotAI();

        if (botAi)
        {            
            if (botAi->GetGuilderType() == GuilderType::SOLO && !botAi->HasRealPlayerMaster()) //Do not invite solo players.
                continue;
            
            if (botAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                continue;
        }

        bool sameGroup = bot->GetGroup() && bot->GetGroup()->IsMember(player->GetObjectGuid());

        if (!sameGroup && sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.spellDistance)
            continue;

        if (sPlayerbotAIConfig.inviteChat && sRandomPlayerbotMgr.IsFreeBot(bot))
        {
            map<string, string> placeholders;
            placeholders["%name"] = player->GetName();
            placeholders["%members"] = guild->GetMemberSize();
            placeholders["%guildname"] = guild->GetName();
            placeholders["%place"] = WorldPosition(player).getAreaName(false, false);

            vector<string> lines;

            switch ((urand(0, 10)* urand(0, 10))/10)
            {
            case 0:
                lines.push_back(BOT_TEXT2("Hey %name do you want to join my guild?", placeholders));
                break;
            case 1:
                lines.push_back(BOT_TEXT2("Hey man you wanna join my guild %name?", placeholders));
                break;
            case 2:
                lines.push_back(BOT_TEXT2("I think you would be a good contribution to %guildname. Would you like to join %name?", placeholders));
                break;
            case 3:
                lines.push_back(BOT_TEXT2("My guild %guildname has %members quality members. Would you like to make it 1 more %name?", placeholders));
                break;
            case 4:
                lines.push_back(BOT_TEXT2("Hey %name do you want to join %guildname? We have %members members and looking to become number 1 of the server.", placeholders));
                break;
            case 5:
                lines.push_back(BOT_TEXT2("I'm not really good at smalltalk. Do you wanna join my guild %name/r?", placeholders));
                break;
            case 6:
                lines.push_back(BOT_TEXT2("Welcome to %place.... do you want to join my guild %name?", placeholders));
                break;
            case 7:
                lines.push_back(BOT_TEXT2("%name, you should join my guild!", placeholders));
                break;
            case 8:
                lines.push_back(BOT_TEXT2("%name, I got this guild....", placeholders));
                break;
            case 9:
                lines.push_back(BOT_TEXT2("You are actually going to join my guild %name?", placeholders));
                lines.push_back(BOT_TEXT2("Haha.. you are the man! We are going to raid Molten...", placeholders));
                break;
            case 10:
                lines.push_back(BOT_TEXT2("Hey Hey! do you guys wanna join my gild????", placeholders));
                lines.push_back(BOT_TEXT2("We've got a bunch of high levels and we are really super friendly..", placeholders));
                lines.push_back(BOT_TEXT2("..and watch your dog and do your homework...", placeholders));
                lines.push_back(BOT_TEXT2("..and we raid once a week and are working on MC raids...", placeholders));
                lines.push_back(BOT_TEXT2("..and we have more members than just me...", placeholders));
                lines.push_back(BOT_TEXT2("..and please stop I'm lonenly and we can get a ride the whole time...", placeholders));
                lines.push_back(BOT_TEXT2("..and it's really beautifull and I feel like crying...", placeholders));
                lines.push_back(BOT_TEXT2("So what do you guys say are you going to join are you going to join?", placeholders));
                break;
            }

            for (auto line : lines)
                if (sameGroup)
                {
                    WorldPacket data;
                    ChatHandler::BuildChatPacket(data, bot->GetGroup()->IsRaidGroup() ? CHAT_MSG_RAID : CHAT_MSG_PARTY, line.c_str(), LANG_UNIVERSAL, CHAT_TAG_NONE, bot->GetObjectGuid(), bot->GetName());
                    bot->GetGroup()->BroadcastPacket(data,true);
                }
                else
                    bot->Say(line, (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
        }
        
        if (ai->DoSpecificAction("guild invite", Event("guild management", guid), true))
        {
            if (sPlayerbotAIConfig.inviteChat)
                return true;
            found++;
        }
    }

    return found > 0;
}

bool GuildManageNearbyAction::isUseful()
{
    if (!bot->GetGuildId())
        return false;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    MemberSlot* botMember = guild->GetMemberSlot(bot->GetObjectGuid());

    return  guild->HasRankRight(botMember->RankId, GR_RIGHT_DEMOTE) || guild->HasRankRight(botMember->RankId, GR_RIGHT_PROMOTE) || guild->HasRankRight(botMember->RankId, GR_RIGHT_INVITE);
}

bool GuildLeaveAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    Player* owner = event.getOwner();
    if (owner && !ai->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE, false, owner, true))
    {
        ai->TellError(requester, "Sorry, I am happy in my guild :)");
        return false;
    }

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId()); 
    
    if (guild->GetMemberSize() >= 1000)
    {
        guild->BroadcastToGuild(bot->GetSession(), "I am leaving this guild to prevent it from reaching the 1064 member limit.", LANG_UNIVERSAL);
    }

    sPlayerbotAIConfig.logEvent(ai, "GuildLeaveAction", guild->GetName(), to_string(guild->GetMemberSize()));

    WorldPacket packet;
    bot->GetSession()->HandleGuildLeaveOpcode(packet);
    return true;
}