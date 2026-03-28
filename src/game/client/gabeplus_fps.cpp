//========= Gabe Mod FPS =========//

#include "cbase.h"

#include <vgui_controls/Panel.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IPanel.h>
#include "vguimatsurface/imatsystemsurface.h"

#include "view.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static ConVar gabe_showfps("gabe_showfps", "1", FCVAR_CLIENTDLL);
static ConVar gabe_smoothfps("gabe_smoothfps", "1", FCVAR_CLIENTDLL);

//-----------------------------------------------------------------------------
// PANEL
//-----------------------------------------------------------------------------
class CGabeFPSPanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CGabeFPSPanel, vgui::Panel);

public:
	CGabeFPSPanel(vgui::VPANEL parent);
	virtual ~CGabeFPSPanel();

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void Paint();
	virtual void OnTick();

private:
	void UpdateFPS();
	void GetFPSColor(int fps, Color& col);

private:
	vgui::HFont m_hFont;

	float m_flLastTime;
	float m_flFPS;
	float m_flAverageFPS;
};

//-----------------------------------------------------------------------------
// CONSTRUCTOR
//-----------------------------------------------------------------------------
CGabeFPSPanel::CGabeFPSPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "GabeFPSPanel")
{
	SetParent(parent);

	SetVisible(false);
	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	SetPos(10, 10);
	SetSize(400, 50);

	m_hFont = 0;
	m_flLastTime = 0.0f;
	m_flFPS = 0.0f;
	m_flAverageFPS = -1.0f;

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

CGabeFPSPanel::~CGabeFPSPanel()
{
}

//-----------------------------------------------------------------------------
void CGabeFPSPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_hFont = pScheme->GetFont("Default");
}

//-----------------------------------------------------------------------------
void CGabeFPSPanel::OnTick()
{
	SetVisible(gabe_showfps.GetBool());
	UpdateFPS();
}

//-----------------------------------------------------------------------------
void CGabeFPSPanel::UpdateFPS()
{
	float ft = gpGlobals->absoluteframetime;

	// Protect against bad values
	if (ft <= 0.0f)
		return;

	float instantFPS = 1.0f / ft;

	if (gabe_smoothfps.GetBool())
	{
		float weight = 0.1f;

		if (m_flAverageFPS < 0.0f)
		{
			m_flAverageFPS = instantFPS;
		}
		else
		{
			m_flAverageFPS = m_flAverageFPS * (1.0f - weight) + instantFPS * weight;
		}

		m_flFPS = m_flAverageFPS;
	}
	else
	{
		m_flFPS = instantFPS;
		m_flAverageFPS = -1.0f;
	}
}

//-----------------------------------------------------------------------------
void CGabeFPSPanel::GetFPSColor(int fps, Color& col)
{
	if (fps >= 60)
		col = Color(0, 255, 0, 255);
	else if (fps >= 30)
		col = Color(255, 255, 0, 255);
	else
		col = Color(255, 0, 0, 255);
}

//-----------------------------------------------------------------------------
void CGabeFPSPanel::Paint()
{
	if (!gabe_showfps.GetBool())
		return;

	int fps = (int)m_flFPS;

	Color col;
	GetFPSColor(fps, col);

	float ms = (m_flFPS > 0.0f) ? (1000.0f / m_flFPS) : 0.0f;

	g_pMatSystemSurface->DrawColoredText(
		m_hFont,
		2, 2,
		col.r(), col.g(), col.b(), 255,
		"%3i fps (%.1f ms) on %s",
		fps,
		ms,
		engine->GetLevelName()
	);
}

//-----------------------------------------------------------------------------
// WRAPPER
//-----------------------------------------------------------------------------
class CGabeFPS
{
public:
	CGabeFPS() { m_pPanel = NULL; }

	void Create(vgui::VPANEL parent)
	{
		m_pPanel = new CGabeFPSPanel(parent);
	}

	void Destroy()
	{
		if (m_pPanel)
		{
			m_pPanel->SetParent((vgui::Panel*)NULL);
			delete m_pPanel;
			m_pPanel = NULL;
		}
	}

private:
	CGabeFPSPanel* m_pPanel;
};

//-----------------------------------------------------------------------------
// GLOBAL
//-----------------------------------------------------------------------------
static CGabeFPS g_GabeFPS;

//-----------------------------------------------------------------------------
// SIMPLE ENTRY FUNCTION (NO HEADER NEEDED)
//-----------------------------------------------------------------------------
void GabeFPS_Create(vgui::VPANEL parent)
{
	g_GabeFPS.Create(parent);
}