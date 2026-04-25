#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "vgui_all.h"
#include "convar.h"
#include "utlvector.h"

using namespace vgui;

// 🔹 ConVar (used by server to pass name)
ConVar cl_undo_name("gabe_undoname", "", FCVAR_CLIENTDLL, "Auto set by undo, don't touch");

// 🔹 Line struct
struct UndoLine
{
	wchar_t text[128];
	float timeAdded;
};

// 🔹 HUD class
class CHudUndo : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE(CHudUndo, Panel);

public:
	CHudUndo(const char* name);

	void Init()
	{
		m_Lines.Purge();
	}

	void Reset()
	{
		m_Lines.Purge();
	}

	void ShowUndo()
	{
		const char* name = cl_undo_name.GetString();

		char buffer[128];
		Q_snprintf(buffer, sizeof(buffer), "Undone: %s", name);

		UndoLine line;
		g_pVGuiLocalize->ConvertANSIToUnicode(buffer, line.text, sizeof(line.text));
		line.timeAdded = gpGlobals->curtime;

		m_Lines.AddToHead(line);

		// limit stack size
		if (m_Lines.Count() > 6)
			m_Lines.Remove(m_Lines.Count() - 1);
	}

	virtual void Paint()
	{
		if (m_Lines.Count() == 0)
			return;

		int screenW, screenH;
		surface()->GetScreenSize(screenW, screenH);

		HFont font = scheme()->GetIScheme(GetScheme())->GetFont("DefaultVerySmall", true);

		int baseY = screenH * 0.4f;
		int spacing = 26;

		for (int i = m_Lines.Count() - 1; i >= 0; i--)
		{
			float age = gpGlobals->curtime - m_Lines[i].timeAdded;

			// remove expired
			if (age > 3.0f)
			{
				m_Lines.Remove(i);
				continue;
			}

			int drawIndex = (m_Lines.Count() - 1) - i;

			// =========================
			// ANIMATION
			// =========================

			int alpha = RemapValClamped(age, 0.0f, 3.0f, 255.0f, 0.0f);
			float slide = RemapValClamped(age, 0.0f, 0.25f, -120.0f, 0.0f);
			float yOffset = RemapValClamped(age, 0.0f, 0.2f, -10.0f, 0.0f);

			const wchar_t* text = m_Lines[i].text;
			const wchar_t* symbol = L"x"; // safe ASCII

			int symW, symH;
			surface()->GetTextSize(font, symbol, symW, symH);

			int textW, textH;
			surface()->GetTextSize(font, text, textW, textH);

			int totalW = symW + textW + 128;

			// =========================
			// POSITION (middle-left)
			// =========================

			int x = 60 + slide;
			int y = baseY + (drawIndex * spacing) + yOffset;

			int pad = 8;

			// =========================
			// BACKGROUND
			// =========================

			surface()->DrawSetColor(0, 0, 0, alpha / 2);
			surface()->DrawFilledRect(
				x - pad,
				y - 3,
				x + totalW + pad,
				y + textH + 3
			);

			// red strip
			surface()->DrawSetColor(255, 60, 60, alpha);
			surface()->DrawFilledRect(
				x - pad,
				y - 3,
				x - pad + 3,
				y + textH + 3
			);

			// =========================
			// SYMBOL
			// =========================

			surface()->DrawSetTextColor(255, 255, 255, alpha);
			surface()->DrawSetTextFont(font);
			surface()->DrawSetTextPos(x, y);
			surface()->DrawPrintText(symbol, wcslen(symbol));

			x += symW + 5;

			// =========================
			// TEXT
			// =========================

			surface()->DrawSetTextPos(x, y);
			surface()->DrawPrintText(text, wcslen(text));
		}
	}

private:
	CUtlVector<UndoLine> m_Lines;
};

// 🔹 Register HUD
DECLARE_HUDELEMENT(CHudUndo);

// 🔹 Global pointer
static CHudUndo* g_pHudUndo = NULL;

// 🔹 Constructor
CHudUndo::CHudUndo(const char* name) : CHudElement(name), Panel(NULL, "HudUndo")
{
	g_pHudUndo = this;

	SetParent(g_pClientMode->GetViewport());
	SetPaintBackgroundEnabled(false);
	SetBounds(0, 0, ScreenWidth(), ScreenHeight());
}

// 🔹 Console command
CON_COMMAND(show_undo, "Show undo notification")
{
	if (g_pHudUndo)
	{
		g_pHudUndo->ShowUndo();
	}
}