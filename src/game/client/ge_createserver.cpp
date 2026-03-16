#include "cbase.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>

#include "filesystem.h"
#include "ienginevgui.h"

using namespace vgui;

class CGECreateServer : public Frame
{
	DECLARE_CLASS_SIMPLE(CGECreateServer, Frame);

public:

	CGECreateServer(VPANEL parent);

	virtual void SetVisible(bool state);

protected:

	virtual void OnCommand(const char* command);
	void PopulateMaps();

private:

	ComboBox* m_MapList;
	Button* m_StartButton;

	bool m_bLoaded;
};


static CGECreateServer* g_CreateServer = NULL;



CGECreateServer::CGECreateServer(VPANEL parent) : BaseClass(NULL, "CreateServer")
{
	SetParent(parent);

	SetSize(400, 250);
	SetTitle("Create Server", true);

	MoveToCenterOfScreen();

	SetSizeable(false);
	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(true);

	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);

	m_MapList = new ComboBox(this, "MapList", 10, false);
	m_MapList->SetPos(40, 60);
	m_MapList->SetSize(300, 24);

	m_StartButton = new Button(this, "StartButton", "Start Game");
	m_StartButton->SetPos(150, 170);
	m_StartButton->SetCommand("play");

	m_bLoaded = false;
}



void CGECreateServer::SetVisible(bool state)
{
	BaseClass::SetVisible(state);

	if (state)
	{
		if (!m_bLoaded)
		{
			PopulateMaps();
			m_bLoaded = true;
		}

		MoveToCenterOfScreen();
		RequestFocus();
	}
}



void CGECreateServer::PopulateMaps()
{
	m_MapList->DeleteAllItems();

	FileFindHandle_t findHandle;
	char mapname[64];

	const char* filename = filesystem->FindFirstEx("maps\\*.bsp", "MOD", &findHandle);

	while (filename)
	{
		Q_FileBase(filename, mapname, sizeof(mapname));

		m_MapList->AddItem(mapname, new KeyValues(mapname));

		filename = filesystem->FindNext(findHandle);
	}

	filesystem->FindClose(findHandle);

	m_MapList->SetNumberOfEditLines(10);
	m_MapList->SetEditable(false);

	if (m_MapList->GetItemCount() > 0)
		m_MapList->ActivateItemByRow(0);
}



void CGECreateServer::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "play"))
	{
		KeyValues* data = m_MapList->GetActiveItemUserData();

		if (data)
		{
			char cmd[128];

			Q_snprintf(cmd, sizeof(cmd), "map %s", data->GetName());

			engine->ClientCmd_Unrestricted(cmd);
		}

		SetVisible(false);
		return;
	}

	BaseClass::OnCommand(command);
}



CON_COMMAND(showcreateserver, "Open the Create Server panel")
{
	if (!g_CreateServer)
	{
		g_CreateServer = new CGECreateServer(enginevgui->GetPanel(PANEL_GAMEUIDLL));
	}

	g_CreateServer->SetVisible(!g_CreateServer->IsVisible());
}