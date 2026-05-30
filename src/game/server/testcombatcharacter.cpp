#include "cbase.h"
#include "npc_combine.h"

#include "tier0/memdbgon.h"

class CNPC_GabeCombine : public CNPC_Combine
{
public:
	DECLARE_CLASS(CNPC_GabeCombine, CNPC_Combine);
	DECLARE_DATADESC();

	void Spawn(void);
	void Precache(void);

	// Override AI behavior
	void PrescheduleThink(void);

private:
	float m_flNextSpeakTime;

	static const char* m_pszVoiceLines[];
};

LINK_ENTITY_TO_CLASS(npc_gabe_combine, CNPC_GabeCombine);

BEGIN_DATADESC(CNPC_GabeCombine)
DEFINE_FIELD(m_flNextSpeakTime, FIELD_TIME),
END_DATADESC()

//-----------------------------------------------------------------------------
// Voice lines (RAW WAV = use PrecacheSound!)
//-----------------------------------------------------------------------------
const char* CNPC_GabeCombine::m_pszVoiceLines[] =
{
	"npc/combine_soldier/vo/alert1.wav",
	"npc/combine_soldier/vo/contact.wav",
	"npc/combine_soldier/vo/coverme.wav",
	"npc/combine_soldier/vo/readyweapons.wav",
	"npc/combine_soldier/vo/sectorclear.wav"
};

//-----------------------------------------------------------------------------
// Precache
//-----------------------------------------------------------------------------
void CNPC_GabeCombine::Precache(void)
{
	PrecacheModel("models/combine_soldier.mdl");

	for (int i = 0; i < ARRAYSIZE(m_pszVoiceLines); i++)
	{
		PrecacheSound(m_pszVoiceLines[i]);
	}

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CNPC_GabeCombine::Spawn(void)
{
	Precache();

	BaseClass::Spawn();

	SetHealth(150);
	SetModel("models/combine_soldier.mdl");

	// Let AI handle movement normally
	// (no forced schedules!)

	m_flNextSpeakTime = gpGlobals->curtime + RandomFloat(2.0f, 5.0f);
}

//-----------------------------------------------------------------------------
// Safe AI hook
//-----------------------------------------------------------------------------
void CNPC_GabeCombine::PrescheduleThink(void)
{
	BaseClass::PrescheduleThink();

	// Only run lightweight logic here
	if (gpGlobals->curtime >= m_flNextSpeakTime)
	{
		int index = RandomInt(0, ARRAYSIZE(m_pszVoiceLines) - 1);

		EmitSound(m_pszVoiceLines[index]);

		m_flNextSpeakTime = gpGlobals->curtime + RandomFloat(4.0f, 8.0f);
	}
}