#include "cbase.h"
#include "overrideui_rootpanel.h"
#include "ioverrideinterface.h"
#include "igabegameui.h"

#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "ienginevgui.h"
#include <engine/IEngineSound.h>
#include "filesystem.h"

using namespace vgui;

// See interface.h/.cpp for specifics:  basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule 
//  over and over again.
static CDllDemandLoader g_GameUIDLL("GameUI");

OverrideUI_RootPanel* guiroot = NULL;

#include <windows.h> // REQUIRED

void OverrideGameUI()
{
	if (!OverrideUI->GetPanel())
	{
		OverrideUI->Create(NULL);
	}

	if (!guiroot || !guiroot->GetGameUI())
		return;

	static IGabeGameUI* pUI = NULL;
	static CSysModule* pModule = NULL;

	if (!pUI)
	{
		char fullpath[MAX_PATH];

		// Find DLL using Source filesystem
		if (!filesystem->GetLocalPath("bin/Gabe_GameUI.dll", fullpath, sizeof(fullpath)))
		{
			Msg("[GabeUI] Failed to resolve path\n");
			return;
		}

		Msg("[GabeUI] Path: %s\n", fullpath);

		// Load DLL
		pModule = Sys_LoadModule(fullpath);

		if (!pModule)
		{
			Msg("[GabeUI] FAILED to load Gabe_GameUI.dll\n");

			// Debug fallback
			HMODULE test = LoadLibraryA(fullpath);
			if (!test)
			{
				Msg("[GabeUI] LoadLibrary error: %lu\n", GetLastError());
			}
			else
			{
				Msg("[GabeUI] LoadLibrary worked (path issue earlier)\n");
				FreeLibrary(test);
			}

			return;
		}

		Msg("[GabeUI] DLL loaded\n");

		// Get factory
		CreateInterfaceFn factory = Sys_GetFactory(pModule);

		if (!factory)
		{
			Msg("[GabeUI] Failed to get factory\n");
			return;
		}

		// Get interface
		pUI = (IGabeGameUI*)factory(GABEGAMEUI_INTERFACE_VERSION, NULL);

		if (!pUI)
		{
			Msg("[GabeUI] Failed to get IGabeGameUI\n");
			return;
		}

		Msg("[GabeUI] Interface acquired\n");
	}

	// Use it
	pUI->SetMainMenuOverride(guiroot->GetVPanel());

	pUI->ShowWindowsMessageBox(
		"Gabe Mod",
		"This is a message box from the Gabe mod!",
		0
	);

	Msg("[GabeUI] Calls succeeded\n");
}

OverrideUI_RootPanel::OverrideUI_RootPanel(VPANEL parent) : Panel(NULL, "OverrideUIRootPanel")
{
	SetParent(parent);
	guiroot = this;

	m_bCopyFrameBuffer = false;
	gameui = NULL;

	LoadGameUI();

	m_ExitingFrameCount = 0;
}

IGameUI* OverrideUI_RootPanel::GetGameUI()
{
	if (!gameui)
	{
		if (!LoadGameUI())
			return NULL;
	}

	return gameui;
}

IGabeGameUI* OverrideUI_RootPanel::GetGabeGameUI()
{
	if (!gabegameui)
	{
		if (!LoadGabeGameUI())
			return NULL;
	}

	return gabegameui;
}

bool OverrideUI_RootPanel::LoadGabeGameUI()
{
	if (!gabegameui)
	{
		CreateInterfaceFn gabeGameUIFactory = g_GameUIDLL.GetFactory();
		if (gabeGameUIFactory)
		{
			gabegameui = (IGabeGameUI*)gabeGameUIFactory(GABEGAMEUI_INTERFACE_VERSION, NULL);
			if (!gabegameui)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool OverrideUI_RootPanel::LoadGameUI()
{
	if (!gameui)
	{
		CreateInterfaceFn gameUIFactory = g_GameUIDLL.GetFactory();
		if (gameUIFactory)
		{
			gameui = (IGameUI*)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
			if (!gameui)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

OverrideUI_RootPanel::~OverrideUI_RootPanel()
{
	gameui = NULL;
	g_GameUIDLL.Unload();
}

void OverrideUI_RootPanel::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Resize the panel to the screen size
	// Otherwise, it'll just be in a little corner
	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);
	SetSize(wide, tall);
}