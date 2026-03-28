//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_basehlplayer.h"
#include "vguimatsurface/IMatSystemSurface.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMesh.h"
#include "materialsystem/imaterialvar.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Draws the zoom screen
//-----------------------------------------------------------------------------
class CHudZoom : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CHudZoom, vgui::Panel);

public:
	CHudZoom(const char* pElementName);

	bool	ShouldDraw(void);
	void	Init(void);
	void	LevelInit(void);

protected:
	virtual void ApplySchemeSettings(vgui::IScheme* scheme);
	virtual void Paint(void);

private:
	bool	m_bZoomOn;
	float	m_flZoomStartTime;
	bool	m_bPainted;

	CPanelAnimationVarAliasType(float, m_flCircle1Radius, "Circle1Radius", "66", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flCircle2Radius, "Circle2Radius", "74", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flDashGap, "DashGap", "16", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flDashHeight, "DashHeight", "4", "proportional_float");

	CMaterialReference m_ZoomMaterial;
};

DECLARE_HUDELEMENT(CHudZoom);

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudZoom::CHudZoom(const char* pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudZoom")
{
	vgui::Panel* pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudZoom::Init(void)
{
	m_bZoomOn = false;
	m_bPainted = false;
	m_flZoomStartTime = -999.0f;
	m_ZoomMaterial.Init("vgui/zoom", TEXTURE_GROUP_VGUI);
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudZoom::LevelInit(void)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CHudZoom::ApplySchemeSettings(vgui::IScheme* scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetFgColor(scheme->GetColor("ZoomReticleColor", GetFgColor()));

	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudZoom::ShouldDraw(void)
{
	bool bNeedsDraw = false;

	C_BaseHLPlayer* pPlayer = dynamic_cast<C_BaseHLPlayer*>(C_BasePlayer::GetLocalPlayer());
	if (pPlayer == NULL)
		return false;

	if (pPlayer->m_HL2Local.m_bZooming)
	{
		// need to paint
		bNeedsDraw = true;
	}
	else if (m_bPainted)
	{
		// keep painting until state is finished
		bNeedsDraw = true;
	}

	return (bNeedsDraw && CHudElement::ShouldDraw());
}

#define	ZOOM_FADE_TIME	0.4f
//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CHudZoom::Paint(void)
{
	m_bPainted = false;

	C_BaseHLPlayer* pPlayer = dynamic_cast<C_BaseHLPlayer*>(C_BasePlayer::GetLocalPlayer());
	if (pPlayer == NULL)
		return;

	if (pPlayer->m_HL2Local.m_bZooming && m_bZoomOn == false)
	{
		m_bZoomOn = true;
		m_flZoomStartTime = gpGlobals->curtime;
	}
	else if (pPlayer->m_HL2Local.m_bZooming == false && m_bZoomOn)
	{
		m_bZoomOn = false;
		m_flZoomStartTime = gpGlobals->curtime;
	}

	float deltaTime = (gpGlobals->curtime - m_flZoomStartTime);
	float scale = clamp(deltaTime / ZOOM_FADE_TIME, 0.0f, 1.0f);

	float alpha;

	if (m_bZoomOn)
	{
		alpha = scale;
	}
	else
	{
		if (scale >= 1.0f)
			return;

		alpha = (1.0f - scale) * 0.25f;
		scale = 1.0f - (scale * 0.5f);
	}

	int wide, tall;
	GetSize(wide, tall);

	int cx = wide / 2;
	int cy = tall / 2;

	//----------------------------------------
	// COLOR
	//----------------------------------------
	Color col = GetFgColor();
	col[3] = alpha * 255;
	surface()->DrawSetColor(col);

	//----------------------------------------
	// PULSING RINGS
	//----------------------------------------
	float pulse = sin(gpGlobals->curtime * 3.0f) * 3.0f;

	surface()->DrawOutlinedCircle(cx, cy, (m_flCircle1Radius + pulse) * scale, 64);
	surface()->DrawOutlinedCircle(cx, cy, (m_flCircle2Radius - pulse) * scale, 64);

	// subtle glow layer
	surface()->DrawSetColor(255, 255, 255, alpha * 30);
	surface()->DrawOutlinedCircle(cx, cy, (m_flCircle1Radius + pulse + 2) * scale, 64);
	surface()->DrawOutlinedCircle(cx, cy, (m_flCircle2Radius - pulse + 2) * scale, 64);

	//----------------------------------------
	// DASHED LINES
	//----------------------------------------
	int dashCount = 1;
	int ypos = (tall - m_flDashHeight) / 2;

	int xpos = (int)((cx)+(m_flDashGap * dashCount * scale));

	while (xpos < wide && xpos > 0)
	{
		surface()->DrawFilledRect(xpos, ypos, xpos + 2, ypos + m_flDashHeight);
		surface()->DrawFilledRect(wide - xpos, ypos, wide - xpos + 2, ypos + m_flDashHeight);

		dashCount++;
		xpos = (int)((cx)+(m_flDashGap * dashCount * max(scale, 0.1f)));
	}

	//----------------------------------------
	// MATERIAL SAFETY
	//----------------------------------------
	if (!m_ZoomMaterial.IsValid())
	{
		m_bPainted = true;
		return;
	}

	//----------------------------------------
	// VIGNETTE (IMPROVED)
	//----------------------------------------
	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->Bind(m_ZoomMaterial);

	IMesh* pMesh = pRenderContext->GetDynamicMesh(true, NULL, NULL, NULL);

	float x0 = 0.0f, x1 = wide / 2.0f, x2 = wide;
	float y0 = 0.0f, y1 = tall / 2.0f, y2 = tall;

	float uv1 = 1.0f - (1.0f / 255.0f);
	float uv2 = 0.0f + (1.0f / 255.0f);

	struct coord_t { float x, y, u, v; };

	coord_t coords[16] =
	{
		{ x0,y0,uv1,uv2 }, { x1,y0,uv2,uv2 }, { x1,y1,uv2,uv1 }, { x0,y1,uv1,uv1 },
		{ x1,y0,uv2,uv2 }, { x2,y0,uv1,uv2 }, { x2,y1,uv1,uv1 }, { x1,y1,uv2,uv1 },
		{ x1,y1,uv2,uv1 }, { x2,y1,uv1,uv1 }, { x2,y2,uv1,uv2 }, { x1,y2,uv2,uv2 },
		{ x0,y1,uv1,uv1 }, { x1,y1,uv2,uv1 }, { x1,y2,uv2,uv2 }, { x0,y2,uv1,uv2 }
	};

	CMeshBuilder meshBuilder;
	meshBuilder.Begin(pMesh, MATERIAL_QUADS, 4);

	float vignetteAlpha = alpha * 0.6f;

	for (int i = 0; i < 16; i++)
	{
		meshBuilder.Color4f(0.0f, 0.0f, 0.0f, vignetteAlpha);
		meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
		meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();
	pMesh->Draw();

	m_bPainted = true;
}