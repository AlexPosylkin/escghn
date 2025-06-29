//=============================================================================//
//
// Purpose:  Client DATADESC for npc_escghn_mob
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_Escgn_Mob : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_Escgn_Mob, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_Escgn_Mob();
	virtual			~C_Escgn_Mob();

private:
	C_Escgn_Mob(const C_Escgn_Mob&); // not defined, not accessible
};

IMPLEMENT_CLIENTCLASS_DT(C_Escgn_Mob, DT_NPC_Escghmob, CNPC_Escghmob)
END_RECV_TABLE()

C_Escgn_Mob::C_Escgn_Mob()
{
}


C_Escgn_Mob::~C_Escgn_Mob()
{
}