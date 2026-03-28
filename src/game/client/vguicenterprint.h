//========= GabeMod Advanced CenterPrint (Compatible with Valve) ============//
#if !defined( VGUICENTERPRINT_H )
#define VGUICENTERPRINT_H
#ifdef _WIN32
#pragma once
#endif

#include "ivguicenterprint.h"
#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>
#include <vector>

//-----------------------------------------------------------------------------
// Forward declare
//-----------------------------------------------------------------------------
namespace vgui
{
	class Panel;
}

//-----------------------------------------------------------------------------
// Message struct
//-----------------------------------------------------------------------------
struct CenterMsg_t
{
	wchar_t text[512];
	Color color;
	float start;
	float end;
};

//-----------------------------------------------------------------------------
// Advanced label
//-----------------------------------------------------------------------------
class CCenterStringLabel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CCenterStringLabel, vgui::Panel);

public:
	CCenterStringLabel(vgui::VPANEL parent);
	virtual ~CCenterStringLabel();

	// vgui
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnTick(void);
	virtual void Paint(void);

	// API (same as Valve)
	virtual void SetTextColor(int r, int g, int b, int a);
	virtual void Print(char* text);
	virtual void Print(wchar_t* text);
	virtual void ColorPrint(int r, int g, int b, int a, char* text);
	virtual void ColorPrint(int r, int g, int b, int a, wchar_t* text);
	virtual void Clear(void);

protected:
	MESSAGE_FUNC_INT_INT(OnScreenSizeChanged, "OnScreenSizeChanged", oldwide, oldtall);

private:
	void ComputeSize(void);
	void AddMsg(const wchar_t* txt, Color col);

private:
	std::vector<CenterMsg_t> m_Messages;
	vgui::HFont m_hFont;
};

//-----------------------------------------------------------------------------
// Wrapper
//-----------------------------------------------------------------------------
class CCenterPrint : public ICenterPrint
{
private:
	CCenterStringLabel* vguiCenterString;

public:
	CCenterPrint(void);

	virtual void Create(vgui::VPANEL parent);
	virtual void Destroy(void);

	virtual void SetTextColor(int r, int g, int b, int a);
	virtual void Print(char* text);
	virtual void Print(wchar_t* text);
	virtual void ColorPrint(int r, int g, int b, int a, char* text);
	virtual void ColorPrint(int r, int g, int b, int a, wchar_t* text);
	virtual void Clear(void);
};

//-----------------------------------------------------------------------------
extern CCenterPrint* internalCenterPrint;

#endif // VGUICENTERPRINT_H