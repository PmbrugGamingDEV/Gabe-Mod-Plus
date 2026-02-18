//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_sdkbase.h"
#include "gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "sdk_grenade.h"


#ifdef CLIENT_DLL
	
#else

	#include "player.h"
	#include "items.h"
	#include "sdk_basegrenade_projectile.h"

#endif


#define GRENADE_TIMER	3.0f //Seconds

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSDKGrenade, DT_WeaponSDKGrenade )

BEGIN_NETWORK_TABLE( CWeaponSDKGrenade, DT_WeaponSDKGrenade )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSDKGrenade )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( sdk_grenade, CWeaponSDKGrenade );
PRECACHE_WEAPON_REGISTER( sdk_grenade );

#ifdef GAME_DLL

#define GRENADE_MODEL "models/Weapons/w_eq_fraggrenade_thrown.mdl"

class CGrenadeProjectile : public CBaseGrenadeProjectile
{
public:
	DECLARE_CLASS( CGrenadeProjectile, CBaseGrenadeProjectile );

	// Overrides.
public:
	virtual void Spawn()
	{
		SetModel( GRENADE_MODEL );
		BaseClass::Spawn();
	}

	virtual void Precache()
	{
		PrecacheModel( GRENADE_MODEL );
		BaseClass::Precache();
	}

	// Grenade stuff.
public:

	static CGrenadeProjectile* Create( 
		const Vector &position, 
		const QAngle &angles, 
		const Vector &velocity, 
		const AngularImpulse &angVelocity, 
		CBaseCombatCharacter *pOwner,
		CWeaponSDKBase *pWeapon,
		float timer )
	{
		CGrenadeProjectile *pGrenade = (CGrenadeProjectile*)CBaseEntity::Create( "grenade_projectile", position, angles, pOwner );

		// Set the timer for 1 second less than requested. We're going to issue a SOUND_DANGER
		// one second before detonation.
		pGrenade->SetVelocity( velocity, angVelocity );

		pGrenade->SetDetonateTimerLength( timer );
		pGrenade->SetAbsVelocity( velocity );
		pGrenade->SetupInitialTransmittedGrenadeVelocity( velocity );
		pGrenade->SetThrower( pOwner ); 

		pGrenade->SetGravity( BaseClass::GetGrenadeGravity() );
		pGrenade->SetFriction( BaseClass::GetGrenadeFriction() );
		pGrenade->SetElasticity( BaseClass::GetGrenadeElasticity() );

		pGrenade->m_flDamage = pWeapon->GetSDKWpnData().m_iDamage;
		pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;
		pGrenade->ChangeTeam( pOwner->GetTeamNumber() );
		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );	

		// make NPCs afaid of it while in the air
		pGrenade->SetThink( &CGrenadeProjectile::DangerSoundThink );
		pGrenade->SetNextThink( gpGlobals->curtime );

		return pGrenade;
	}
};

LINK_ENTITY_TO_CLASS( grenade_projectile, CGrenadeProjectile );
PRECACHE_WEAPON_REGISTER( grenade_projectile );

BEGIN_DATADESC( CWeaponSDKGrenade )
END_DATADESC()

void CWeaponSDKGrenade::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer, CWeaponSDKBase *pWeapon )
{
	CGrenadeProjectile::Create( vecSrc, vecAngles, vecVel, angImpulse, pPlayer, pWeapon, GRENADE_TIMER );
}
	
#endif

//Tony; todo; add ACT_MP_PRONE* activities, so we have them.
acttable_t CWeaponSDKGrenade::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE,            ACT_HL2MP_IDLE_GRENADE,            false },
	{ ACT_MP_CROUCH_IDLE,           ACT_HL2MP_IDLE_CROUCH_GRENADE,     false },

	{ ACT_MP_RUN,                   ACT_HL2MP_RUN_GRENADE,             false },
	{ ACT_MP_CROUCHWALK,            ACT_HL2MP_WALK_CROUCH_GRENADE,      false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,   ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,  ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE, false },
};


IMPLEMENT_ACTTABLE( CWeaponSDKGrenade );
