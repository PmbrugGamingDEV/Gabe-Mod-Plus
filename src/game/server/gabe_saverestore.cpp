#include "cbase.h"
#include "gabe_saverestore.h"
#include "filesystem.h"
#include "entitylist.h"
#include "KeyValues.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

static const char* GABE_SAVE_DIR = "SAVE";

void CGabeSave::WriteEntityBasic(CBaseEntity* pEnt)
{
	if (!pEnt)
		return;

	const char* pszClassname = pEnt->GetClassname();
	const char* pszModel = STRING(pEnt->GetModelName());

	Vector vecOrigin = pEnt->GetAbsOrigin();
	QAngle angAngles = pEnt->GetAbsAngles();

	int iHealth = pEnt->GetHealth();

	WriteString("classname", pszClassname);
	WriteString("model", pszModel);
	WriteVector("origin", vecOrigin);

	Vector vecAngles(angAngles.x, angAngles.y, angAngles.z);
	WriteVector("angles", vecAngles);

	WriteInt("health", &iHealth, 1);
}

CBaseEntity* CGabeRestore::ReadEntityBasic()
{
	char szClassname[256];
	char szModel[MAX_PATH];

	Vector vecOrigin;
	Vector vecAngles;
	int iHealth = 0;

	StartBlock();

	ReadString(szClassname, sizeof(szClassname), 0);
	ReadString(szModel, sizeof(szModel), 0);
	ReadVector(&vecOrigin, 1, sizeof(Vector));
	ReadVector(&vecAngles, 1, sizeof(Vector));
	ReadInt(&iHealth, 1, sizeof(int));

	EndBlock();

	if (szClassname[0] == 0)
		return NULL;

	CBaseEntity* pEnt = CreateEntityByName(szClassname);
	if (!pEnt)
		return NULL;

	if (szModel[0])
	{
		pEnt->PrecacheModel(szModel);
		pEnt->SetModel(szModel);
	}

	pEnt->SetAbsOrigin(vecOrigin);
	pEnt->SetAbsAngles(QAngle(vecAngles.x, vecAngles.y, vecAngles.z));

	DispatchSpawn(pEnt);
	pEnt->Activate();

	if (iHealth > 0)
		pEnt->SetHealth(iHealth);

	return pEnt;
}