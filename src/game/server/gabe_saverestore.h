#ifndef GABE_SAVERESTORE_H
#define GABE_SAVERESTORE_H

#include "cbase.h"
#include "saverestore.h"

class CGabeSave : public CSave
{
public:
	CGabeSave(CSaveRestoreData* pData) : CSave(pData)
	{}

	void WriteEntityBasic(CBaseEntity* pEnt);
};

class CGabeRestore : public CRestore
{
public:
	CGabeRestore(CSaveRestoreData* pData) : CRestore(pData)
	{}

	CBaseEntity* ReadEntityBasic();
};

void Gabe_SaveGame(const char* pszName);
void Gabe_LoadGame(const char* pszName);

#endif