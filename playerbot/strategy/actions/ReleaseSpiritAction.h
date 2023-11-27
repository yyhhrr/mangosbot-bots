#pragma once

#include "../../ServerFacade.h"
#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "ReviveFromCorpseAction.h"

namespace ai
{
    class ReleaseSpiritAction : public ChatCommandAction
    {
    public:
        ReleaseSpiritAction(PlayerbotAI* ai, string name = "release") : ChatCommandAction(ai, name) {}

    public:
        virtual bool Execute(Event& event) override
        {
            Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
            if (sServerFacade.IsAlive(bot))
                return false;

            if (bot->GetCorpse() && bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
            {
                ai->TellPlayerNoFacing(requester, "I am already a spirit", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
                return false;
            }

            WorldPacket& p = event.getPacket();
            if (!p.empty() && p.GetOpcode() == CMSG_REPOP_REQUEST)
                ai->TellPlayerNoFacing(requester, "Releasing...", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            else
                ai->TellPlayerNoFacing(requester, "Meet me at the graveyard", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

            sLog.outDetail("Bot #%d %s:%d <%s> released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);

            // add waiting for ress aura
            if (bot->InBattleGround() && !ai->HasAura(2584, bot))
            {
                // cast Waiting for Resurrect
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);
            }

            return true;
        }
    };

    class AutoReleaseSpiritAction : public ReleaseSpiritAction 
    {
    public:
        AutoReleaseSpiritAction(PlayerbotAI* ai, string name = "auto release") : ReleaseSpiritAction(ai, name) {}

        virtual bool Execute(Event& event) override
        {
            sLog.outDetail("Bot #%d %s:%d <%s> auto released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);

            // add waiting for ress aura
            if (bot->InBattleGround() && !ai->HasAura(2584, bot))
            {
                // cast Waiting for Resurrect
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);
            }

            sPlayerbotAIConfig.logEvent(ai, "AutoReleaseSpiritAction");

            return true;
        }

        virtual bool isUseful()
        {
            if (!sServerFacade.UnitIsDead(bot))
                return false;

            if (bot->GetCorpse())
                return false;

#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif

            if (bot->InBattleGround())
                return !bot->GetCorpse() || !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);

            if (bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                return false;

            if (!bot->GetGroup())
                return true;

            if (!ai->GetGroupMaster())
                return true;

            if (ai->GetGroupMaster() == bot)
                return true;

            if (!ai->HasActivePlayerMaster())
                return true;

            if (ai->HasActivePlayerMaster() && ai->GetGroupMaster()->GetMapId() == bot->GetMapId() && (bot->GetMap()->IsRaid() || bot->GetMap()->IsDungeon()))
                return false;

            if(sServerFacade.UnitIsDead(ai->GetGroupMaster()))
                return true;

            if (sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.sightDistance))
                return true;

            return false;
        }
    };

    class RepopAction : public SpiritHealerAction 
    {
    public:
        RepopAction(PlayerbotAI* ai, string name = "repop") : SpiritHealerAction(ai, name) {}

    public:
        virtual bool Execute(Event& event)
        {
            GuidPosition grave = AI_VALUE(GuidPosition, "best graveyard");
            Player* requester = event.getOwner() ? event.getOwner() : GetMaster();

            if (!grave)
                return false;

            if (!bot->GetCorpse() || AI_VALUE(bool, "should spirit healer"))
            {
                sLog.outBasic("Bot #%d %s:%d <%s> repops at graveyard [%s]", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName(), event.getSource().c_str());
            }
            else
            {
                sLog.outBasic("Bot #%d %s:%d <%s> repops at spirit healer", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());
                PlayerbotChatHandler ch(bot);
                bot->ResurrectPlayer(0.5f, !ai->HasCheat(BotCheatMask::repair));
                if (!ai->HasCheat(BotCheatMask::repair))
                {
                    bot->DurabilityLossAll(0.25f, true);
                }

                bot->SpawnCorpseBones();
                bot->SaveToDB();
                context->GetValue<Unit*>("current target")->Set(nullptr);
                bot->SetSelectionGuid(ObjectGuid());
                ai->TellPlayer(requester, BOT_TEXT("hello"), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            }

            bot->TeleportTo(grave.getMapId(), grave.getX(), grave.getY(), grave.getZ(), grave.getO());

            sPlayerbotAIConfig.logEvent(ai, "RepopAction");

            RESET_AI_VALUE(bool,"combat::self target");
            RESET_AI_VALUE(WorldPosition, "current position");

            return true;
        }

        virtual bool isUseful()
        {
            if (bot->InBattleGround())
                return false;

            return true;
        }
    };

    class SelfResurrectAction : public ChatCommandAction
    {
    public:
        SelfResurrectAction(PlayerbotAI* ai, string name = "self resurrect") : ChatCommandAction(ai, name) {}

    public:
        bool Execute(Event& event) override
        {
            WorldPacket packet(CMSG_SELF_RES);
            bot->GetSession()->HandleSelfResOpcode(packet);
            return true;
        }

        bool isPossible() override
        {
            return ai->IsStateActive(BotState::BOT_STATE_DEAD) && bot->GetUInt32Value(PLAYER_SELF_RES_SPELL);
        }
    };
}
