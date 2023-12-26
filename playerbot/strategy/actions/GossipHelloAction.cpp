#include "botpch.h"
#include "../../playerbot.h"
#include "GossipHelloAction.h"

#include "../../ServerFacade.h"

using namespace ai;

bool GossipHelloAction::Execute(Event& event)
{
	ObjectGuid guid;

    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
	WorldPacket &p = event.getPacket();
	if (p.empty())
	{
		if (requester)
			guid = requester->GetSelectionGuid();
	}
	else
	{
		p.rpos(0);
		p >> guid;
	}

	if (!guid)
		return false;

	Creature *pCreature = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
	if (!pCreature)
	{
		DEBUG_LOG("[PlayerbotMgr]: HandleMasterIncomingPacket - Received  CMSG_GOSSIP_HELLO %s not found or you can't interact with him.", guid.GetString().c_str());
		return false;
	}

	GossipMenuItemsMapBounds pMenuItemBounds = sObjectMgr.GetGossipMenuItemsMapBounds(pCreature->GetCreatureInfo()->GossipMenuId);
	if (pMenuItemBounds.first == pMenuItemBounds.second)
		return false;

	string text = event.getParam();
	int menuToSelect = -1;
	if (text.empty())
	{
        WorldPacket p1;
        p1 << guid;
        bot->GetSession()->HandleGossipHelloOpcode(p1);
        sServerFacade.SetFacingTo(bot, pCreature);

        ostringstream out; out << "--- " << pCreature->GetName() << " ---";
        ai->TellPlayerNoFacing(requester, out.str());

        TellGossipMenus(requester);
	}
	else if (!bot->GetPlayerMenu())
	{
	    ai->TellPlayerNoFacing(requester, "I need to talk first");
	    return false;
	}
	else
	{
	    menuToSelect = atoi(text.c_str());
	    if (menuToSelect > 0) menuToSelect--;
        ProcessGossip(requester, menuToSelect);
	}

	bot->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
	return true;
}

void GossipHelloAction::TellGossipText(Player* requester, uint32 textId)
{
    if (!textId)
        return;

    GossipText const* text = sObjectMgr.GetGossipText(textId);
    if (text)
    {
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; i++)
        {
            string text0 = text->Options[i].Text_0;
            if (!text0.empty()) ai->TellPlayerNoFacing(requester, text0);
            string text1 = text->Options[i].Text_1;
            if (!text1.empty()) ai->TellPlayerNoFacing(requester, text1);
        }
    }
}

void GossipHelloAction::TellGossipMenus(Player* requester)
{
    if (!bot->GetPlayerMenu())
        return;
 
     GossipMenu& menu = bot->GetPlayerMenu()->GetGossipMenu();

     if (requester)
     {
         Creature* pCreature = bot->GetNPCIfCanInteractWith(requester->GetSelectionGuid(), UNIT_NPC_FLAG_NONE);

         if (pCreature)
         {
             uint32 textId = bot->GetGossipTextId(menu.GetMenuId(), pCreature);
             TellGossipText(requester, textId);
         }
     }

    for (unsigned int i = 0; i < menu.MenuItemCount(); i++)
    {
        GossipMenuItem const& item = menu.GetItem(i);
        ostringstream out; out << "[" << (i+1) << "] " << item.m_gMessage;
        ai->TellPlayerNoFacing(requester, out.str());
    }
}

bool GossipHelloAction::ProcessGossip(Player* requester, int menuToSelect)
{
    GossipMenu& menu = bot->GetPlayerMenu()->GetGossipMenu();
    if (menuToSelect >= 0 && (unsigned int)menuToSelect >= menu.MenuItemCount())
    {
        ai->TellError(requester, "Unknown gossip option");
        return false;
    }
    GossipMenuItem const& item = menu.GetItem(menuToSelect);
    WorldPacket p;
    std::string code;
    p << GetMaster()->GetSelectionGuid();
#ifdef MANGOSBOT_ZERO
    p << menuToSelect;
#else
    p << menu.GetMenuId() << menuToSelect;
#endif
    p << code;
    bot->GetSession()->HandleGossipSelectOptionOpcode(p);

    TellGossipMenus(requester);
    return true;
}
