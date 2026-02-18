#include "cbase.h"

#include <vgui_controls/Frame.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ScrollBar.h>

#include "ienginevgui.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include "vgui_int.h"

#include "tier0/dbg.h"
#include "utlvector.h"
#include "utlstring.h"

using namespace vgui;

//============================================================
// Issue storage
//============================================================
struct GabeIssue_t
{
	SpewType_t type;
	CUtlString text;
};

static CUtlVector<GabeIssue_t> g_GabeIssues;

//============================================================
// Spew hook
//============================================================
static bool g_bIssueHookInstalled = false;

SpewRetval_t GabeIssueSpewFunc(SpewType_t type, const tchar* pMsg)
{
	if (type == SPEW_WARNING || type == SPEW_ERROR)
	{
		GabeIssue_t issue;
		issue.type = type;
		issue.text = pMsg;

		g_GabeIssues.AddToTail(issue);
	}

	return SPEW_CONTINUE;
}

void InstallGabeIssueHook()
{
	if (g_bIssueHookInstalled)
		return;

	SpewOutputFunc(GabeIssueSpewFunc);
	g_bIssueHookInstalled = true;
}

//============================================================
// Issues panel
//============================================================
class CGabeIssuesPanel;
static CGabeIssuesPanel* g_pIssuesPanel = NULL;

class CGabeIssuesPanel : public Frame
{
	DECLARE_CLASS_SIMPLE(CGabeIssuesPanel, Frame);

public:
	CGabeIssuesPanel(VPANEL parent) : BaseClass(NULL, "GabeIssuesPanel")
	{
		SetParent(parent);
		SetTitle("GABE MOD – Issues", true);

		SetMoveable(true);
		SetSizeable(false);
		SetCloseButtonVisible(true);
		SetDeleteSelfOnClose(true);
		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);

		int sw, sh;
		surface()->GetScreenSize(sw, sh);

		int w = 720;
		int h = 520;

		SetSize(w, h);
		SetPos((sw - w) / 2, (sh - h) / 2);

		m_pScroll = new ScrollBar(this, "IssueScroll", true);
		m_pScroll->SetPos(w - 18, 35);
		m_pScroll->SetSize(16, h - 45);

		m_pList = new Panel(this, "IssueList");
		m_pList->SetPos(10, 35);
		m_pList->SetSize(w - 40, h - 45);

		PopulateIssues();
		UpdateScrollBar();

		MakePopup();
		Activate();
		RequestFocus();
	}

	virtual void OnClose()
	{
		BaseClass::OnClose();
		g_pIssuesPanel = NULL;
	}

	virtual void OnKeyCodePressed(KeyCode code)
	{
		if (code == KEY_ESCAPE)
		{
			Close();
			return;
		}

		BaseClass::OnKeyCodePressed(code);
	}

	virtual void OnThink()
	{
		BaseClass::OnThink();

		int scroll = m_pScroll->GetValue();
		m_pList->SetPos(10, 35 - scroll);
	}

private:
	void PopulateIssues()
	{
		int y = 0;

		for (int i = 0; i < g_GabeIssues.Count(); i++)
		{
			Color col;

			if (g_GabeIssues[i].type == SPEW_ERROR)
				col = Color(255, 90, 90, 255);
			else
				col = Color(255, 210, 120, 255);

			Label* lbl = new Label(
				m_pList,
				NULL,
				g_GabeIssues[i].text.String()
			);

			lbl->SetPos(0, y);
			lbl->SetWide(m_pList->GetWide());
			lbl->SetWrap(true);
			lbl->SetFgColor(col);
			lbl->SetBgColor(Color(0, 0, 0, 0));

			int wide, tall;
			lbl->GetContentSize(wide, tall);
			lbl->SetTall(tall + 6);

			y += lbl->GetTall() + 4;
		}

		m_iContentTall = y;
	}

	void UpdateScrollBar()
	{
		int viewTall = m_pList->GetTall();

		if (m_iContentTall > viewTall)
		{
			m_pScroll->SetRange(0, m_iContentTall - viewTall);
			m_pScroll->SetRangeWindow(viewTall);
			m_pScroll->SetVisible(true);
		}
		else
		{
			m_pScroll->SetVisible(false);
		}
	}

private:
	Panel* m_pList;
	ScrollBar* m_pScroll;
	int        m_iContentTall;
};

//============================================================
// Show panel
//============================================================
void ShowGabeIssuesPanel()
{
	InstallGabeIssueHook();

	if (g_pIssuesPanel)
		return;

	g_pIssuesPanel = new CGabeIssuesPanel(
		enginevgui->GetPanel(PANEL_GAMEUIDLL));
}

//============================================================
// Console command
//============================================================
CON_COMMAND_F(gabeplus_issues, "Shows Source engine warnings and errors.", FCVAR_CLIENTDLL)
{
	ShowGabeIssuesPanel();
}
