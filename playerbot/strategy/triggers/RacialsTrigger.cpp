#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "RpgTriggers.h"
#include "../../PlayerbotAIConfig.h"
#include "../actions/GuildCreateActions.h"
#include "SocialMgr.h"
#include "../../ServerFacade.h"


bool ManaTapTrigger::IsActive()
{
	return AI_VALUE2(bool, "has mana", "current target");
}