//=========			NPC Gehenna mob (Blue Archive) by AlexPos	   ============//
//
// Purpose: Client part of Gehenna mob                                         //
//
//=============================================================================//

#include "cbase.h"
#include "c_ai_basenpc.h"

#include "tier0/memdbgon.h"

class C_Escghmob : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_Escghmob, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_Escghmob();
	virtual ~C_Escghmob();

private:
	C_Escghmob(const C_Escghmob&);

};

IMPLEMENT_CLIENTCLASS_DT(C_Escghmob, DT_NPC_Escghmob, CNPC_Escghmob)
END_RECV_TABLE()

C_Escghmob::C_Escghmob()
{
}

C_Escghmob::~C_Escghmob()
{
}