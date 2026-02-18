// gabeplus_spawnmenu.cpp
// Client-side spawn menu for GabeMod+
// Source SDK Base 2007 compatible

#include "cbase.h"

#include <vgui/IVGui.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>

#include "filesystem.h"

#include "soundenvelope.h"

#include "ienginevgui.h"

#ifdef CLIENT_DLL
#include "cdll_client_int.h"
extern IVEngineClient* engine;
#endif

using namespace vgui;

// ============================================================
// Generic list tab (used for props / ragdolls / npcs)
// ============================================================

class CSpawnListTab : public Panel
{
	DECLARE_CLASS_SIMPLE(CSpawnListTab, Panel);
public:
	CSpawnListTab(Panel* parent, const char* spawnType)
		: BaseClass(parent, "SpawnListTab")
	{
		Q_strncpy(m_szSpawnType, spawnType, sizeof(m_szSpawnType));

		m_pList = new ListPanel(this, "SpawnList");

		SetKeyBoardInputEnabled(false);

		m_pList->AddColumnHeader(
			0, "name", "Name", 200,
			ListPanel::COLUMN_RESIZEWITHWINDOW
		);

		m_pList->AddColumnHeader(
			1, "value", "Model / Class", 360,
			ListPanel::COLUMN_RESIZEWITHWINDOW
		);

		m_pList->SetMultiselectEnabled(false);
		m_pList->SetEmptyListText("Nothing available.");
		m_pList->SetColumnTextAlignment(0, Label::a_west);
		m_pList->SetColumnTextAlignment(1, Label::a_west);

		m_pSpawn = new Button(this, "Spawn", "Spawn", this, "Spawn");
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		int w, h;
		GetSize(w, h);

		const int pad = 10;
		const int btnH = 26;
		const int btnW = 120;

		m_pList->SetPos(pad, pad);
		m_pList->SetSize(w - pad * 2, h - pad * 3 - btnH);

		m_pSpawn->SetSize(btnW, btnH);
		m_pSpawn->SetPos(w - pad - btnW, h - pad - btnH);
	}

	virtual void OnCommand(const char* cmd)
	{
		if (!Q_stricmp(cmd, "Spawn"))
		{
			Msg("called Spawn command\n");
			SpawnSelected();
			return;
		}

		BaseClass::OnCommand(cmd);
	}

	void AddItem(const char* name, const char* value)
	{
		KeyValues* kv = new KeyValues("item");
		kv->SetString("name", name);
		kv->SetString("value", value);

		m_pList->AddItem(kv, 0, false, false);
		// DO NOT delete kv (ListPanel owns it in 2007)
	}

private:
	void SpawnSelected()
	{
#ifdef CLIENT_DLL
		int itemID = m_pList->GetSelectedItem(0);
		Msg("Selected ID: %d\n", itemID);
		if (itemID == -1)
			return;


		KeyValues* kv = m_pList->GetItem(itemID);
		if (!kv)
			return;

		const char* value = kv->GetString("value", "");
		if (!value[0])
			return;

		Msg("Selected ID: %d\n", itemID);

		// Single unified spawn command
		engine->ClientCmd_Unrestricted(
			VarArgs(
				"gabe_spawn %s \"%s\"\n",
				m_szSpawnType,
				value
			)
		);

		// UI sounds
		surface()->PlaySound("ui/buttonclick.wav");
		surface()->PlaySound("common/bugreporter_succeeded.wav");
#endif
	}

private:
	ListPanel* m_pList;
	Button* m_pSpawn;
	char m_szSpawnType[16];
};

class CMultitoolTab : public Panel
{
	DECLARE_CLASS_SIMPLE(CMultitoolTab, Panel);

public:
	CMultitoolTab(Panel* parent)
		: BaseClass(parent, "MultitoolTab")
	{
		m_pList = new ListPanel(this, "ModeList");

		m_pList->AddColumnHeader(0, "name", "Mode", 260,
			ListPanel::COLUMN_RESIZEWITHWINDOW);

		m_pList->SetMultiselectEnabled(false);
		m_pList->SetEmptyListText("No modes. You shouldn't be seeing this!");

		m_pApply = new Button(this, "Apply", "Apply Mode", this, "Apply");
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		int w, h;
		GetSize(w, h);

		const int pad = 10;
		const int btnH = 26;
		const int btnW = 140;

		m_pList->SetPos(pad, pad);
		m_pList->SetSize(w - pad * 2, h - pad * 3 - btnH);

		m_pApply->SetSize(btnW, btnH);
		m_pApply->SetPos(w - pad - btnW, h - pad - btnH);
	}

	virtual void OnCommand(const char* cmd)
	{
		if (!Q_stricmp(cmd, "Apply"))
		{
#ifdef CLIENT_DLL
			Msg("called Spawn command\n");

			int itemID = m_pList->GetSelectedItem(0);
			Msg("Selected ID: %d\n", itemID);
			if (itemID == -1)
				return;

			KeyValues* kv = m_pList->GetItem(itemID);
			if (!kv)
				return;

			const char* mode = kv->GetString("value", "");
			if (!mode[0])
				return;

			engine->ClientCmd_Unrestricted(
				VarArgs("mtool_mode %s\n", mode)
			);

			surface()->PlaySound("ui/buttonclick.wav");
#endif
			return;
		}

		BaseClass::OnCommand(cmd);
	}

	void AddMode(const char* displayName, const char* modeString)
	{
		KeyValues* kv = new KeyValues("item");
		kv->SetString("name", displayName);
		kv->SetString("value", modeString);
		m_pList->AddItem(kv, 0, false, false);
	}

private:
	ListPanel* m_pList;
	Button* m_pApply;
};


// ============================================================
// Main spawn menu frame
// ============================================================

class CGabePlusSpawnMenu : public Frame
{
	DECLARE_CLASS_SIMPLE(CGabePlusSpawnMenu, Frame);
public:
	CGabePlusSpawnMenu(VPANEL parent)
		: BaseClass(NULL, "GabePlusSpawnMenu")
	{
		SetParent(parent);

		SetTitle("SPAWNMENU", true);
		SetSize(640, 480);
		SetMoveable(true);
		SetSizeable(false);
		SetCloseButtonVisible(true);
		SetKeyBoardInputEnabled(false);
		SetMouseInputEnabled(true);
		SetDeleteSelfOnClose(false);

		m_pTabs = new PropertySheet(this, "Tabs");

		Populate();

		CenterOnScreen();
		InvalidateLayout(true, true);
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		int w, h;
		GetSize(w, h);

		const int pad = 10;
		m_pTabs->SetPos(pad, pad + 28);
		m_pTabs->SetSize(w - pad * 2, h - pad * 2 - 28);
	}

	void PopulateSection(KeyValues* root, const char* sectionName, CSpawnListTab* tab)
	{
		KeyValues* section = root->FindKey(sectionName);
		if (!section)
			return;

		for (KeyValues* sub = section->GetFirstSubKey();
			sub;
			sub = sub->GetNextKey())
		{
			const char* display = sub->GetName();
			const char* value = sub->GetString();

			tab->AddItem(display, value);
		}
	}

	void PopulateMultitool(KeyValues* root)
	{
		KeyValues* section = root->FindKey("Multitool");
		if (!section)
			return;

		for (KeyValues* sub = section->GetFirstSubKey();
			sub;
			sub = sub->GetNextKey())
		{
			const char* display = sub->GetName();
			const char* value = sub->GetString();

			m_pMultitool->AddMode(display, value);
		}
	}


private:
	void Populate()
	{
		KeyValues* kvRoot = new KeyValues("SpawnMenu");

		if (!kvRoot->LoadFromFile(filesystem, "settings/sm_populate.txt", "MOD"))
		{
			Warning("Failed to load settings/sm_populate.txt\n");
			kvRoot->deleteThis();
			return;
		}

		for (KeyValues* section = kvRoot->GetFirstSubKey();
			section;
			section = section->GetNextKey())
		{
			const char* tabName = section->GetName();
			const char* spawnType = section->GetString("_type", "");

			if (!spawnType[0])
				continue;

			KeyValues* entries = section->FindKey("entries");
			if (!entries)
				continue;

			// ----- Multitool Special Case -----
			if (!Q_stricmp(spawnType, "mtool"))
			{
				m_pMultitool = new CMultitoolTab(m_pTabs);
				m_pTabs->AddPage(m_pMultitool, tabName);

				for (KeyValues* sub = entries->GetFirstSubKey();
					sub;
					sub = sub->GetNextKey())
				{
					m_pMultitool->AddMode(sub->GetName(), sub->GetString());
				}

				continue;
			}

			// ----- Normal Spawn Tab -----
			CSpawnListTab* newTab = new CSpawnListTab(m_pTabs, spawnType);
			m_pTabs->AddPage(newTab, tabName);
			m_SpawnTabs.AddToTail(newTab);

			for (KeyValues* sub = entries->GetFirstSubKey();
				sub;
				sub = sub->GetNextKey())
			{
				newTab->AddItem(sub->GetName(), sub->GetString());
			}
		}

		kvRoot->deleteThis();
	}

	void CenterOnScreen()
	{
		int sw, sh;
		surface()->GetScreenSize(sw, sh);

		int w, h;
		GetSize(w, h);

		SetPos((sw - w) / 2, (sh - h) / 2);
	}

private:
	PropertySheet* m_pTabs;

	CMultitoolTab* m_pMultitool; // Tabs for multitool modes

	CUtlVector<CSpawnListTab*> m_SpawnTabs; // Tabs for spawning entities
};

// ============================================================
// Console command
// ============================================================

static CGabePlusSpawnMenu* g_pSpawnMenu = NULL;

CON_COMMAND_F(
	gabeplus_spawnmenu,
	"Open the spawn menu",
	FCVAR_CLIENTDLL
)
{
	VPANEL root = enginevgui->GetPanel(PANEL_GAMEDLL);
	if (!root)
		return;

	if (!g_pSpawnMenu)
	{
		g_pSpawnMenu = new CGabePlusSpawnMenu(root);
	}

	g_pSpawnMenu->SetParent(root);
	g_pSpawnMenu->SetVisible(true);
	g_pSpawnMenu->MakePopup();
	g_pSpawnMenu->MoveToFront();
	g_pSpawnMenu->RequestFocus();
}
