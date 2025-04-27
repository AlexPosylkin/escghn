//-----------------------------------------------------------------------------
//
//*********************** ESCGHN MODIFICATION SECTION *************************
//                        by AlexPos 2025
//
// you can embed this code into the file funk_tank.cpp - as example
// (the server part of Source dll '\src\game\server\*\func_tank.cpp').
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Turret Escghn gun (Shupogaki train turret)
//-----------------------------------------------------------------------------
class CFuncEscghnGun : public CFuncTank
{
public:
	DECLARE_CLASS(CFuncEscghnGun, CFuncTank);
	DECLARE_DATADESC();

	void Precache(void);
	virtual void Spawn();
	virtual void Activate();
	virtual void Fire(int bulletCount, const Vector& barrelEnd, const Vector& forward, CBaseEntity* pAttacker, bool bIgnoreSpread);
	virtual void ControllerPostFrame();
	virtual void OnStopControlled();
	virtual const char* GetTracerType(void);
	virtual Vector WorldBarrelPosition(void);
	virtual Vector WorldBarrelPosition1(void);
	virtual Vector WorldBarrelPosition2(void);
	virtual Vector WorldBarrelPosition3(void);

private:
	void CreateSounds();
	void DestroySounds();
	void DoMuzzleFlash();
	void StartFiring();
	void StopFiring();

	CSoundPatch* m_pGunFiringSound;
	float		m_flNextHeavyShotTime;
	bool		m_bIsFiring;

	string_t	m_iszAirboatGunModel;  // Models (Warning: use Shupogaki turret models, because this class have 4 muzzles attachments!)
	CHandle<CBaseAnimating> m_hAirboatGunModel;

	// For Shupogaki quadruple machine gun (on train)
	int			m_nGunBarrelAttachment;
	int			m_nGunBarrelAttachment1;
	int			m_nGunBarrelAttachment2;
	int			m_nGunBarrelAttachment3;
	float		m_flLastImpactEffectTime;
};

//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
BEGIN_DATADESC(CFuncEscghnGun)

DEFINE_SOUNDPATCH(m_pGunFiringSound),
DEFINE_FIELD(m_flNextHeavyShotTime, FIELD_TIME),
DEFINE_FIELD(m_bIsFiring, FIELD_BOOLEAN),
DEFINE_KEYFIELD(m_iszAirboatGunModel, FIELD_STRING, "airboat_gun_model"),
DEFINE_FIELD(m_flLastImpactEffectTime, FIELD_TIME),

END_DATADESC()

LINK_ENTITY_TO_CLASS(func_tankescghngun, CFuncEscghnGun);  // EntityClass name is func_tankescghngun - add to *.fgd for Hammer

// fgd config for this entity class name:
//@SolidClass base(BaseTank) = func_tankescghngun : "Escghn Gun Turret"
//[
//	  gun_model(target_destination) : "Name of a prop_dynamic which is the gun model"
//]


//-----------------------------------------------------------------------------
// Precache
//-----------------------------------------------------------------------------
void CFuncEscghnGun::Precache(void)
{
	BaseClass::Precache();
	// TODO: add custom sound library ?
	PrecacheScriptSound("Airboat.FireGunLoop");     // standard airboatgun sound from Half-Life 2
	PrecacheScriptSound("Airboat.FireGunRevDown");
	CreateSounds();
}

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CFuncEscghnGun::Spawn(void)
{
	BaseClass::Spawn();
	m_flNextHeavyShotTime = 0.0f;
	m_bIsFiring = false;
	m_flLastImpactEffectTime = -1;
}

//-----------------------------------------------------------------------------
// Attachment indices (Warning: this funk_tank.. have 4 attachments! Take this into account when using the model.)
//-----------------------------------------------------------------------------
void CFuncEscghnGun::Activate()
{
	BaseClass::Activate();

	if (m_iszAirboatGunModel != NULL_STRING)
	{
		m_hAirboatGunModel = dynamic_cast<CBaseAnimating*>(gEntList.FindEntityByName(NULL, m_iszAirboatGunModel));
		if (m_hAirboatGunModel)
		{
			m_nGunBarrelAttachment = m_hAirboatGunModel->LookupAttachment("muzzle");
			m_nGunBarrelAttachment1 = m_hAirboatGunModel->LookupAttachment("muzzle1");
			m_nGunBarrelAttachment2 = m_hAirboatGunModel->LookupAttachment("muzzle2");
			m_nGunBarrelAttachment3 = m_hAirboatGunModel->LookupAttachment("muzzle3");
		}
	}
}

//-----------------------------------------------------------------------------
// Create/destroy looping sounds 
//-----------------------------------------------------------------------------
void CFuncEscghnGun::CreateSounds()
{
	CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();

	CPASAttenuationFilter filter(this);
	if (!m_pGunFiringSound)
	{
		m_pGunFiringSound = controller.SoundCreate(filter, entindex(), "Airboat.FireGunLoop");

		controller.Play(m_pGunFiringSound, 0, 180); // Pitch 180 for fast shooting sound effect
	}
}

void CFuncEscghnGun::DestroySounds()
{
	CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();

	controller.SoundDestroy(m_pGunFiringSound);
	m_pGunFiringSound = NULL;
}


//-----------------------------------------------------------------------------
// Stop Firing sounds
//-----------------------------------------------------------------------------
void CFuncEscghnGun::StartFiring()
{
	if (!m_bIsFiring)
	{
		CSoundEnvelopeController* pController = &CSoundEnvelopeController::GetController();
		float flVolume = pController->SoundGetVolume(m_pGunFiringSound);
		pController->SoundChangeVolume(m_pGunFiringSound, 1.0f, 0.1f * (1.0f - flVolume));
		m_bIsFiring = true;
	}
}

void CFuncEscghnGun::StopFiring()
{
	if (m_bIsFiring)
	{
		CSoundEnvelopeController* pController = &CSoundEnvelopeController::GetController();
		float flVolume = pController->SoundGetVolume(m_pGunFiringSound);
		pController->SoundChangeVolume(m_pGunFiringSound, 0.0f, 0.1f * flVolume);
		EmitSound("Airboat.FireGunRevDown");
		m_bIsFiring = false;
	}
}

//-----------------------------------------------------------------------------
// Maintains gun sounds
//-----------------------------------------------------------------------------
void CFuncEscghnGun::ControllerPostFrame(void)
{
	if (IsPlayerManned())
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(GetController());
		if (pPlayer->m_nButtons & IN_ATTACK)
		{
			StartFiring();
		}
		else
		{
			StopFiring();
		}
	}

	BaseClass::ControllerPostFrame();
}

//-----------------------------------------------------------------------------
// Stop controlled
//-----------------------------------------------------------------------------
void CFuncEscghnGun::OnStopControlled()
{
	StopFiring();
	BaseClass::OnStopControlled();
}

//-----------------------------------------------------------------------------
// Barrel position for 4 barrels (by Attachments)
//-----------------------------------------------------------------------------
Vector CFuncEscghnGun::WorldBarrelPosition(void)
{
	if (!m_hAirboatGunModel || (m_nGunBarrelAttachment == 0))
	{
		return BaseClass::WorldBarrelPosition();
	}

	Vector vecOrigin;
	m_hAirboatGunModel->GetAttachment(m_nGunBarrelAttachment, vecOrigin);
	return vecOrigin;
}

Vector CFuncEscghnGun::WorldBarrelPosition1(void)
{
	if (!m_hAirboatGunModel || (m_nGunBarrelAttachment1 == 0))
	{
		return BaseClass::WorldBarrelPosition();
	}

	Vector vecOrigin;
	m_hAirboatGunModel->GetAttachment(m_nGunBarrelAttachment1, vecOrigin);
	return vecOrigin;
}

Vector CFuncEscghnGun::WorldBarrelPosition2(void)
{
	if (!m_hAirboatGunModel || (m_nGunBarrelAttachment2 == 0))
	{
		return BaseClass::WorldBarrelPosition();
	}

	Vector vecOrigin;
	m_hAirboatGunModel->GetAttachment(m_nGunBarrelAttachment2, vecOrigin);
	return vecOrigin;
}

Vector CFuncEscghnGun::WorldBarrelPosition3(void)
{
	if (!m_hAirboatGunModel || (m_nGunBarrelAttachment3 == 0))
	{
		return BaseClass::WorldBarrelPosition();
	}

	Vector vecOrigin;
	m_hAirboatGunModel->GetAttachment(m_nGunBarrelAttachment3, vecOrigin);
	return vecOrigin;
}

//-----------------------------------------------------------------------------
// TracerType (standard tracer types from https://developer.valvesoftware.com/wiki/List_of_Client_Effects)
//-----------------------------------------------------------------------------
const char* CFuncEscghnGun::GetTracerType(void)
{
	if (gpGlobals->curtime >= m_flNextHeavyShotTime)
		return "AirboatGunHeavyTracer";
	//return "";

	return "AirboatGunTracer";
	//return "";
}

//-----------------------------------------------------------------------------
// MuzzleFlash by attachments
//-----------------------------------------------------------------------------
void CFuncEscghnGun::DoMuzzleFlash(void)
{
	if (m_hAirboatGunModel && (m_nGunBarrelAttachment != 0))
	{
		CEffectData data;
		data.m_nEntIndex = m_hAirboatGunModel->entindex();
		data.m_nAttachmentIndex = m_nGunBarrelAttachment;
		data.m_flScale = 1.0f;
		DispatchEffect("AirboatMuzzleFlash", data);
	}
	if (m_hAirboatGunModel && (m_nGunBarrelAttachment1 != 0))
	{
		CEffectData data;
		data.m_nEntIndex = m_hAirboatGunModel->entindex();
		data.m_nAttachmentIndex = m_nGunBarrelAttachment1;
		data.m_flScale = 1.0f;
		DispatchEffect("AirboatMuzzleFlash", data);
	}
	if (m_hAirboatGunModel && (m_nGunBarrelAttachment2 != 0))
	{
		CEffectData data;
		data.m_nEntIndex = m_hAirboatGunModel->entindex();
		data.m_nAttachmentIndex = m_nGunBarrelAttachment2;
		data.m_flScale = 1.0f;
		DispatchEffect("AirboatMuzzleFlash", data);
	}
	if (m_hAirboatGunModel && (m_nGunBarrelAttachment3 != 0))
	{
		CEffectData data;
		data.m_nEntIndex = m_hAirboatGunModel->entindex();
		data.m_nAttachmentIndex = m_nGunBarrelAttachment3;
		data.m_flScale = 1.0f;
		DispatchEffect("AirboatMuzzleFlash", data);
	}
}

//-----------------------------------------------------------------------------
// Fires bullets
//-----------------------------------------------------------------------------
#define ESCGHNGUN_HEAVY_SHOT_INTERVAL	0.2f
#define ESCGHNGUN_TRACEFREQENCY			3
#define ESCGHNGUN_VECTOR_CONE_01		Vector(0.014,0.013,0.018) // Pretty small conus
#define ESCGHNGUN_VECTOR_CONE_02		Vector(0.024,0.023,0.028) // Medium conus
#define ESCGHNGUN_VECTOR_CONE_03		Vector(0.031,0.030,0.032) // Large conus

void CFuncEscghnGun::Fire(int bulletCount, const Vector& barrelEnd, const Vector& forward, CBaseEntity* pAttacker, bool bIgnoreSpread)
{
	CAmmoDef* pAmmoDef = GetAmmoDef();
	int ammoType = pAmmoDef->Index("AirboatGun");	// AmmoType from default shared types of Half-Life 2

	FireBulletsInfo_t info;
	FireBulletsInfo_t info1;
	FireBulletsInfo_t info2;
	FireBulletsInfo_t info3;

	// info fire
	info.m_vecSrc = WorldBarrelPosition();
	info.m_vecDirShooting = forward;
	info.m_flDistance = 4096;
	info.m_iAmmoType = ammoType;
	info.m_iTracerFreq = ESCGHNGUN_TRACEFREQENCY;

	// info fire1
	info1.m_vecSrc = WorldBarrelPosition1();
	info1.m_vecDirShooting = forward;
	info1.m_flDistance = 4096;
	info1.m_iAmmoType = ammoType;
	info1.m_iTracerFreq = ESCGHNGUN_TRACEFREQENCY;

	// info fire2
	info2.m_vecSrc = WorldBarrelPosition2();
	info2.m_vecDirShooting = forward;
	info2.m_flDistance = 4096;
	info2.m_iAmmoType = ammoType;
	info2.m_iTracerFreq = ESCGHNGUN_TRACEFREQENCY;

	// info fire3
	info3.m_vecSrc = WorldBarrelPosition3();
	info3.m_vecDirShooting = forward;
	info3.m_flDistance = 4096;
	info3.m_iAmmoType = ammoType;
	info3.m_iTracerFreq = ESCGHNGUN_TRACEFREQENCY;

	//Warning("Fire barrelEnd: X: %.2f Y: %.2f Z: %.2f \n", barrelEnd.x, barrelEnd.y, barrelEnd.z);
	if (gpGlobals->curtime >= m_flNextHeavyShotTime)
	{
		info.m_iShots = 3;
		info.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;
		info.m_flDamageForceScale = 1000.0f;

		info1.m_iShots = 3;
		info1.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;
		info1.m_flDamageForceScale = 1000.0f;

		info2.m_iShots = 3;
		info2.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;
		info2.m_flDamageForceScale = 1000.0f;

		info3.m_iShots = 3;
		info3.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;
		info3.m_flDamageForceScale = 1000.0f;
	}
	else
	{
		info.m_iShots = 6;
		info.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;

		info1.m_iShots = 6;
		info1.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;

		info2.m_iShots = 6;
		info2.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;

		info3.m_iShots = 6;
		info3.m_vecSpread = ESCGHNGUN_VECTOR_CONE_01;
	}

	FireBullets(info);
	FireBullets(info1);
	FireBullets(info2);
	FireBullets(info3);

	DoMuzzleFlash();

	if (gpGlobals->curtime >= m_flNextHeavyShotTime)
	{
		m_flNextHeavyShotTime = gpGlobals->curtime + ESCGHNGUN_HEAVY_SHOT_INTERVAL;
	}
}

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
