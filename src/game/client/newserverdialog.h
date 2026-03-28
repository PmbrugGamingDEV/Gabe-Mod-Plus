#ifndef NEWSERVERDIALOG_H
#define NEWSERVERDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/PropertyDialog.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/PanelListPanel.h"

#include "KeyValues.h"

// gameplay system
class CDescription;
struct mpcontrol_t;

class CNewServerDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(CNewServerDialog, vgui::PropertyDialog);

public:
	CNewServerDialog(VPANEL* parent);
	~CNewServerDialog();

protected:
	virtual bool OnOK(bool applyOnly);

private:
	// SERVER PAGE (merged)
	void LoadMaps();
	void LoadMapList();
	const char* GetMapName();

	vgui::ComboBox* m_pMapList;
	vgui::CheckButton* m_pEnableBotsCheck;
	char m_szMapName[64];

	// GAMEPLAY PAGE (merged)
	void LoadGameOptionsList();
	void GatherCurrentValues();
	const char* GetValue(const char* cvar, const char* def);

	CDescription* m_pDescription;
	mpcontrol_t* m_pList;
	vgui::PanelListPanel* m_pOptionsList;

	// COMMON
	KeyValues* m_pSavedData;
};

#endif