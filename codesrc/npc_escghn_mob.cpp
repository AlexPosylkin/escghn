//===================NPC Gehenna mob (Blue Archive) by AlexPos=================//
//
// Purpose: The main fighting force of the Prefect's team. Weak but numerous.  //
//
//=============================================================================//


#include "cbase.h"
#include "ai_default.h"
#include "ai_task.h"
#include "ai_schedule.h"
#include "ai_node.h"
#include "ai_hull.h"
#include "ai_hint.h"
#include "ai_squad.h"
#include "ai_senses.h"
#include "ai_navigator.h"
#include "ai_motor.h"
#include "ai_behavior.h"
#include "ai_baseactor.h"
#include "ai_behavior_lead.h"
#include "ai_behavior_follow.h"
#include "ai_behavior_standoff.h"
#include "ai_behavior_assault.h"
#include "npc_playercompanion.h"
#include "soundent.h"
#include "game.h"
#include "npcevent.h"
#include "activitylist.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "sceneentity.h"
#include "ai_behavior_functank.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ESCGHNMOB_MODEL "models/myproject/escghn_mob_01.mdl"

ConVar	sk_escghn_mob_health("sk_escghn_mob_health", "0");

//=========================================================
// Mob activities
//=========================================================

class CNPC_Escghmob : public CNPC_PlayerCompanion
{
public:
	DECLARE_CLASS(CNPC_Escghmob, CNPC_PlayerCompanion);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Precache()
	{
		// Prevents a warning
		SelectModel();
		BaseClass::Precache();

		PrecacheScriptSound("NPC_Citizen.FootstepLeft");
		PrecacheScriptSound("NPC_Citizen.FootstepRight");
		PrecacheScriptSound("NPC_Alyx.Die");		
	}

	void	Spawn(void);
	void	SelectModel();
	Class_T Classify(void);
	void	Weapon_Equip(CBaseCombatWeapon* pWeapon);

	bool CreateBehaviors(void);

	void HandleAnimEvent(animevent_t* pEvent);

	bool ShouldLookForBetterWeapon() { return false; }	// TODO: line 147

	void OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior);

	void DeathSound(const CTakeDamageInfo& info);
	void GatherConditions();
	void UseFunc(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	CAI_FuncTankBehavior		m_FuncTankBehavior;
	COutputEvent				m_OnPlayerUse;

	DEFINE_CUSTOM_AI;
};


LINK_ENTITY_TO_CLASS(npc_escghn_mob, CNPC_Escghmob);

//---------------------------------------------------------
//---------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST(CNPC_Escghmob, DT_NPC_Escghmob)
END_SEND_TABLE()


//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CNPC_Escghmob)
//						m_FuncTankBehavior
DEFINE_OUTPUT(m_OnPlayerUse, "OnPlayerUse"),
DEFINE_USEFUNC(UseFunc),
END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Escghmob::SelectModel()
{
	SetModelName(AllocPooledString(ESCGHNMOB_MODEL));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Escghmob::Spawn(void)
{
	Precache();

	m_iHealth = 150; // BaseEntity var Health (optional, because this npc cannot be ded)

	BaseClass::Spawn();

	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL | EFL_NO_PHYSCANNON_INTERACTION);

	NPCInit();

	SetUse(&CNPC_Escghmob::UseFunc);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Class_T	CNPC_Escghmob::Classify(void)
{
	return	CLASS_PLAYER_ALLY_VITAL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Escghmob::Weapon_Equip(CBaseCombatWeapon* pWeapon)  // TODO:	DEL weapon equip for mobs maybe??????????? 
{
	BaseClass::Weapon_Equip(pWeapon);
}

//---------------------------------------------------------
//---------------------------------------------------------
void CNPC_Escghmob::HandleAnimEvent(animevent_t* pEvent)
{
	switch (pEvent->event)
	{
	case NPC_EVENT_LEFTFOOT:
	{
		EmitSound("NPC_Citizen.FootstepLeft", pEvent->eventtime);
	}
	break;
	case NPC_EVENT_RIGHTFOOT:
	{
		EmitSound("NPC_Citizen.FootstepRight", pEvent->eventtime);
	}
	break;

	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CNPC_Escghmob::DeathSound(const CTakeDamageInfo& info)
{
	// Sentences don't play on dead NPCs
	SentenceStop();

	EmitSound("npc_alyx.die");

}

bool CNPC_Escghmob::CreateBehaviors(void)
{
	BaseClass::CreateBehaviors();
	AddBehavior(&m_FuncTankBehavior);

	return true;
}

void CNPC_Escghmob::OnChangeRunningBehavior(CAI_BehaviorBase* pOldBehavior, CAI_BehaviorBase* pNewBehavior)
{
	if (pNewBehavior == &m_FuncTankBehavior)
	{
		m_bReadinessCapable = false;
	}
	else if (pOldBehavior == &m_FuncTankBehavior)
	{
		m_bReadinessCapable = IsReadinessCapable();
	}

	BaseClass::OnChangeRunningBehavior(pOldBehavior, pNewBehavior);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Escghmob::GatherConditions()
{
	BaseClass::GatherConditions();

	// AI speech processing is not used (no VCDs)
	if (m_NPCState == NPC_STATE_IDLE || m_NPCState == NPC_STATE_ALERT || m_NPCState == NPC_STATE_COMBAT ||
		((m_NPCState == NPC_STATE_SCRIPT) && CanSpeakWhileScripting()))
	{
		DoCustomSpeechAI();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Escghmob::UseFunc(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	m_bDontUseSemaphore = true;
	SpeakIfAllowed(TLK_USE);
	m_bDontUseSemaphore = false;

	m_OnPlayerUse.FireOutput(pActivator, pCaller);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC(npc_escghn_mob, CNPC_Escghmob)

AI_END_CUSTOM_NPC()
