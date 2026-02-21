#include "cbase.h"
#include "player.h"
#define COMM_MODEL "models/props_junk/watermelon01.mdl" // The Model of our melon.
class CMelon : public CBaseAnimating { // The melons Class
public:
	DECLARE_CLASS(CMelon, CBaseAnimating); // Declares CMelon to CBaseAnimating.
	DECLARE_DATADESC(); // Declares all datadescs.
	CMelon() {

	}
	// Use System starts here//
#ifdef GAME_DLL
// Allow +USE pickup
	int	ObjectCaps(void)
	{
		return (BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS);
	}
#endif
	// Use System Ends here //
	void Spawn(void); // Spawn function
	void Precache(void); // Precache function.
	void OnUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value); // Use function.
private:
	bool CreateVPhysics(); // Physic function.
};

LINK_ENTITY_TO_CLASS(func_melon, CMelon);
BEGIN_DATADESC(CMelon)

DEFINE_USEFUNC(OnUse), // Use function gets linked here.


END_DATADESC()

void CMelon::Precache() {
	PrecacheModel(COMM_MODEL); // The model gets precached.
	BaseClass::Precache(); // Links the precache function to the baseclass.
}

void CMelon::Spawn() {

	Precache(); // Runs the precache function.
	SetUse(&CMelon::OnUse); // Links our onuse function.
	SetModel(COMM_MODEL); // Sets the model.
	SetSolid(SOLID_NONE); // Sets our melon to SOLID_NONE because we use CreateVPhysics().
	CreateVPhysics();
}

void CMelon::OnUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) {
	CBasePlayer* pPlayer = ToBasePlayer(pActivator); // The activation becomes a CSDKPlayer.
	if (pPlayer == NULL) // If he doesn't exists, then...
		return; // Abort the use function.
	pPlayer->InventoryAddItem(1); // Adds the entity with the id 1 (our Melon) into the players inventory.
	Remove(); // Deletes the entity from the world.
}

bool CMelon::CreateVPhysics()
{
	// Create the object in the physics system
	VPhysicsInitNormal(SOLID_BBOX, 0, false); // Inits the VPhysics and makes our entity solid.
	return true;
}

CON_COMMAND(create_melon, "Melons... Yammeee.") // Spawns the melon.
{
	//Same like the InventoryRemoveItem function.
	Vector vecForward;
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
	{
		Warning("Could not deterime calling player!\n");
		return;
	}

	AngleVectors(pPlayer->EyeAngles(), &vecForward);
	CBaseEntity* pEnt = CreateEntityByName("func_melon");
	if (pEnt)
	{
		trace_t tr;
		UTIL_TraceLine(pPlayer->EyePosition(), pPlayer->EyePosition() + (vecForward * 256), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr);
		pEnt->SetAbsOrigin(tr.endpos + Vector(0, 0, 32));
		pEnt->SetAbsAngles(vec3_angle);
		pEnt->Spawn();
	}
}