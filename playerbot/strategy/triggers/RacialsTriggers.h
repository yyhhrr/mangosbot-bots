#include "botpch.h"
#include "../../playerbot.h"
#include "GenericTriggers.h"
#include "HealthTriggers.h"
#include "../../PlayerbotAIConfig.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "Player.h"

using namespace ai;


class BerserkingTrigger : public BoostTrigger
{
public:
    BerserkingTrigger(PlayerbotAI* ai) : BoostTrigger(ai, "berserking") {}
};

class BloodFuryTrigger : public BoostTrigger
{
public:
    BloodFuryTrigger(PlayerbotAI* ai) : BoostTrigger(ai, "blood fury") {}
};

class CannibalizeTrigger : public Trigger 
{
public:
    CannibalizeTrigger(PlayerbotAI* ai) : Trigger(ai, "cannibalize") {}
    virtual bool IsActive()
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        return bot->getRace() == RACE_UNDEAD && 
            bot->GetHealthPercent() < 50.0f &&
        (target->GetCreatureType()) == CREATURE_TYPE_HUMANOID || CREATURE_TYPE_UNDEAD &&
        AI_VALUE2(bool, "dead", GetTargetName()) &&
        target && AI_VALUE2(float, "distance", "current target") <= sPlayerbotAIConfig.meleeDistance;
    }
};

class WoTFTrigger : public Trigger // Sheeped, feared, stunned....(Use Will of the Forsaken)  "WOtF"
{
public:
    WoTFTrigger(PlayerbotAI* ai) : Trigger(ai, "WoTF") {}
    virtual bool IsActive()
    {
        return bot->getRace() == RACE_UNDEAD && 
            bot->HasAuraType(SPELL_AURA_MOD_FEAR) ||
            bot->HasAuraType(SPELL_AURA_MOD_STUN) || 
            bot->HasAuraType(SPELL_AURA_MOD_CHARM) || 
            bot->HasAuraType(SPELL_AURA_MOD_CONFUSE);
    }
};

class EMfHTrigger : public Trigger // Sheeped, feared, stunned....(Use Every Man for Himself)  "EMfH"
{
public:
    EMfHTrigger(PlayerbotAI* ai) : Trigger(ai, "EMfH") {}
    virtual bool IsActive()
    {
        return bot->getRace() == RACE_HUMAN &&
            bot->HasAuraType(SPELL_AURA_MOD_FEAR) ||
            bot->HasAuraType(SPELL_AURA_MOD_STUN) ||
            bot->HasAuraType(SPELL_AURA_MOD_CHARM) ||
            bot->HasAuraType(SPELL_AURA_MOD_CONFUSE);
    }
};

class EscapeArtistTrigger : public Trigger //escape artist
{  
public:
    EscapeArtistTrigger(PlayerbotAI* ai) : Trigger(ai, "escape artist") {}
    virtual bool IsActive()
    {	
        return (bot->getRace() == RACE_GNOME) && 
            bot->HasAuraType(SPELL_AURA_MOD_ROOT) || 
            bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED);		
    }
};

class ManaTapTrigger : public InterruptSpellTrigger 
{
public:
    ManaTapTrigger(PlayerbotAI* ai) : InterruptSpellTrigger(ai, "mana tap") {}
    virtual bool IsActive()
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        return bot->getRace() == RACE_BLOODELF && target && AI_VALUE2(bool, "has mana", "current target");
    }
};

///another trigger for stoneform to remove bleeding, poisons, curses but im not sure about this and need to add function "HasAuraWithDispelType"
/*
class StoneformTrigger : public Trigger
{
public:
    StoneformTrigger(PlayerbotAI* ai) : Trigger(ai, "stoneform") {}
    virtual bool IsActive()
    {	
        return (bot->getRace() == RACE_DWARF) && 
            //bot->HasAuraWithDispelType(DISPEL_POISON) ||
            //bot->HasAuraWithDispelType(DISPEL_DISEASE) ||
            bot->HasAuraState(AURA_STATE_BLEEDING);
    }
};*/

class StoneformPoisonTrigger : public NeedCureTrigger
{
public:
    StoneformPoisonTrigger(PlayerbotAI* ai) : NeedCureTrigger(ai, "stoneform", DISPEL_POISON) {}
    virtual bool IsActive() 
    {
        return (bot->getRace() == RACE_DWARF);
    }
};

class StoneformDiseaseTrigger : public NeedCureTrigger
{
public:
    StoneformDiseaseTrigger(PlayerbotAI* ai) : NeedCureTrigger(ai, "stoneform", DISPEL_DISEASE) {}
    virtual bool IsActive() 
    {
        return (bot->getRace() == RACE_DWARF);
    }
};

class ArcanetorrentTrigger : public InterruptSpellTrigger
{
public:
    ArcanetorrentTrigger(PlayerbotAI* ai) : InterruptSpellTrigger(ai, "arcane torrent") {}
    virtual bool IsActive()
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        return bot->getRace() == RACE_BLOODELF && target && AI_VALUE2(float, "distance", "current target") <= sPlayerbotAIConfig.aoeRadius && ai->IsInterruptableSpellCasting(GetTarget(), getName(), true);;
    }
};

class WarStompTrigger : public SpellTrigger
{
public:
    WarStompTrigger(PlayerbotAI* ai) : SpellTrigger(ai, "war stomp") {}
    virtual bool IsActive()
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        return bot->getRace() == RACE_TAUREN && bot->GetHealthPercent() < 50.0f &&
        AI_VALUE2(bool, "combat", "self target") && AI_VALUE(uint8, "my attacker count") >= 3 &&
        target && AI_VALUE2(float, "distance", "current target") <= sPlayerbotAIConfig.tooCloseDistance;
    }
};

class ShadowmeldTrigger : public Trigger
{
public:
    ShadowmeldTrigger(PlayerbotAI* ai) : Trigger(ai, "shadowmeld") {}
    virtual bool IsActive()
    {
        Unit* master = ai->GetMaster();
        if (ai->HasAura("shadowmeld", bot))
            return false;
        return ((bot->getRace() == RACE_NIGHTELF) && master && master->HasStealthAura() && !master->IsMoving() && !bot->IsMoving());
    }
};

class PerceptionTrigger : public BuffTrigger
{
public:
    PerceptionTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "perception") {}
    virtual bool IsActive()
    {
        Unit::AttackerSet attackers(bot->getAttackers());
        for (set<Unit*>::const_iterator i = attackers.begin(); i != attackers.end(); i++)
        {
            Unit* attacker = *i;
        return (bot->getRace() == RACE_HUMAN) && (attacker->getClass() == CLASS_ROGUE && attacker->IsPlayer()) &&         
        sServerFacade.GetDistance2d(bot, attacker) <= sPlayerbotAIConfig.spellDistance;
        }
    }
};

class GiftOfTheNaaruTrigger : public Trigger
{
public:
    GiftOfTheNaaruTrigger(PlayerbotAI* ai) : Trigger(ai, "gift of the naaru") {}
    virtual bool IsActive()
    {      
        return (bot->getRace() == RACE_DRAENEI) &&  AI_VALUE2(bool, "low health", "self target");
    }
       
};
