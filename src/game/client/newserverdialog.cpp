#include "cbase.h"
#include "newserverdialog.h"

#include "filesystem.h"
#include "cdll_int.h"
#include "tier1/convar.h"
#include "ienginevgui.h"

#include <vgui_controls/ComboBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PanelListPanel.h>

extern IVEngineClient* engine;
extern ICvar* cvar;
extern IEngineVGui* enginevgui;

using namespace vgui;

//-----------------------------------------------------------------------------
// Simple pages (FIX: PropertyDialog requires PropertyPage)
//-----------------------------------------------------------------------------
class CServerPage : public PropertyPage
{
public:
	CServerPage(Panel* parent) : PropertyPage(parent, "ServerPage")
	{
		m_pMapList = new ComboBox(this, "MapList", 12, false);
		m_pMapList->SetBounds(10, 10, 200, 24);

		LoadMaps();
	}

	ComboBox* GetMapList() { return m_pMapList; }

private:
	void LoadMaps()
	{
		FileFindHandle_t handle;
		const char* file = g_pFullFileSystem->FindFirstEx("maps/*.bsp", "GAME", &handle);

		while (file)
		{
			char map[256];
			Q_strncpy(map, file, sizeof(map));

			char* ext = Q_strstr(map, ".bsp");
			if (ext) *ext = 0;

			m_pMapList->AddItem(map, new KeyValues("data", "mapname", map));

			file = g_pFullFileSystem->FindNext(handle);
		}

		g_pFullFileSystem->FindClose(handle);

		m_pMapList->ActivateItem(0);
	}

	ComboBox* m_pMapList;
};

//-----------------------------------------------------------------------------
class CGamePage : public PropertyPage
{
public:
	CGamePage(Panel* parent) : PropertyPage(parent, "GamePage")
	{
		m_pEnableBots = new CheckButton(this, "Bots", "Enable Bots");
		m_pEnableBots->SetBounds(10, 10, 150, 24);
	}

private:
	CheckButton* m_pEnableBots;
};

//-----------------------------------------------------------------------------
// Dialog
//-----------------------------------------------------------------------------
CNewServerDialog::CNewServerDialog(VPANEL parent)
	: PropertyDialog(NULL, "NewServerDialog")
{
	SetParent(parent);

	SetSize(400, 500);
	SetTitle("Create Server", true);
	SetOKButtonText("Start");

	m_pServerPage = new CServerPage(this);
	m_pGamePage = new CGamePage(this);

	AddPage(m_pServerPage, "Server");
	AddPage(m_pGamePage, "Game");
}

//-----------------------------------------------------------------------------
CNewServerDialog::~CNewServerDialog()
{}

//-----------------------------------------------------------------------------
const char* CNewServerDialog::GetMapName()
{
	KeyValues* kv = m_pServerPage->GetMapList()->GetActiveItemUserData();
	if (!kv) return "d1_trainstation_01";

	return kv->GetString("mapname", "d1_trainstation_01");
}

//-----------------------------------------------------------------------------
bool CNewServerDialog::OnOK(bool applyOnly)
{
	cvar->RevertFlaggedConVars(FCVAR_REPLICATED);
	cvar->RevertFlaggedConVars(FCVAR_CHEAT);

	const char* map = GetMapName();

	char cmd[1024];

	Q_snprintf(cmd, sizeof(cmd),
		"disconnect\nwait\nwait\n"
		"sv_lan 1\n"
		"map %s\n",
		map
	);

	engine->ClientCmd(cmd);

	return true;
}

// ------------------------------------------------------------
// Factory + console command
// ------------------------------------------------------------
static CNewServerDialog* g_pNewServer = NULL;

static void OpenOrToggleNewServerDialog()
{
	VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);

	if (!root)
	{
		Warning("newserverdialog: PANEL_GAMEUIDLL root is NULL\n");
		return;
	}

	if (!g_pNewServer)
	{
		g_pNewServer = new CNewServerDialog(root);
	}

	// ensure parent is valid
	g_pNewServer->SetParent(root);

	// toggle
	bool visible = !g_pNewServer->IsVisible();
	g_pNewServer->SetVisible(visible);

	if (visible)
	{
		g_pNewServer->MakePopup();
		g_pNewServer->MoveToFront();
		g_pNewServer->RequestFocus();
		g_pNewServer->SetZPos(10000);
	}
}

//-----------------------------------------------------------------------------
void CC_NewServerDialog()
{
	OpenOrToggleNewServerDialog();
}

//-----------------------------------------------------------------------------
static ConCommand gabe_newserver(
	"gabe_newserver",
	CC_NewServerDialog,
	"Open Create Server dialog",
	FCVAR_CLIENTDLL
);