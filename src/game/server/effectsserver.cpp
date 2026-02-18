//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Utility code.
//
//=============================================================================//

#include "cbase.h"
#include "te.h"
#include "shake.h"
#include "decals.h"
#include "IEffects.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern short g_sModelIndexSmoke;
extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;

//-----------------------------------------------------------------------------
// Client-server neutral effects interface (SERVER IMPLEMENTATION)
//-----------------------------------------------------------------------------
class CEffectsServer : public IEffects
{
public:
	CEffectsServer() {}
	virtual ~CEffectsServer() {}

	// IEffects interface
	virtual void Beam(
		const Vector& Start, const Vector& End, int nModelIndex,
		int nHaloIndex, unsigned char frameStart, unsigned char frameRate,
		float flLife, unsigned char width, unsigned char endWidth,
		unsigned char fadeLength, unsigned char noise,
		unsigned char red, unsigned char green, unsigned char blue,
		unsigned char brightness, unsigned char speed);

	virtual void Smoke(const Vector& origin, int mModel, float flScale, float flFramerate);
	virtual void Sparks(const Vector& position, int nMagnitude = 1, int nTrailLength = 1, const Vector* pvecDir = NULL);
	virtual void Dust(const Vector& pos, const Vector& dir, float size, float speed);
	virtual void MuzzleFlash(const Vector& origin, const QAngle& angles, float scale, int type);
	virtual void MetalSparks(const Vector& position, const Vector& direction);
	virtual void EnergySplash(const Vector& position, const Vector& direction, bool bExplosive = false);
	virtual void Ricochet(const Vector& position, const Vector& direction);

	virtual float Time() { return gpGlobals->curtime; }
	virtual bool IsServer() { return true; }
	virtual void SuppressEffectsSounds(bool) {}
};

//-----------------------------------------------------------------------------
// Global interface
//-----------------------------------------------------------------------------
static CEffectsServer s_EffectsServer;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(
	CEffectsServer,
	IEffects,
	IEFFECTS_INTERFACE_VERSION,
	s_EffectsServer
);

IEffects* g_pEffects = &s_EffectsServer;

//-----------------------------------------------------------------------------
// EFFECT IMPLEMENTATIONS (ALL BROADCAST — MP SAFE)
//-----------------------------------------------------------------------------

void CEffectsServer::Beam(
	const Vector& vecStart, const Vector& vecEnd,
	int nModelIndex, int nHaloIndex,
	unsigned char frameStart, unsigned char frameRate,
	float flLife, unsigned char width, unsigned char endWidth,
	unsigned char fadeLength, unsigned char noise,
	unsigned char red, unsigned char green, unsigned char blue,
	unsigned char brightness, unsigned char speed)
{
	CBroadcastRecipientFilter filter;

	te->BeamPoints(
		filter,
		0.0f,
		&vecStart,
		&vecEnd,
		nModelIndex,
		nHaloIndex,
		frameStart,
		frameRate,
		flLife,
		width,
		endWidth,
		fadeLength,
		noise,
		red,
		green,
		blue,
		brightness,
		speed
	);
}

void CEffectsServer::Smoke(const Vector& origin, int mModel, float flScale, float flFramerate)
{
	CBroadcastRecipientFilter filter;

	te->Smoke(
		filter,
		0.0f,
		&origin,
		mModel,
		flScale,
		flFramerate
	);
}

void CEffectsServer::Sparks(const Vector& position, int nMagnitude, int nTrailLength, const Vector* pvecDir)
{
	CBroadcastRecipientFilter filter;

	te->Sparks(
		filter,
		0.0f,
		&position,
		nMagnitude,
		nTrailLength,
		pvecDir
	);
}

void CEffectsServer::Dust(const Vector& pos, const Vector& dir, float size, float speed)
{
	CBroadcastRecipientFilter filter;

	te->Dust(
		filter,
		0.0f,
		pos,
		dir,
		size,
		speed
	);
}

void CEffectsServer::MuzzleFlash(const Vector& origin, const QAngle& angles, float scale, int type)
{
	CBroadcastRecipientFilter filter;

	te->MuzzleFlash(
		filter,
		0.0f,
		origin,
		angles,
		scale,
		type
	);
}

void CEffectsServer::MetalSparks(const Vector& position, const Vector& direction)
{
	CBroadcastRecipientFilter filter;

	te->MetalSparks(
		filter,
		0.0f,
		&position,
		&direction
	);
}

void CEffectsServer::EnergySplash(const Vector& position, const Vector& direction, bool bExplosive)
{
	CBroadcastRecipientFilter filter;

	te->EnergySplash(
		filter,
		0.0f,
		&position,
		&direction,
		bExplosive
	);
}

void CEffectsServer::Ricochet(const Vector& position, const Vector& direction)
{
	CBroadcastRecipientFilter filter;

	te->ArmorRicochet(
		filter,
		0.0f,
		&position,
		&direction
	);
}
