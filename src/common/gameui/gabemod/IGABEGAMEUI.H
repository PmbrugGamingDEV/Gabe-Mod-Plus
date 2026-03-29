#pragma once
#include "interface.h"
#include "vgui/IPanel.h"
#include <vgui/VGUI.h>

class IGabeGameUI
{
public:
    virtual void SetMainMenuOverride(vgui::VPANEL panel) = 0;
    virtual int ShowWindowsMessageBox( const char* title, const char* message, unsigned int flags ) = 0;
};

#define GABEGAMEUI_INTERFACE_VERSION "GabeGameUI002" // Interface version for gabe mod to use when connecting to GameUI.dll