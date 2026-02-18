//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Friends list panel for Gabe Mod Plus
//          Displays Steam friends and basic status information.
//
//=============================================================================

#include "cbase.h"

// VGUI core
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>

// VGUI controls
#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>

#include "ienginevgui.h"

// Steam (Orange Box)
#include "steam/steam_api.h"
#include "steam/isteamfriends.h"

using namespace vgui;

// ------------------------------------------------------------
// Friend Info Panel
// ------------------------------------------------------------
class CGabePlusFriendInfoPanel : public Frame
{
	DECLARE_CLASS_SIMPLE(CGabePlusFriendInfoPanel, Frame);

public:
	CGabePlusFriendInfoPanel(VPANEL parent, KeyValues* kv)
		: BaseClass(NULL, "GabePlusFriendInfoPanel")
	{
		SetParent(parent);

		SetTitle("Friend Info", true);
		SetSize(320, 170);
		SetMoveable(true);
		SetSizeable(false);
		SetCloseButtonVisible(true);
		SetDeleteSelfOnClose(true);

		const char* name = kv ? kv->GetString("friend", "Unknown") : "Unknown";
		const char* ingame = kv ? kv->GetString("ingame", "No") : "No";

		m_pName = new Label(this, "Name", VarArgs("Friend: %s", name));
		m_pState = new Label(this, "State", VarArgs("In Game: %s", ingame));

		CenterOnScreen();
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		m_pName->SetPos(16, 50);
		m_pName->SizeToContents();

		m_pState->SetPos(16, 80);
		m_pState->SizeToContents();
	}

private:
	void CenterOnScreen()
	{
		int sw, sh;
		surface()->GetScreenSize(sw, sh);

		int w, h;
		GetSize(w, h);

		SetPos((sw - w) / 2, (sh - h) / 2);
	}

private:
	Label* m_pName;
	Label* m_pState;
};

// ------------------------------------------------------------
// Friends Panel
// ------------------------------------------------------------
class CGabePlusFriendsPanel : public Frame
{
	DECLARE_CLASS_SIMPLE(CGabePlusFriendsPanel, Frame);

public:
	CGabePlusFriendsPanel(VPANEL parent)
		: BaseClass(NULL, "GabePlusFriendsPanel")
	{
		SetParent(parent);

		SetTitle("Friends", true);
		SetSize(360, 420);
		SetMoveable(true);
		SetSizeable(false);
		SetCloseButtonVisible(true);
		SetDeleteSelfOnClose(false);

		// Friends list
		m_pList = new ListPanel(this, "FriendsList");

		m_pList->AddColumnHeader(
			0, "friend", "Friend", 220,
			ListPanel::COLUMN_RESIZEWITHWINDOW
		);

		m_pList->AddColumnHeader(
			1, "ingame", "In Game", 80,
			ListPanel::COLUMN_FIXEDSIZE
		);

		m_pList->SetMultiselectEnabled(false);
		m_pList->SetEmptyListText("Steam not available.");

		// Bottom buttons
		m_pSteamFriends = new Button(this, "Steam", "Steam Friends", this, "Friend_OpenSteamFriends");
		m_pRefresh = new Button(this, "Refresh", "Refresh", this, "Friend_Refresh");

		PopulateFriends();
		CenterOnScreen();
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		int w, h;
		GetSize(w, h);

		const int pad = 8;
		const int btnH = 24;
		const int btnW = 96;
		const int gap = 6;

		int y = h - pad - btnH;

		// Buttons
		m_pSteamFriends->SetSize(btnW + 24, btnH);
		m_pRefresh->SetSize(btnW, btnH);

		m_pSteamFriends->SetPos(pad + btnW + gap, y);
		m_pRefresh->SetPos(w - pad - btnW, y);

		// List panel
		m_pList->SetPos(pad, pad + 28);
		m_pList->SetSize(
			w - pad * 2,
			(y - (pad + 28)) - pad
		);
	}

	virtual void OnThink()
	{
		BaseClass::OnThink();

		bool bHasSel = (m_pList->GetSelectedItem(0) != -1);
	}

	virtual void OnCommand(const char* cmd)
	{
		if (!Q_stricmp(cmd, "Friend_ViewInfo"))
		{
			KeyValues* kv = GetSelectedFriendKV();
			if (kv)
				OpenFriendInfoPanel(kv);
			return;
		}

		if (!Q_stricmp(cmd, "Friend_OpenSteamFriends"))
		{
			if (steamapicontext->SteamFriends())
				steamapicontext->SteamFriends()->ActivateGameOverlay("Friends");
			return;
		}

		if (!Q_stricmp(cmd, "Friend_Refresh"))
		{
			PopulateFriends();
			return;
		}

		BaseClass::OnCommand(cmd);
	}

private:
	void PopulateFriends()
	{
		m_pList->RemoveAll();

		if (!steamapicontext->SteamFriends())
			return;

		ISteamFriends* pFriends = steamapicontext->SteamFriends();
		int count = pFriends->GetFriendCount(k_EFriendFlagImmediate);

		for (int i = 0; i < count; ++i)
		{
			CSteamID id = pFriends->GetFriendByIndex(i, k_EFriendFlagImmediate);

			const char* name = pFriends->GetFriendPersonaName(id);
			EPersonaState state = pFriends->GetFriendPersonaState(id);

			bool bOnline =
				(state == k_EPersonaStateOnline ||
					state == k_EPersonaStateAway ||
					state == k_EPersonaStateBusy ||
					state == k_EPersonaStateSnooze);

			KeyValues* kv = new KeyValues("item");
			kv->SetString("friend", name);
			kv->SetString("ingame", bOnline ? "Yes" : "No");

			m_pList->AddItem(kv, 0, false, false);
		}
	}

	KeyValues* GetSelectedFriendKV()
	{
		int itemID = m_pList->GetSelectedItem(0);
		if (itemID == -1)
			return NULL;

		return m_pList->GetItem(itemID);
	}

	void OpenFriendInfoPanel(KeyValues* kv)
	{
		VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
		if (!root)
			return;

		new CGabePlusFriendInfoPanel(root, kv);
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
	ListPanel* m_pList;
	Button* m_pSteamFriends;
	Button* m_pRefresh;
};

// ------------------------------------------------------------
// Console command
// ------------------------------------------------------------
static CGabePlusFriendsPanel* g_pFriends = NULL;

CON_COMMAND_F(
	gabeplus_friends,
	"Open friends list",
	FCVAR_CLIENTDLL
)
{
	VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	if (!root)
		return;

	if (!g_pFriends)
		g_pFriends = new CGabePlusFriendsPanel(root);

	g_pFriends->SetVisible(true);
	g_pFriends->MakePopup();
	g_pFriends->MoveToFront();
	g_pFriends->RequestFocus();
}
