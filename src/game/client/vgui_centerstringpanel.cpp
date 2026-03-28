#include "cbase.h"
#include "vguicenterprint.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static ConVar scr_centertime("scr_centertime", "3");
static ConVar scr_centerfade("scr_centerfade", "1");

//=========================================================
// CCenterStringLabel
//=========================================================
CCenterStringLabel::CCenterStringLabel(vgui::VPANEL parent)
	: BaseClass(NULL, "CCenterStringLabel")
{
	SetParent(parent);

	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);

	m_hFont = 0;

	ComputeSize();

	vgui::ivgui()->AddTickSignal(GetVPanel(), 16);
}

CCenterStringLabel::~CCenterStringLabel()
{
}

void CCenterStringLabel::OnScreenSizeChanged(int oldwide, int oldtall)
{
	BaseClass::OnScreenSizeChanged(oldwide, oldtall);
	ComputeSize();
}

void CCenterStringLabel::ComputeSize(void)
{
	int w = ScreenWidth();
	int h = ScreenHeight();

	SetSize(w, h);
	SetPos(0, 0);
}

void CCenterStringLabel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_hFont = pScheme->GetFont("Trebuchet24");

	if (!m_hFont)
		m_hFont = pScheme->GetFont("Default");
}

//=========================================================
// MESSAGE SYSTEM
//=========================================================
void CCenterStringLabel::AddMsg(const wchar_t* txt, Color col)
{
	if (!txt || !txt[0])
		return;

	CenterMsg_t msg;

	wcsncpy(msg.text, txt, 511);
	msg.text[511] = 0;

	msg.color = col;
	msg.start = gpGlobals->curtime;
	msg.end = gpGlobals->curtime + scr_centertime.GetFloat();

	m_Messages.push_back(msg);
}

//=========================================================
// API
//=========================================================
void CCenterStringLabel::SetTextColor(int r, int g, int b, int a)
{
	// no-op (handled per message)
}

void CCenterStringLabel::Print(char* text)
{
	wchar_t w[512];
	g_pVGuiLocalize->ConvertANSIToUnicode(text, w, sizeof(w) / sizeof(wchar_t));
	AddMsg(w, Color(255, 255, 255, 255));
}

void CCenterStringLabel::Print(wchar_t* text)
{
	AddMsg(text, Color(255, 255, 255, 255));
}

void CCenterStringLabel::ColorPrint(int r, int g, int b, int a, char* text)
{
	wchar_t w[512];
	g_pVGuiLocalize->ConvertANSIToUnicode(text, w, sizeof(w) / sizeof(wchar_t));
	AddMsg(w, Color(r, g, b, a));
}

void CCenterStringLabel::ColorPrint(int r, int g, int b, int a, wchar_t* text)
{
	AddMsg(text, Color(r, g, b, a));
}

void CCenterStringLabel::Clear(void)
{
	m_Messages.clear();
}

//=========================================================
// UPDATE
//=========================================================
void CCenterStringLabel::OnTick(void)
{
	float cur = gpGlobals->curtime;

	for (int i = (int)m_Messages.size() - 1; i >= 0; --i)
	{
		if (cur > m_Messages[i].end)
			m_Messages.erase(m_Messages.begin() + i);
	}

	SetVisible(!m_Messages.empty());
}

//=========================================================
// DRAW
//=========================================================
void CCenterStringLabel::Paint(void)
{
	if (!m_hFont)
		return;

	int screenW = ScreenWidth();
	int screenH = ScreenHeight();

	int baseY = (int)(screenH * 0.4f);

	for (int i = 0; i < (int)m_Messages.size(); i++)
	{
		CenterMsg_t& msg = m_Messages[i];

		float life = msg.end - msg.start;
		if (life <= 0.0f)
			continue;

		float t = (gpGlobals->curtime - msg.start) / life;

		float alpha = 255.0f;

		if (scr_centerfade.GetBool())
		{
			if (t < 0.1f)
				alpha = t * 10.0f * 255.0f;
			else if (t > 0.8f)
				alpha = (1.0f - t) * 5.0f * 255.0f;
		}

		alpha = clamp(alpha, 0.0f, 255.0f);

		int yOffset = i * 40;

		int textW, textH;
		vgui::surface()->GetTextSize(m_hFont, msg.text, textW, textH);

		int x = (screenW - textW) / 2;
		int y = baseY - yOffset;

		vgui::surface()->DrawSetTextFont(m_hFont);

		// Shadow
		vgui::surface()->DrawSetTextColor(0, 0, 0, (int)(alpha * 0.8f));
		vgui::surface()->DrawSetTextPos(x + 2, y + 2);
		vgui::surface()->DrawPrintText(msg.text, wcslen(msg.text));

		// Glow
		vgui::surface()->DrawSetTextColor(msg.color.r(), msg.color.g(), msg.color.b(), (int)(alpha * 0.25f));
		vgui::surface()->DrawSetTextPos(x - 1, y);
		vgui::surface()->DrawPrintText(msg.text, wcslen(msg.text));
		vgui::surface()->DrawSetTextPos(x + 1, y);
		vgui::surface()->DrawPrintText(msg.text, wcslen(msg.text));

		// Main
		vgui::surface()->DrawSetTextColor(msg.color.r(), msg.color.g(), msg.color.b(), (int)alpha);
		vgui::surface()->DrawSetTextPos(x, y);
		vgui::surface()->DrawPrintText(msg.text, wcslen(msg.text));
	}
}

//=========================================================
// CCenterPrint WRAPPER (UNCHANGED BEHAVIOR)
//=========================================================
CCenterPrint::CCenterPrint(void)
{
	vguiCenterString = NULL;
}

void CCenterPrint::Create(vgui::VPANEL parent)
{
	if (!vguiCenterString)
	{
		vguiCenterString = new CCenterStringLabel(parent);
	}
}

void CCenterPrint::Destroy(void)
{
	if (vguiCenterString)
	{
		vguiCenterString->SetParent((vgui::Panel*)NULL);
		delete vguiCenterString;
		vguiCenterString = NULL;
	}
}

void CCenterPrint::SetTextColor(int r, int g, int b, int a)
{
	if (vguiCenterString)
		vguiCenterString->SetTextColor(r, g, b, a);
}

void CCenterPrint::Print(char* text)
{
	if (vguiCenterString)
		vguiCenterString->Print(text);
}

void CCenterPrint::Print(wchar_t* text)
{
	if (vguiCenterString)
		vguiCenterString->Print(text);
}

void CCenterPrint::ColorPrint(int r, int g, int b, int a, char* text)
{
	if (vguiCenterString)
		vguiCenterString->ColorPrint(r, g, b, a, text);
}

void CCenterPrint::ColorPrint(int r, int g, int b, int a, wchar_t* text)
{
	if (vguiCenterString)
		vguiCenterString->ColorPrint(r, g, b, a, text);
}

void CCenterPrint::Clear(void)
{
	if (vguiCenterString)
		vguiCenterString->Clear();
}

//=========================================================
// GLOBAL
//=========================================================
static CCenterPrint g_CenterPrint;
CCenterPrint* internalCenterPrint = &g_CenterPrint;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(
	CCenterPrint,
	ICenterPrint,
	VCENTERPRINT_INTERFACE_VERSION,
	g_CenterPrint);