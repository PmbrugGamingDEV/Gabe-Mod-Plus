//============= 2026 Gabe Mod =============================================//
//
// Purpose: GameUI exposure for Gabe Mod.
// 
//================================================================//

#include <windows.h>
#include "IGabeGameUI.h"
#include "GameUI_Interface.h"

//-----------------------------------------------------------------------------
// Purpose: Implementation of custom interface
//-----------------------------------------------------------------------------
class CGabeGameUI : public IGabeGameUI
{
public:
    virtual void SetMainMenuOverride(vgui::VPANEL panel)
    {
        GameUI().SetMainMenuOverride(panel);
        Msg("GabeGameUI: Overridden main menu.\n");
    }


    virtual int ShowWindowsMessageBox( const char* title, const char* message, unsigned int flags )
    {
#ifdef _WIN32
        Msg("[GabeGameUI]: Showing Windows message box with title '%s' and message '%s'.\n", title, message);
		MessageBoxA(NULL, message, title, flags); // just do it here
#else
        Msg("[GabeGameUI]: Showing message box is not supported on this platform.\n");
#endif
        return 0; // we don't care about the result
	}

};

//-----------------------------------------------------------------------------
// Global instance
//-----------------------------------------------------------------------------
static CGabeGameUI g_GabeGameUI;

//-----------------------------------------------------------------------------
// Expose to CreateInterface
//-----------------------------------------------------------------------------
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(
    CGabeGameUI,
    IGabeGameUI,
    GABEGAMEUI_INTERFACE_VERSION,
    g_GabeGameUI
);