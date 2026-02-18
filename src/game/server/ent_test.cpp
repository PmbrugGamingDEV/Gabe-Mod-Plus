#include "cbase.h"
#include "engine/IEngineTrace.h"
#include "game/server/iplayerinfo.h"
#include "baseanimating.h"
#include "gamerules.h"
#include "util.h"

class CTestEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(CTestEntity, CBaseAnimating);
	DECLARE_DATADESC();
	CTestEntity()
	{
		m_iTestValue = 0;
	}
	void Spawn() override
	{
		Precache();
		SetModel("models/props_c17/oildrum001.mdl");
		SetSolid(SOLID_BBOX);
		BaseClass::Spawn();
		for (int i = 0; i < 15; ++i) // Increment test value 15 times on spawn
		{
			IncrementTestValueAndPrintToConsole();
		}
	}
	void Precache() override
	{
		PrecacheModel("models/props_c17/oildrum001.mdl");
		BaseClass::Precache();
	}
	void IncrementTestValueAndPrintToConsole()
	{
		m_iTestValue++;
		Msg("CTestEntity: Current Test Value = %d\n", m_iTestValue); // This will happen 15 times on spawn
		// wait a few seconds between each output so that it doesn't flood the console too quickly
		SetThink(&CTestEntity::IncrementTestValueAndPrintToConsole);
		SetNextThink(gpGlobals->curtime + 2.0f); // 2 seconds delay // gpGlobals->curtime is the current server time and +2.0f schedules the next think 2 seconds later

	}

private:
	int m_iTestValue;
};

LINK_ENTITY_TO_CLASS(test_entity, CTestEntity); // Any error here indicates CTestEntity is not derived from CBaseAnimating.

BEGIN_DATADESC(CTestEntity)
END_DATADESC()