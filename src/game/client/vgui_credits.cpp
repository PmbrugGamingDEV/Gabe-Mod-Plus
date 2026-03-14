//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Gabe Mod Plus - Special Thanks Panel (Rainbow)
//          Text-only JBMod-style credits panel
//
//=============================================================================

#include "cbase.h"

// VGUI core
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>

// VGUI controls
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>

#include "gabe_events.h"
#include "tier1/fmtstr.h"

#include "ienginevgui.h"

using namespace vgui;

ConVar gabeplus_thanks_rbow(
	"gabeplus_thanks_rbow",
	"0",
	FCVAR_CLIENTDLL,
	"Whether the thanks panel should cycle through rainbow colors"
);

extern ConVar gabe_forceholiday;

// ------------------------------------------------------------
// Thanks Panel
// ------------------------------------------------------------
class CGabePlusThanksPanel : public Panel
{
	DECLARE_CLASS_SIMPLE(CGabePlusThanksPanel, Panel);

public:
	CGabePlusThanksPanel(VPANEL parent)
		: BaseClass(NULL, "GabePlusThanksPanel")
	{
		SetParent(parent);

		SetPaintBackgroundEnabled(false);
		SetPaintBorderEnabled(false);

		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);

		SetVisible(true);

		// --------------------------------------------------
		// Credits Label
		// --------------------------------------------------

		m_pText = new Label(
			this,
			"ThanksText",
			"Special Thanks to:\n"
			"\n"
			"Garry Newman: Block spawner in gabe_pond\n"
			"PmbrugGaming: Development, AI prompter\n"
			"DasBoSchitt: HAX\n"
			"Adnan Zafar: Rotation for Physics Gun\n"
			"Half-Life 2: Sandbox developers: Updated Physics Gun"
		);

		m_pText->SetContentAlignment(Label::a_northwest);
		m_pText->SetPaintBackgroundEnabled(false);

		// --------------------------------------------------
		// Holiday Label (big)
		// --------------------------------------------------

		m_pHoliday = new Label(this, "HolidayText", "");
		m_pHoliday->SetContentAlignment(Label::a_west);
		m_pHoliday->SetPaintBackgroundEnabled(false);

		SetScheme(scheme()->GetScheme("ClientScheme"));

		InvalidateLayout();
	}

	virtual void ApplySchemeSettings(IScheme* pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);

		HFont small = pScheme->GetFont("DebugFixedSmall", false);
		m_pText->SetFont(small);

			HFont big = pScheme->GetFont("Default", false);

		m_pHoliday->SetFont(big);
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		int sw, sh;
		surface()->GetScreenSize(sw, sh);

		const int width = 500;
		const int height = 190; // increased
		const int pad = 12;

		SetSize(width, height);

		SetPos(
			sw - width - pad,
			sh - height - pad
		);

		const int creditsHeight = 150;

		m_pText->SetPos(0, 0);
		m_pText->SetSize(width, creditsHeight);

		m_pHoliday->SetPos(0, creditsHeight + 6);
		m_pHoliday->SetSize(width, height - creditsHeight - 6);
	}

	virtual void OnThink()
	{
		BaseClass::OnThink();

		Color currentColor(255, 255, 255, 255);

		if (gabeplus_thanks_rbow.GetBool())
		{
			float t = gpGlobals->curtime * 2.0f;

			int r = (int)(sinf(t + 0.0f) * 127.0f + 128.0f);
			int g = (int)(sinf(t + 2.094f) * 127.0f + 128.0f);
			int b = (int)(sinf(t + 4.188f) * 127.0f + 128.0f);

			currentColor = Color(r, g, b, 255);
		}

		m_pText->SetFgColor(currentColor);

		// ------------------------------
		// Holiday Handling
		// ------------------------------

		if (GabeEvents_GetHoliday() != HOLIDAY_NONE)
		{
			m_pHoliday->SetVisible(true);
			m_pHoliday->SetFgColor(currentColor);

			m_pHoliday->SetText(
				CFmtStr("Happy %s!", GabeEvents_GetHolidayName())
			);
		}
		else
		{
			m_pHoliday->SetVisible(true);
			m_pHoliday->SetFgColor(currentColor);

			m_pHoliday->SetText(
				CFmtStr("no holiday, sorry!") 
			);
		}
	}

private:
	Label* m_pText;
	Label* m_pHoliday;
};

// ------------------------------------------------------------
// Console command (toggle)
// ------------------------------------------------------------
static CGabePlusThanksPanel* g_pThanks = NULL;

CON_COMMAND_F(
	gabeplus_thanks,
	"Toggle GabeMod+ rainbow thanks panel",
	FCVAR_CLIENTDLL
)
{
	VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	if (!root)
		return;

	if (!g_pThanks)
	{
		g_pThanks = new CGabePlusThanksPanel(root);
	}
	else
	{
		g_pThanks->SetVisible(!g_pThanks->IsVisible());
	}
}
