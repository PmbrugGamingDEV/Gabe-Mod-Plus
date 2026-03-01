//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_basetempentity.h"
#include "iefx.h"
#include "fx.h"
#include "decals.h"
#include "materialsystem/IMaterialSystem.h"
#include "filesystem.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterialvar.h"
#include "ClientEffectPrecacheSystem.h"
#include "tier0/vprof.h"

#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar cl_playerspraydisable( "cl_playerspraydisable", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, "Disable player sprays." );

#ifndef _XBOX
CLIENTEFFECT_REGISTER_BEGIN( PrecachePlayerDecal )
CLIENTEFFECT_MATERIAL( "decals/playerlogo01" )
#if !defined(HL2_DLL) || defined(HL2MP)
CLIENTEFFECT_MATERIAL( "decals/playerlogo02" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo03" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo04" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo05" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo06" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo07" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo08" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo09" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo10" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo11" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo12" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo13" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo14" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo15" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo16" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo17" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo18" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo19" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo20" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo21" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo22" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo23" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo24" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo25" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo26" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo27" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo28" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo29" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo30" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo31" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo32" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo33" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo34" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo35" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo36" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo37" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo38" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo39" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo40" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo41" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo42" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo43" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo44" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo45" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo46" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo47" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo48" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo49" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo40" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo41" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo42" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo43" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo44" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo45" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo46" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo47" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo48" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo49" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo50" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo51" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo52" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo53" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo54" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo55" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo56" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo57" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo58" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo59" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo60" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo61" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo62" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo63" )
CLIENTEFFECT_MATERIAL( "decals/playerlogo64" )
#endif
CLIENTEFFECT_REGISTER_END()
#endif

//-----------------------------------------------------------------------------
// Purpose: Player Decal TE
//-----------------------------------------------------------------------------
class C_TEPlayerDecal : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerDecal, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

					C_TEPlayerDecal( void );
	virtual			~C_TEPlayerDecal( void );

	virtual void	PostDataUpdate( DataUpdateType_t updateType );

	virtual void	Precache( void );

public:
	int				m_nPlayer;
	Vector			m_vecOrigin;
	int				m_nEntity;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TEPlayerDecal::C_TEPlayerDecal( void )
{
	m_nPlayer = 0;
	m_vecOrigin.Init();
	m_nEntity = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TEPlayerDecal::~C_TEPlayerDecal( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TEPlayerDecal::Precache( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : filter - 
//			delay - 
//			pos - 
//			player - 
//			entity - 
//-----------------------------------------------------------------------------

bool LoadLocalSteamSpray()
{
    if (!steamapicontext || !steamapicontext->SteamUser())
        return false;

    CSteamID steamID = steamapicontext->SteamUser()->GetSteamID();
    uint32 appID = engine->GetAppID();

    char sprayPath[MAX_PATH];
    Q_snprintf(
        sprayPath,
        sizeof(sprayPath),
        "userdata/%llu/%u/remote/spray.vtf",
        steamID.ConvertToUint64(),
        appID
    );

    if (!filesystem->FileExists(sprayPath))
        return false;

    // Copy to materials/temp
    if (!filesystem->FileExists("materials/temp/spray.vtf"))
    {
        engine->CopyFile(sprayPath, "materials/temp/spray.vtf");
    }

    return true;
}

void TE_PlayerDecal(IRecipientFilter& filter, float delay,
    const Vector* pos, int player, int entity)
{
    if (cl_playerspraydisable.GetBool())
        return;

    if (!r_decals.GetBool())
        return;

    C_BaseEntity* ent = cl_entitylist->GetEnt(entity);
    if (!ent)
        return;

    player_info_t info;
    engine->GetPlayerInfo(player, &info);

    IMaterial* logo = NULL;

    //---------------------------------------------------------
    // CASE 1: Steam CRC exists (MP or properly initialized SP)
    //---------------------------------------------------------
    if (info.customFiles[0] != 0)
    {
        logo = materials->FindMaterial(
            VarArgs("decals/playerlogo%2.2d", player),
            TEXTURE_GROUP_DECAL);

        if (IsErrorMaterial(logo))
            return;

        char logohex[16];
        Q_binarytohex(
            (byte*)&info.customFiles[0],
            sizeof(info.customFiles[0]),
            logohex,
            sizeof(logohex));

        char texname[512];
        Q_snprintf(texname, sizeof(texname), "temp/%s", logohex);

        char fulltexname[512];
        Q_snprintf(fulltexname, sizeof(fulltexname),
            "materials/temp/%s.vtf", logohex);

        if (!filesystem->FileExists(fulltexname))
        {
            char custname[512];
            Q_snprintf(custname, sizeof(custname),
                "downloads/%s.dat", logohex);

            if (filesystem->FileExists(custname))
            {
                engine->CopyFile(custname, fulltexname);
            }
        }

        ITexture* texture =
            materials->FindTexture(texname, TEXTURE_GROUP_DECAL);

        if (!IsErrorTexture(texture))
        {
            bool bFound = false;
            IMaterialVar* pMatVar =
                logo->FindVar("$basetexture", &bFound);

            if (bFound && pMatVar)
            {
                pMatVar->SetTextureValue(texture);
                logo->RefreshPreservingMaterialVars();
            }
        }
    }
    //---------------------------------------------------------
    // CASE 2: Singleplayer fallback (no CRC available)
    //---------------------------------------------------------
    else
    {
        // Directly use local spray file
        // This assumes you have:
        // materials/temp/spray.vtf
        // materials/temp/spray.vmt

        logo = materials->FindMaterial(
            "temp/spray",
            TEXTURE_GROUP_DECAL);

        if (IsErrorMaterial(logo))
        {
            // fallback to default playerlogo if temp not found
            logo = materials->FindMaterial(
                "decals/playerlogo01",
                TEXTURE_GROUP_DECAL);

            if (IsErrorMaterial(logo))
                return;
        }
    }

    //---------------------------------------------------------
    // Apply decal
    //---------------------------------------------------------

    color32 rgbaColor = { 255, 255, 255, 255 };

    effects->PlayerDecalShoot(
        logo,
        (void*)player,
        entity,
        ent->GetModel(),
        ent->GetAbsOrigin(),
        ent->GetAbsAngles(),
        *pos,
        0,
        0,
        rgbaColor);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void C_TEPlayerDecal::PostDataUpdate( DataUpdateType_t updateType )
{
#ifndef _XBOX
	VPROF( "C_TEPlayerDecal::PostDataUpdate" );

	// Decals disabled?
	if ( !r_decals.GetBool() )
		return;

	CLocalPlayerFilter filter;
	TE_PlayerDecal(  filter, 0.0f, &m_vecOrigin, m_nPlayer, m_nEntity );
#endif
}

IMPLEMENT_CLIENTCLASS_EVENT_DT(C_TEPlayerDecal, DT_TEPlayerDecal, CTEPlayerDecal)
	RecvPropVector( RECVINFO(m_vecOrigin)),
	RecvPropInt( RECVINFO(m_nEntity)),
	RecvPropInt( RECVINFO(m_nPlayer)),
END_RECV_TABLE()
