
//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
    #include "point_camera.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifdef CLIENT_DLL
#define CWeapon357Camera C_Weapon357Camera
#endif

//-----------------------------------------------------------------------------
// CWeapon357Camera
//-----------------------------------------------------------------------------

class CWeapon357Camera : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeapon357Camera, CBaseHL2MPCombatWeapon );
public:

	CWeapon357Camera( void );

	void	PrimaryAttack( void );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

private:
	
	CWeapon357Camera( const CWeapon357Camera & );

	EHANDLE m_hCamera;
};

IMPLEMENT_NETWORKCLASS_ALIASED( Weapon357Camera, DT_Weapon357Camera )

BEGIN_NETWORK_TABLE( CWeapon357Camera, DT_Weapon357Camera )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeapon357Camera )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_357camera, CWeapon357Camera );
PRECACHE_WEAPON_REGISTER( weapon_357camera );


acttable_t CWeapon357Camera::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PISTOL,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PISTOL,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PISTOL,					false },
};

IMPLEMENT_ACTTABLE( CWeapon357Camera );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon357Camera::CWeapon357Camera( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeapon357Camera::PrimaryAttack(void)
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

#ifndef CLIENT_DLL
    // Create camera if it doesn't exist
    CPointCamera* pCam = dynamic_cast<CPointCamera*>(m_hCamera.Get());
    if (!pCam)
    {
        pCam = static_cast<CPointCamera*>(
            CreateEntityByName("point_camera")
            );

        if (!pCam)
            return;

        pCam->Spawn();
        pCam->Activate();

        m_hCamera = pCam;
    }

    // Update camera position & angles
    Vector camPos = pPlayer->EyePosition();
    QAngle camAng = pPlayer->EyeAngles();

    // Offset slightly forward
    Vector forward;
    AngleVectors(camAng, &forward);
    camPos += forward * 16.0f;

    pCam->SetAbsOrigin(camPos);
    pCam->SetAbsAngles(camAng);

    pCam->KeyValue("targetname", "_rt_weaponcam");
    pCam->KeyValue("spawnflags", "1");
    pCam->KeyValue("fov", "75");
    pCam->KeyValue("active", "1");
#endif

    // Cooldown only
    m_flNextPrimaryAttack = gpGlobals->curtime + 0.1f;
}