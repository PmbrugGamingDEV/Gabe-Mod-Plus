//========= Copyright Valve Corporation ============//
//
// Purpose: Speedrun Timer HUD (Stylized)
//
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "hudelement.h"
#include "vgui/ilocalize.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include "vgui_controls/Panel.h"

using namespace vgui;

//------------------------------------------------------------
// ConVars
//------------------------------------------------------------
ConVar spdrun_timer("spdrun_timer", "05:00", FCVAR_ARCHIVE, "Speedrun timer (mm:ss)");
ConVar spdrun_endmsg("spdrun_endmsg", "Round Over!", FCVAR_ARCHIVE, "Message when timer ends");

//------------------------------------------------------------
// HUD CLASS
//------------------------------------------------------------
class CHudSpeedrunTimer : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE(CHudSpeedrunTimer, Panel);

public:
	CHudSpeedrunTimer(const char* pElementName);

	virtual void Init();
	virtual void VidInit();
	virtual void OnThink();
	virtual bool ShouldDraw();
	virtual void Paint();

private:
	float m_flEndTime;
	float m_flNextAlarmTime;

	bool  m_bStarted;
	bool  m_bFinished;

	HFont m_hTitleFont;
	HFont m_hTimerFont;

	void StartTimerFromConVar();
	void PlayAlarm();

	void DrawShadowText(HFont font, int x, int y, Color col, const wchar_t* text);
};

DECLARE_HUDELEMENT(CHudSpeedrunTimer);

//------------------------------------------------------------
CHudSpeedrunTimer::CHudSpeedrunTimer(const char* pElementName)
	: CHudElement(pElementName), BaseClass(NULL, "HudSpeedrunTimer")
{
	SetParent(g_pClientMode->GetViewport());

	m_flEndTime = 0.0f;
	m_flNextAlarmTime = 0.0f;

	m_bStarted = false;
	m_bFinished = false;

	int w, h;
	surface()->GetScreenSize(w, h);

	SetBounds(w * 0.05f, h * 0.4f, 320, 160);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

//------------------------------------------------------------
void CHudSpeedrunTimer::Init()
{
	m_bStarted = false;
	m_bFinished = false;
	m_flNextAlarmTime = 0.0f;
}

//------------------------------------------------------------
void CHudSpeedrunTimer::VidInit()
{
	m_bStarted = false;
	m_bFinished = false;
	m_flNextAlarmTime = 0.0f;

	m_hTitleFont = surface()->CreateFont();
	surface()->SetFontGlyphSet(m_hTitleFont, "Tahoma", 26, 600, 0, 0, vgui::ISurface::FONTFLAG_ANTIALIAS);

	m_hTimerFont = surface()->CreateFont();
	surface()->SetFontGlyphSet(m_hTimerFont, "Tahoma", 44, 900, 0, 0, vgui::ISurface::FONTFLAG_ANTIALIAS);

	StartTimerFromConVar();
}

//------------------------------------------------------------
void CHudSpeedrunTimer::StartTimerFromConVar()
{
	int m = 0, s = 0;
	sscanf(spdrun_timer.GetString(), "%d:%d", &m, &s);

	m_flEndTime = gpGlobals->curtime + (m * 60 + s);
	m_bStarted = true;
	m_bFinished = false;
	m_flNextAlarmTime = 0.0f;
}

//------------------------------------------------------------
void CHudSpeedrunTimer::PlayAlarm()
{
	C_BasePlayer* p = C_BasePlayer::GetLocalPlayer();
	if (p)
		p->EmitSound("ambient/alarms/klaxon1.wav");
}

//------------------------------------------------------------
void CHudSpeedrunTimer::OnThink()
{
	BaseClass::OnThink();

	C_BasePlayer* p = C_BasePlayer::GetLocalPlayer();

	if (m_bFinished && p && p->IsAlive())
	{
		StartTimerFromConVar();
		return;
	}

	if (!m_bStarted)
		return;

	float t = m_flEndTime - gpGlobals->curtime;

	if (t <= 30.0f && t > 0.0f)
	{
		if (gpGlobals->curtime >= m_flNextAlarmTime)
		{
			PlayAlarm();
			m_flNextAlarmTime = gpGlobals->curtime + ((t <= 10.0f) ? 0.5f : 1.0f);
		}
	}

	if (t <= 0.0f && !m_bFinished)
	{
		if (p)
		{
			ClientPrint(p, HUD_PRINTCENTER, spdrun_endmsg.GetString());
			engine->ClientCmd("mp_restartgame 1");
		}

		m_bStarted = false;
		m_bFinished = true;
	}
}

//------------------------------------------------------------
bool CHudSpeedrunTimer::ShouldDraw()
{
	const char* val = spdrun_timer.GetString();
	return (val && val[0] && Q_stricmp(val, "0") != 0);
}

//------------------------------------------------------------
// 🔥 SHADOW TEXT HELPER
//------------------------------------------------------------
void CHudSpeedrunTimer::DrawShadowText(HFont font, int x, int y, Color col, const wchar_t* text)
{
	surface()->DrawSetTextFont(font);

	// shadow
	surface()->DrawSetTextColor(0, 0, 0, col.a());
	surface()->DrawSetTextPos(x + 2, y + 2);
	surface()->DrawPrintText(text, wcslen(text));

	// main
	surface()->DrawSetTextColor(col);
	surface()->DrawSetTextPos(x, y);
	surface()->DrawPrintText(text, wcslen(text));
}

//------------------------------------------------------------
void CHudSpeedrunTimer::Paint()
{
	if (!m_bStarted && !m_bFinished)
		return;

	float t = m_flEndTime - gpGlobals->curtime;
	if (t < 0) t = 0;

	int sec = (int)t;
	int min = sec / 60;
	int s = sec % 60;

	char buf[32];
	Q_snprintf(buf, sizeof(buf), "%02d:%02d", min, s);

	//------------------------------------------------------------
	// COLORS + PULSE
	//------------------------------------------------------------
	bool danger = (t <= 30.0f && !m_bFinished);

	float pulse = fabs(sin(gpGlobals->curtime * 6.0f));

	Color bg = danger ? Color(60 + pulse * 80, 0, 0, 180) : Color(20, 20, 20, 160);
	Color outline = danger ? Color(255, 50, 50, 255) : Color(150, 150, 150, 255);

	Color title = Color(255, 180, 255, 255);
	Color time = danger
		? Color(255, 80 + pulse * 100, 80, 255)
		: Color(255, 255, 255, 255);

	//------------------------------------------------------------
	// BACK PANEL
	//------------------------------------------------------------
	surface()->DrawSetColor(bg);
	surface()->DrawFilledRect(0, 0, 300, 140);

	// outline
	surface()->DrawSetColor(outline);
	surface()->DrawOutlinedRect(0, 0, 300, 140);

	//------------------------------------------------------------
	// TEXT
	//------------------------------------------------------------
	wchar_t wTitle[] = L"ROUND TIME";

	wchar_t wTime[32];
	g_pVGuiLocalize->ConvertANSIToUnicode(buf, wTime, sizeof(wTime));

	DrawShadowText(m_hTitleFont, 10, 8, title, wTitle);

	if (m_bFinished)
	{
		wchar_t wMsg[128];
		g_pVGuiLocalize->ConvertANSIToUnicode(spdrun_endmsg.GetString(), wMsg, sizeof(wMsg));

		DrawShadowText(m_hTimerFont, 10, 50, time, wMsg);
	}
	else
	{
		DrawShadowText(m_hTimerFont, 10, 50, time, wTime);
	}
}