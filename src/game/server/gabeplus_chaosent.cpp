//========= GabePlus Server Chaos Entity ===============================
// MAXIMUM OVERDRIVE – Source SDK Base 2007 / HL2MP
//=====================================================================

#include "cbase.h"
#include "ndebugoverlay.h"
#include "player.h"
#include "ai_basenpc.h"
#include "util.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "tier0/memdbgon.h"

//=====================================================================
class CGabeplusServerChaos : public CBaseEntity
{
public:
	DECLARE_CLASS( CGabeplusServerChaos, CBaseEntity );
	DECLARE_DATADESC();

	void Precache();
	void Spawn();
	void Think();

	bool  m_bChaosEnabled;
	float m_flSpin;
	float m_flHue;
};

LINK_ENTITY_TO_CLASS( gabeplus_chaos, CGabeplusServerChaos );

BEGIN_DATADESC( CGabeplusServerChaos )
	DEFINE_THINKFUNC( Think ),
	DEFINE_FIELD( m_bChaosEnabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flSpin, FIELD_FLOAT ),
	DEFINE_FIELD( m_flHue, FIELD_FLOAT ),
END_DATADESC()

//=====================================================================
static const char *RandomBeep()
{
	static const char *beeps[] =
	{
		"buttons/blip1.wav",
		"buttons/button10.wav",
		"buttons/button14.wav",
		"buttons/combine_button7.wav",
		"common/warning.wav"
	};
	return beeps[ random->RandomInt( 0, ARRAYSIZE( beeps ) - 1 ) ];
}

static void HueToRGB( float h, int &r, int &g, int &b )
{
	h = fmod( h, 360.0f );
	float x = fabs( fmod( h / 60.0f, 2 ) - 1 );
	float c = 1.0f;

	float rf=0, gf=0, bf=0;

	if      ( h < 60 )  { rf=c; gf=x; bf=0; }
	else if ( h < 120 ) { rf=x; gf=c; bf=0; }
	else if ( h < 180 ) { rf=0; gf=c; bf=x; }
	else if ( h < 240 ) { rf=0; gf=x; bf=c; }
	else if ( h < 300 ) { rf=x; gf=0; bf=c; }
	else                { rf=c; gf=0; bf=x; }

	r = (int)( rf * 255 );
	g = (int)( gf * 255 );
	b = (int)( bf * 255 );
}

//=====================================================================
void CGabeplusServerChaos::Precache()
{
	PrecacheModel( "models/error.mdl" );

	PrecacheScriptSound( "buttons/blip1.wav" );
	PrecacheScriptSound( "buttons/button10.wav" );
	PrecacheScriptSound( "buttons/button14.wav" );
	PrecacheScriptSound( "buttons/combine_button7.wav" );
	PrecacheScriptSound( "common/warning.wav" );

	BaseClass::Precache();
}

//=====================================================================
void CGabeplusServerChaos::Spawn()
{
	Precache();
	BaseClass::Spawn();

	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
	AddEffects( EF_NODRAW );

	m_bChaosEnabled = false;
	m_flSpin = 0.0f;
	m_flHue  = 0.0f;

#ifdef _WIN32
	if ( !engine->IsDedicatedServer() )
	{
		int result = MessageBoxA(
			NULL,
			"Continue if you want to see chaos. Click close if you don't want to, and the game will stop. WARNING: This will inevitably crash the game.",
			"GABE MOD +",
			MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL | MB_TOPMOST
		);

		if ( result == IDYES )
			m_bChaosEnabled = true;
		else
		{
			engine->ServerCommand( "quit\n" );
			engine->ServerExecute();
			return;
		}
	}
#endif

	SetNextThink( gpGlobals->curtime + 0.02f );
}

//=====================================================================
void CGabeplusServerChaos::Think()
{
	if ( !m_bChaosEnabled )
		return;

	const float dur = 0.08f;

	m_flSpin += gpGlobals->frametime * 1080.0f;
	m_flHue  += gpGlobals->frametime * 240.0f;

	int cr, cg, cb;
	HueToRGB( m_flHue, cr, cg, cb );

	// --------------------------------------------------
	// GLOBAL OVERLAY STORM
	// --------------------------------------------------
	for ( int i = 0; i < 10; i++ )
	{
		Vector pos(
			random->RandomFloat( -4096, 4096 ),
			random->RandomFloat( -4096, 4096 ),
			random->RandomFloat( 0, 2048 )
		);

		NDebugOverlay::Axis(
			pos,
			QAngle( m_flSpin, m_flSpin * 2, m_flSpin * 0.25f ),
			random->RandomFloat( 64, 256 ),
			true,
			dur
		);

		NDebugOverlay::Circle(
			pos,
			random->RandomFloat( 32, 256 ),
			cr, cg, cb, 200,
			true,
			dur
		);
	}

	// --------------------------------------------------
	// ENTITY CORRUPTION + ORBIT
	// --------------------------------------------------
	for ( int i = 1; i < gpGlobals->maxEntities; i++ )
	{
		CBaseEntity *ent = UTIL_EntityByIndex( i );
		if ( !ent )
			continue;

		Vector o = ent->GetAbsOrigin();
		o.x += cos( m_flSpin * 0.01f ) * 2.0f;
		o.y += sin( m_flSpin * 0.01f ) * 2.0f;
		ent->SetAbsOrigin( o );

		ent->SetRenderColor( cr, cg, cb );

		NDebugOverlay::EntityBounds(
			ent,
			cr, cg, cb,
			255,
			dur
		);

		// Sky laser
		NDebugOverlay::Line(
			Vector( o.x, o.y, o.z + 4096 ),
			o,
			cr, cg, cb,
			true,
			dur
		);

		// NPC corruption
		CAI_BaseNPC *npc = dynamic_cast<CAI_BaseNPC*>( ent );
		if ( npc && npc->IsAlive() )
		{
			if ( Q_stricmp( STRING( npc->GetModelName() ), "models/error.mdl" ) != 0 )
				npc->SetModel( "models/error.mdl" );

			NDebugOverlay::EntityText(
				npc->entindex(),
				0,
				"!!! AI PANIC !!!",
				dur,
				255, 0, 0, 255
			);
		}

		// Props -> error.mdl
		const char *cls = ent->GetClassname();
		if ( cls &&
			( !Q_stricmp( cls, "prop_physics" ) ||
			  !Q_stricmp( cls, "prop_physics_multiplayer" ) ||
			  !Q_stricmp( cls, "prop_dynamic" ) ||
			  !Q_stricmp( cls, "prop_static" ) ) )
		{
			if ( ent->GetModelName() == NULL_STRING ||
				 Q_stricmp( STRING( ent->GetModelName() ), "models/error.mdl" ) != 0 )
			{
				ent->SetModel( "models/error.mdl" );
			}
		}

		// Random parenting chaos
		if ( random->RandomInt( 0, 120 ) == 0 )
		{
			CBaseEntity *other = UTIL_EntityByIndex(
				random->RandomInt( 1, gpGlobals->maxEntities - 1 ) );
			if ( other && other != ent )
				ent->SetParent( other );
		}
	}

	// --------------------------------------------------
	// PLAYER CAMERA TORTURE (2007-CORRECT)
	// --------------------------------------------------
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pl = UTIL_PlayerByIndex( i );
		if ( !pl || !pl->IsAlive() )
			continue;

		QAngle ang = pl->EyeAngles();

		ang[PITCH] += random->RandomFloat( -2.0f, 2.0f );
		ang[YAW]   += random->RandomFloat( -4.0f, 4.0f );
		ang[ROLL]  = sin( m_flSpin * 0.05f ) * 10.0f;

		pl->SnapEyeAngles( ang );
		pl->ViewPunch( QAngle(
			random->RandomFloat( -1.5f, 1.5f ),
			random->RandomFloat( -1.5f, 1.5f ),
			0.0f
		) );
	}

	// --------------------------------------------------
	// AUDIO HELL
	// --------------------------------------------------
	for ( int i = 0; i < 3; i++ )
	{
		EmitSound_t s;
		s.m_pSoundName = RandomBeep();
		s.m_flVolume   = 1.0f;
		s.m_SoundLevel = SNDLVL_130dB;
		s.m_nChannel   = CHAN_STATIC;
		s.m_nPitch     = 60 + (int)( sin( m_flSpin * 0.05f ) * 80 );

		CBroadcastRecipientFilter filter;
		EmitSound( filter, entindex(), s );
	}

	SetNextThink( gpGlobals->curtime + 0.02f );
}
