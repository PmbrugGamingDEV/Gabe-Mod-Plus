// This panel has no code created by artificial intelligence, it is entirely created by human developers.

// Copyright (C) 1999-2026 Valve Corporation. All rights reserved. //

#include "cbase.h"
#include "vgui/ISurface.h"
#include "vgui/ivgui.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/AnalogBar.h"
#include "vgui_controls/Button.h"

#include "ienginevgui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
class CSelfCodedPanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CSelfCodedPanel, vgui::Panel);

	CSelfCodedPanel::CSelfCodedPanel(Panel* parent, const char* panelName, bool showTaskbarIcon = true)
	{
		SetName("SELF-CODED-PANEL");
		SetSize(640, 480);
		pButton = new vgui::Button(this, "TestButtonClick", "Click Me!!!!!!");
		pLabel = new vgui::Label(this, "TestLabel", "I am a label");
	}

	void Paint() override
	{
		BaseClass::Paint();
		vgui::surface()->DrawSetColor(0, 255, 0, 255);
		vgui::surface()->DrawFilledRect(540, 380, 640, 480);
		pButton->SetPos(100, 100);
		pLabel->SetPos(100, 150);
	}


private:
	vgui::Button* pButton;
	vgui::Label* pLabel;
};

static CSelfCodedPanel* g_pSelfCodedPanel = nullptr;

CON_COMMAND(selfcodedpanel_show, "Shows the self-coded panel")
{
	vgui::VPANEL parent = enginevgui->GetPanel(PANEL_CLIENTDLL);
	if (!g_pSelfCodedPanel)
	{
		g_pSelfCodedPanel = new CSelfCodedPanel((vgui::Frame*)vgui::ipanel()->GetPanel(parent, "CLIENTDLL"), "SelfCodedPanel");
		g_pSelfCodedPanel->MakePopup();
		g_pSelfCodedPanel->MoveToFront();
		g_pSelfCodedPanel->MakeReadyForUse();
	}
}



