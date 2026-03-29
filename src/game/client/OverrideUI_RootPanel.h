#include "cbase.h"
#include "vgui_controls/Panel.h"
#include "igabegameui.h"
#include "GameUI/IGameUI.h"

// Overrides GameUI.dll's root panel
class OverrideUI_RootPanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(OverrideUI_RootPanel, vgui::Panel);
public:
	OverrideUI_RootPanel(vgui::VPANEL parent);
	virtual ~OverrideUI_RootPanel();

	IGameUI* GetGameUI();

	IGabeGameUI* GetGabeGameUI();

	bool LoadGabeGameUI();

protected:
	virtual void	ApplySchemeSettings(vgui::IScheme* pScheme);

private:
	bool			LoadGameUI();

	int				m_ExitingFrameCount;
	bool			m_bCopyFrameBuffer;

	IGameUI* gameui;
	IGabeGameUI* gabegameui;
};

extern OverrideUI_RootPanel* guiroot;