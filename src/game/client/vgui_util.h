#pragma once

//============ Public Domain for Source SDK =========//
//
// VGUI Tools
//
//================================//

#include "cbase.h"
#include <vgui/VGUI.h>
#include <vgui/ISurface.h>
#include "ienginevgui.h"
#include "vgui_all.h"

using namespace vgui;

// Parent viewports : Very simple

#define PanelType_GameUIPanel PANEL_GAMEUIDLL
#define PanelType_GameScreen PANEL_INGAMESCREENS
#define PanelType_ClientDLL PANEL_CLIENTDLL
#define PanelType_GameDLL PANEL_GAMEDLL
#define PanelType_Tools PANEL_TOOLS
#define PanelType_ClientTools PANEL_CLIENTDLL_TOOLS
#define PanelType_ConnectedToRoot PANEL_ROOT

// macros

#define PANEL_CREATE(className, baseClass, Title, w, h, FUNCS, MEMBERS) \
class className : public baseClass \
{ \
public: \
    DECLARE_CLASS_SIMPLE(className, baseClass); \
\
    className(vgui::Panel* parent, const char* name = #className) \
        : baseClass(parent, name) \
    { \
		SetTitle(Title, true);	 \
		SetMoveable(true);	\
		SetSizeable(true);			\
		SetCloseButtonVisible(true);	\
		SetDeleteSelfOnClose(false);	\
        SetSize(w, h); \
        SetVisible(true); \
        SetMouseInputEnabled(true); \
        SetKeyBoardInputEnabled(true); \
    } \
	   \
	FUNCS \
 \
private: \
	MEMBERS \
};

#define PANELORHUDCMD(className, parentType, CmdName, CmdDesc) \
static className* g_p##className = NULL; \
CON_COMMAND(CmdName, CmdDesc) \
{ \
    vgui::VPANEL root = enginevgui->GetPanel(parentType); \
    if (!root) \
        return; \
\
    vgui::Panel* parent = vgui::ipanel()->GetPanel(root, "ClientDLL"); \
    if (!parent) \
        parent = vgui::ipanel()->GetPanel(root, NULL); \
\
    if (!g_p##className) \
    { \
        if (parent) \
            g_p##className = new className(parent); \
        else \
            g_p##className = new className(NULL); \
    } \
\
    if (g_p##className) \
    { \
        g_p##className->SetVisible(true); \
        g_p##className->MakePopup(); \
        g_p##className->MoveToFront(); \
        g_p##className->RequestFocus(); \
    } \
}

#define HUDELEMENT_CREATE(className, hiddenbits, borderenable, paintbgndeble, vis, borderpnt) \
class className : public CHudElement, public vgui::Panel \
{ \
public: \
    DECLARE_CLASS_SIMPLE(className, vgui::Panel); \
\
    className(const char* name) \
        : CHudElement(name), vgui::Panel(NULL, name) \
    { \
        SetHiddenBits(hiddenbits); \
        SetPaintBackgroundEnabled(paintbgndeble); \
        SetPaintBorderEnabled(borderpnt); \
        SetVisible(vis); \
    } \
}; \
DECLARE_HUDELEMENT(className);

#define VGUI_FRAME(parent, name, title, x,y,w,h) \
    vgui::Frame* name = VGUI::CreateFrame(parent, title, x,y,w,h)

#define VGUI_LABEL(parent, name, text, x,y,w,h) \
    vgui::Label* name = VGUI::CreateLabel(parent, text, x,y,w,h)

#define VGUI_BUTTON(parent, name, text, cmd, x,y,w,h) \
    vgui::Button* name = VGUI::CreateButton(parent, text, cmd, x,y,w,h)

#define VGUI_TEXTBOX(parent, name, x,y,w,h) \
    vgui::TextEntry* name = VGUI::CreateTextBox(parent, x,y,w,h)

#define VGUI_LIST(parent, name, x,y,w,h) \
    vgui::ListPanel* name = VGUI::CreateList(parent, x,y,w,h)

#define VGUI_TABS(parent, name, x,y,w,h) \
    vgui::PropertySheet* name = VGUI::CreateTabs(parent, x,y,w,h)

#define VGUI_TAB(sheet, name, title) \
    vgui::PropertyPage* name = VGUI::CreateTab(sheet, title)


// ========================================================
// 🧠 CORE UTILITY SYSTEM
// ========================================================

namespace VGUI
{
	// =========================
	// BASE CREATE
	// =========================
	template <class T>
	inline T* Create(Panel* parent, const char* panelName = "")
	{
		return new T(parent, panelName);
	}

	// =========================
	// FRAME
	// =========================
	inline Frame* CreateFrame(Panel* parent, const char* title, int x, int y, int w, int h)
	{
		Frame* f = new Frame(parent, "frame");
		f->SetTitle(title, true);
		f->SetPos(x, y);
		f->SetSize(w, h);
		f->SetVisible(true);
		f->MakePopup();
		return f;
	}

	// =========================
	// LABEL
	// =========================
	inline Label* CreateLabel(Panel* parent, const char* text, int x, int y, int w, int h)
	{
		Label* l = new Label(parent, "", text);
		l->SetPos(x, y);
		l->SetSize(w, h);
		return l;
	}

	// =========================
	// BUTTON
	// =========================
	inline Button* CreateButton(Panel* parent, const char* text, const char* cmd, int x, int y, int w, int h)
	{
		Button* b = new Button(parent, "", text);
		b->SetPos(x, y);
		b->SetSize(w, h);

		if (cmd && cmd[0])
			b->SetCommand(cmd);

		return b;
	}

	// =========================
	// TEXT BOX
	// =========================
	inline TextEntry* CreateTextBox(Panel* parent, int x, int y, int w, int h)
	{
		TextEntry* t = new TextEntry(parent, "");
		t->SetPos(x, y);
		t->SetSize(w, h);
		return t;
	}

	// =========================
	// LIST PANEL
	// =========================
	inline ListPanel* CreateList(Panel* parent, int x, int y, int w, int h)
	{
		ListPanel* list = new ListPanel(parent, "");
		list->SetPos(x, y);
		list->SetSize(w, h);
		return list;
	}

	// =========================
	// TABS
	// =========================
	inline PropertySheet* CreateTabs(Panel* parent, int x, int y, int w, int h)
	{
		PropertySheet* sheet = new PropertySheet(parent, "");
		sheet->SetPos(x, y);
		sheet->SetSize(w, h);
		return sheet;
	}

	inline PropertyPage* CreateTab(PropertySheet* sheet, const char* name)
	{
		PropertyPage* page = new PropertyPage(sheet, name);
		sheet->AddPage(page, name);
		return page;
	}

	// ========================================================
	// 📐 LAYOUT HELPERS
	// ========================================================

	inline void DockFill(Panel* p)
	{
		if (!p || !p->GetParent()) return;

		int w, h;
		p->GetParent()->GetSize(w, h);
		p->SetPos(0, 0);
		p->SetSize(w, h);
	}

	inline void DockTop(Panel* p, int height)
	{
		int w, h;
		p->GetParent()->GetSize(w, h);
		p->SetPos(0, 0);
		p->SetSize(w, height);
	}

	inline void DockBottom(Panel* p, int height)
	{
		int w, h;
		p->GetParent()->GetSize(w, h);
		p->SetPos(0, h - height);
		p->SetSize(w, height);
	}

	inline void Center(Panel* p)
	{
		int sw, sh;
		surface()->GetScreenSize(sw, sh);

		int w, h;
		p->GetSize(w, h);

		p->SetPos((sw - w) / 2, (sh - h) / 2);
	}

	// ========================================================
	// 🎨 DEBUG HELPERS
	// ========================================================

	inline void SetDebugColor(Panel* p, int r, int g, int b, int a = 255)
	{
		p->SetPaintBackgroundEnabled(true);
		p->SetBgColor(Color(r, g, b, a));
	}

	inline void PrintPosSize(Panel* p)
	{
		int x, y, w, h;
		p->GetPos(x, y);
		p->GetSize(w, h);

		Msg("Panel [%s] Pos(%d,%d) Size(%d,%d)\n",
			p->GetName(), x, y, w, h);
	}
}

// ==========================
// 🔥 VGUI UTILS (1–50)
// ==========================

namespace vgui
{

	// 1 - Set panel position (x, y)
	inline void SetPos(vgui::Panel* p, int x, int y) { p->SetPos(x, y); }

	// 2 - Set panel size (width, height)
	inline void SetSize(vgui::Panel* p, int w, int h) { p->SetSize(w, h); }

	// 3 - Set full bounds (position + size)
	inline void SetBounds(vgui::Panel* p, int x, int y, int w, int h) { p->SetBounds(x, y, w, h); }

	// 4 - Show panel
	inline void Show(vgui::Panel* p) { p->SetVisible(true); }

	// 5 - Hide panel
	inline void Hide(vgui::Panel* p) { p->SetVisible(false); }

	// 6 - Toggle visibility on/off
	inline void Toggle(vgui::Panel* p) { p->SetVisible(!p->IsVisible()); }

	// 7 - Enable panel interaction
	inline void Enable(vgui::Panel* p) { p->SetEnabled(true); }

	// 8 - Disable panel interaction
	inline void Disable(vgui::Panel* p) { p->SetEnabled(false); }

	// 9 - Set label text
	inline void SetText(vgui::Label* l, const char* t) { l->SetText(t); }

	// 10 - Set button text
	inline void SetBtnText(vgui::Button* b, const char* t) { b->SetText(t); }

	// 11 - Set button command (runs when clicked)
	inline void SetCmd(vgui::Button* b, const char* c) { b->SetCommand(c); }

	// 12 - Auto-size control (Label/Button) and optionally add padding
	inline void AutoSize(vgui::Panel* p, int padX = 0, int padY = 0)
	{
		// Try Label
		vgui::Label* l = dynamic_cast<vgui::Label*>(p);
		if (l)
		{
			l->SizeToContents();
			int w, h;
			l->GetSize(w, h);
			l->SetSize(w + padX, h + padY);
			return;
		}

		// Try Button
		vgui::Button* b = dynamic_cast<vgui::Button*>(p);
		if (b)
		{
			b->SizeToContents();
			int w, h;
			b->GetSize(w, h);
			b->SetSize(w + padX, h + padY);
			return;
		}

		// Fallback (does nothing for unsupported types)
	}

	// 13 - Center panel on screen
	inline void Center(vgui::Panel* p) {
		int sw, sh; vgui::surface()->GetScreenSize(sw, sh);
		int w, h; p->GetSize(w, h);
		p->SetPos((sw - w) / 2, (sh - h) / 2);
	}

	// 14 - Fill entire parent panel
	inline void Fill(vgui::Panel* p) {
		int w, h; p->GetParent()->GetSize(w, h);
		p->SetBounds(0, 0, w, h);
	}

	// 15 - Dock to top of parent
	inline void Top(vgui::Panel* p, int h) {
		int w, _; p->GetParent()->GetSize(w, _);
		p->SetBounds(0, 0, w, h);
	}

	// 16 - Dock to bottom of parent
	inline void Bottom(vgui::Panel* p, int h) {
		int w, H; p->GetParent()->GetSize(w, H);
		p->SetBounds(0, H - h, w, h);
	}

	// 17 - Dock to left side
	inline void Left(vgui::Panel* p, int w) {
		int _, h; p->GetParent()->GetSize(_, h);
		p->SetBounds(0, 0, w, h);
	}

	// 18 - Dock to right side
	inline void Right(vgui::Panel* p, int w) {
		int W, h; p->GetParent()->GetSize(W, h);
		p->SetBounds(W - w, 0, w, h);
	}

	// 19 - Set background color
	inline void Bg(vgui::Panel* p, int r, int g, int b, int a = 255) {
		p->SetPaintBackgroundEnabled(true);
		p->SetBgColor(Color(r, g, b, a));
	}

	// 20 - Set label text color
	inline void Fg(vgui::Label* l, int r, int g, int b, int a = 255) {
		l->SetFgColor(Color(r, g, b, a));
	}

	// 21 - Move panel vertically by amount
	inline void MoveY(vgui::Panel* p, int dy) {
		int x, y; p->GetPos(x, y); p->SetPos(x, y + dy);
	}

	// 22 - Move panel horizontally by amount
	inline void MoveX(vgui::Panel* p, int dx) {
		int x, y; p->GetPos(x, y); p->SetPos(x + dx, y);
	}

	// 23 - Grow panel size (add width/height)
	inline void Grow(vgui::Panel* p, int dw, int dh) {
		int w, h; p->GetSize(w, h); p->SetSize(w + dw, h + dh);
	}

	// 24 - Set only width
	inline void SetWide(vgui::Panel* p, int w) {
		int _, h; p->GetSize(_, h); p->SetSize(w, h);
	}

	// 25 - Set only height
	inline void SetTall(vgui::Panel* p, int h) {
		int w, _; p->GetSize(w, _); p->SetSize(w, h);
	}

	// 26 - Set panel transparency (alpha)
	inline void Alpha(vgui::Panel* p, int a) { p->SetAlpha(a); }

	// 27 - Enable/disable panel border drawing
	inline void Border(vgui::Panel* p, bool b) { p->SetPaintBorderEnabled(b); }

	// 28 - Enable/disable mouse input
	inline void Mouse(vgui::Panel* p, bool b) { p->SetMouseInputEnabled(b); }

	// 29 - Enable/disable keyboard input
	inline void Keyboard(vgui::Panel* p, bool b) { p->SetKeyBoardInputEnabled(b); }

	// 30 - Force layout update
	inline void Invalidate(vgui::Panel* p) { p->InvalidateLayout(true); }

	// 31 - Force repaint
	inline void Repaint(vgui::Panel* p) { p->Repaint(); }

	// 32 - Set panel name (useful for debugging)
	inline void SetName(vgui::Panel* p, const char* n) { p->SetName(n); }

	// 33 - Add margin inside parent (padding effect)
	inline void DockMargin(vgui::Panel* p, int m) {
		int w, h; p->GetParent()->GetSize(w, h);
		p->SetBounds(m, m, w - 2 * m, h - 2 * m);
	}

	// 34 - Center horizontally (X only)
	inline void CenterX(vgui::Panel* p) {
		int sw, _1; vgui::surface()->GetScreenSize(sw, _1);
		int w, _h; p->GetSize(w, _h);
		int _2, y; p->GetPos(_2, y);
		p->SetPos((sw - w) / 2, y);
	}

	// 35 - Center vertically (Y only)
	inline void CenterY(vgui::Panel* p) {
		int _3, sh; vgui::surface()->GetScreenSize(_3, sh);
		int w, h; p->GetSize(w, h);
		int x, _4; p->GetPos(x, _4);
		p->SetPos(x, (sh - h) / 2);
	}

	// 36 - Place panel below another panel
	inline void StackDown(vgui::Panel* prev, vgui::Panel* next, int spacing = 5) {
		int x, y, w, h;
		prev->GetPos(x, y);
		prev->GetSize(w, h);
		next->SetPos(x, y + h + spacing);
	}

	// 37 - Place panel to the right of another panel
	inline void StackRight(vgui::Panel* prev, vgui::Panel* next, int spacing = 5) {
		int x, y, w, h;
		prev->GetPos(x, y);
		prev->GetSize(w, h);
		next->SetPos(x + w + spacing, y);
	}

	// 38 - Copy position + size from another panel
	inline void CopyBounds(vgui::Panel* from, vgui::Panel* to) {
		int x, y, w, h;
		from->GetBounds(x, y, w, h);
		to->SetBounds(x, y, w, h);
	}

	// 39 - Set minimum window size (Frame only)
	inline void SetMinSize(vgui::Frame* f, int w, int h) { f->SetMinimumSize(w, h); }

	// 40 - Make frame interactive (bring to front, accept input)
	inline void Popup(vgui::Frame* f) { f->MakePopup(); }

	// 41 - Remove frame title bar text
	inline void NoTitle(vgui::Frame* f) { f->SetTitle("", false); }

	// 42 - Disable resizing of frame
	inline void NoResize(vgui::Frame* f) { f->SetSizeable(false); }

	// 43 - Disable dragging of frame
	inline void NoMove(vgui::Frame* f) { f->SetMoveable(false); }

	// 44 - Center align label text
	inline void AlignCenter(vgui::Label* l) { l->SetContentAlignment(vgui::Label::a_center); }

	// 45 - Left align label text
	inline void AlignLeft(vgui::Label* l) { l->SetContentAlignment(vgui::Label::a_west); }

	// 46 - Right align label text
	inline void AlignRight(vgui::Label* l) { l->SetContentAlignment(vgui::Label::a_east); }

	// 47 - Set text entry value
	inline void SetValue(vgui::TextEntry* t, const char* v) { t->SetText(v); }

	// 48 - Get text entry value
	inline void GetValue(vgui::TextEntry* t, char* out, int max) { t->GetText(out, max); }

	// 49 - Add item to combobox
	inline void AddItem(vgui::ComboBox* c, const char* txt) { c->AddItem(txt, NULL); }

	// 50 - Clear all items from list panel
	inline void Clear(vgui::ListPanel* l) { l->RemoveAll(); }

}
