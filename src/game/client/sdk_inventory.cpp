#include "cbase.h"
#include "sdk_inventory.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/SectionedListPanel.h>
#include <vgui_controls/Button.h>
#include "c_baseplayer.h"

//CInventoryPanel class: Tutorial example class
class CInventoryPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CInventoryPanel, vgui::Frame);
	//CInventoryPanel : This Class / vgui::Frame : BaseClass

	CInventoryPanel(vgui::VPANEL parent); // Constructor
	~CInventoryPanel() {};// Destructor

protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	//Other used VGUI control Elements:
	C_BasePlayer* pPlayer; // Self-explanatory. 
	SectionedListPanel* ItemSelection; // Self-explanatory. 
	Button* Drop; // Self-explanatory. 
	bool Updated; // Self-explanatory. 
};

// Constuctor: Initializes the Panel
CInventoryPanel::CInventoryPanel(vgui::VPANEL parent) : BaseClass(NULL, "InventoryPanel")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(true);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetVisible(true);
	ItemSelection = new SectionedListPanel(this, "ItemSelection"); // Self-explanatory. 
	Drop = new Button(this, "Drop", "Drop"); // Self-explanatory. 
	Drop->SetCommand("dropitem");// Sets the command of our drop button to "dropitem".
	ItemSelection->AddSection(0, ""); // Adds a section to our SectionedListPanel.
	ItemSelection->AddColumnToSection(0, "Name", "Name", 0, 300); // The "Name" header.

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/InventoryPanel.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	DevMsg("InventoryPanel has been constructed\n");
}

//Class: CInventoryPanelInterface Class. Used for construction.
class CInventoryPanelInterface : public IInventoryPanel
{
private:
	CInventoryPanel* InventoryPanel;
public:
	CInventoryPanelInterface()
	{
		InventoryPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		InventoryPanel = new CInventoryPanel(parent);
	}
	void Destroy()
	{
		if (InventoryPanel)
		{
			InventoryPanel->SetParent((vgui::Panel*)NULL);
			delete InventoryPanel;
		}
	}
};

static CInventoryPanelInterface g_InventoryPanel;
IInventoryPanel* InventoryPanel = (IInventoryPanel*)&g_InventoryPanel;

ConVar cl_inventory("cl_showmypanel", "0", FCVAR_CLIENTDLL, "Sets the state of the Inventory <state>");
ConVar cl_UpdateInventory("cl_updateInventory", "0", FCVAR_CLIENTDLL, "Updates the Inventory");

CON_COMMAND(cl_inventoryToggle, "Toggles the Inventory Screen")
{
	cl_inventory.SetValue(!cl_inventory.GetBool());
};

void CInventoryPanel::OnTick()
{
	BaseClass::OnTick(); // Links this function with that one from the baseclass.
	pPlayer = C_BasePlayer::GetLocalPlayer(); // pPlayer becomes to a pointer to the client.
	if (pPlayer) // if the player exists, then...
	{
		SetVisible(cl_inventory.GetBool()); // Sets the inventory invisible/visible if the bool is false/true
		if (cl_UpdateInventory.GetBool()) // If the cl_UpdateInventory bool is true, then...
		{
			ItemSelection->RemoveAll(); // Deletes all elements.
			Updated = false; // Sets the Update bool to false.
			cl_UpdateInventory.SetValue(0); // Sets the cl_UpdateInventory bool to false.
		}
		if (cl_inventory.GetBool() && !Updated) // If the GUI is activated and Update is false, then...
		{
			for (int i = 0; i < MAX_INVENTORY; i++) // Gets every item from the inventory array.
			{
				if (pPlayer->GetInventoryArray(i)) // If there's something in our array, then...
				{
					KeyValues* kv = new KeyValues("data"); // Creates a KeyValue.
					kv->SetString("Name", pPlayer->GetEntityVectorElement(pPlayer->GetInventoryArray(i))); // Gets the name, that matches to our inventory array entry.
					ItemSelection->AddItem(0, kv); // Adds the name to the list.
					kv->Clear(); // Clears the KeyValue.
					kv->deleteThis(); // Deletes the KeyValue.
				}
			}
			Updated = true;
		}
	}
	else // Else...
		SetVisible(0); // Our GUI will be invisible until the Player comes avaliable.
}

void CInventoryPanel::OnCommand(const char* cmd)
{
	if (!Q_stricmp(cmd, "turnoff")) // On turnoff,
		cl_inventory.SetValue(0); // closes the inventory panel.
	if (!Q_stricmp(cmd, "dropitem")) // If our button sends dropitem, then...
	{
		char com[12]; // Our buffer.
		if (ItemSelection->GetSelectedItem() == NULL)
		{
			Msg("[Inventory] no valid item to drop!\n");
			return;
		}
		else
		{
			Q_snprintf(com, sizeof(com), "dropitem %i", ItemSelection->GetSelectedItem()); // Prints our command into the buffer.
			engine->ServerCmd(com); // Sends the command.
			cl_UpdateInventory.SetValue(1); // Our GUI wants to do an update now.
		}
	}
}

CON_COMMAND(UpdateInventory, "Updates the inventory")
{
	cl_UpdateInventory.SetValue(1);
}