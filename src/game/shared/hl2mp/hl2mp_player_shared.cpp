//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"

#ifdef CLIENT_DLL
#include "c_hl2mp_player.h"
#include "prediction.h"
#define CRecipientFilter C_RecipientFilter
#else
#include "hl2mp_player.h"
#endif

#include "hl2mp_gamerules.h"

#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "igamesystem.h" // Make sure this is included for GAMESTATE_ACTIVE

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sv_footsteps;

const char* g_ppszPlayerSoundPrefixNames[PLAYER_SOUNDS_MAX] =
{
	"NPC_Citizen",
	"NPC_CombineS",
	"NPC_MetroPolice",
};

const char* CHL2MP_Player::GetPlayerModelSoundPrefix(void)
{
	return g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType];
}

void CHL2MP_Player::PrecacheFootStepSounds(void)
{
	int iFootstepSounds = ARRAYSIZE(g_ppszPlayerSoundPrefixNames);
	int i;

	for (i = 0; i < iFootstepSounds; ++i)
	{
		char szFootStepName[128];

		Q_snprintf(szFootStepName, sizeof(szFootStepName), "%s.RunFootstepLeft", g_ppszPlayerSoundPrefixNames[i]);
		PrecacheScriptSound(szFootStepName);

		Q_snprintf(szFootStepName, sizeof(szFootStepName), "%s.RunFootstepRight", g_ppszPlayerSoundPrefixNames[i]);
		PrecacheScriptSound(szFootStepName);
	}
}

//-----------------------------------------------------------------------------
// Consider the weapon's built-in accuracy, this character's proficiency with
// the weapon, and the status of the target. Use this information to determine
// how accurately to shoot at the target.
//-----------------------------------------------------------------------------
Vector CHL2MP_Player::GetAttackSpread(CBaseCombatWeapon* pWeapon, CBaseEntity* pTarget)
{
	if (pWeapon)
		return pWeapon->GetBulletSpread(WEAPON_PROFICIENCY_PERFECT);

	return VECTOR_CONE_15DEGREES;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : step - 
//			fvol - 
//			force - force sound to play
//-----------------------------------------------------------------------------

ConVar gabeplus_permaterialfootsteps(
    "gabeplus_permaterialfootsteps",
    "1",
    FCVAR_REPLICATED | FCVAR_ARCHIVE,
    "0 = team-based footsteps, 1 = per-material footsteps"
);

void CHL2MP_Player::PlayStepSound(
    Vector& vecOrigin,
    surfacedata_t* psurface,
    float fvol,
    bool force)
{
    if (gpGlobals->maxClients > 1 && !sv_footsteps.GetFloat())
        return;

#if defined(CLIENT_DLL)
    if (!prediction->IsFirstTimePredicted())
        return;
#endif

    if (GetFlags() & FL_DUCKING)
        return;

    m_Local.m_nStepside = ~m_Local.m_nStepside;

    CSoundParameters params;
    const char* pFinalSound = NULL;

    // --------------------------------------------------
    // MODE 1: Surface-based footsteps
    // --------------------------------------------------
    if (gabeplus_permaterialfootsteps.GetInt() == 1)
    {
        if (!psurface)
            return;

        int nSide = m_Local.m_nStepside;

        unsigned short stepSoundName =
            nSide ? psurface->sounds.stepleft
            : psurface->sounds.stepright;

        if (!stepSoundName)
            return;

        IPhysicsSurfaceProps* physprops =
            MoveHelper()->GetSurfaceProps();

        const char* pSoundName =
            physprops->GetString(stepSoundName);

        if (!pSoundName || !pSoundName[0])
            return;

        if (!GetParametersForSound(pSoundName, params, NULL))
            return;

        pFinalSound = params.soundname;
    }
    // --------------------------------------------------
    // MODE 0: Team-based footsteps
    // --------------------------------------------------
    else
    {
        char szStepSound[128];

        if (m_Local.m_nStepside)
        {
            Q_snprintf(
                szStepSound,
                sizeof(szStepSound),
                "%s.RunFootstepLeft",
                g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType]);
        }
        else
        {
            Q_snprintf(
                szStepSound,
                sizeof(szStepSound),
                "%s.RunFootstepRight",
                g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType]);
        }

        if (!GetParametersForSound(szStepSound, params, NULL))
            return;

        pFinalSound = params.soundname;
    }

    // --------------------------------------------------
    // Emit sound
    // --------------------------------------------------

    CRecipientFilter filter;
    filter.AddRecipientsByPAS(vecOrigin);

#ifndef CLIENT_DLL
    if (gpGlobals->maxClients > 1)
        filter.RemoveRecipientsByPVS(vecOrigin);
#endif

    EmitSound_t ep;
    ep.m_nChannel = CHAN_BODY;
    ep.m_pSoundName = pFinalSound;
    ep.m_flVolume = fvol;
    ep.m_SoundLevel = params.soundlevel;
    ep.m_nFlags = 0;
    ep.m_nPitch = params.pitch;
    ep.m_pOrigin = &vecOrigin;

    EmitSound(filter, entindex(), ep);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHL2MP_Player::ShouldCollide(int collisionGroup, int contentsMask) const
{
	if (HL2MPRules()->IsTeamplay())
	{
		if (collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT || collisionGroup == COLLISION_GROUP_PROJECTILE)
		{
			switch (GetTeamNumber())
			{
			case TEAM_REBELS:
				if (!(contentsMask & CONTENTS_TEAM2))
					return false;
				break;

			case TEAM_COMBINE:
				if (!(contentsMask & CONTENTS_TEAM1))
					return false;
				break;
			}
		}
	}

	return BaseClass::ShouldCollide(collisionGroup, contentsMask);
}
