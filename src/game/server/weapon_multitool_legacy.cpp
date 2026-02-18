//========= Copyright Weapons Expanded, All rights reserved. ============//
//
// Purpose: Has modes that apply different stuff to objects, for building basically.
//
//=============================================================================

#include "cbase.h"
#include "basecombatweapon.h"
#include "vphysics/constraints.h"
#include "effect_dispatch_data.h"
#include "te_effect_dispatch.h"
#include <in_buttons.h>
#include "fmtstr.h"
#include "ai_basenpc.h"
#include <beam_shared.h>
#include "player.h"
#include "rope.h"

enum ToolMode
{
    MODE_WELD = 0,
    MODE_LIGHTBULBS,
    MODE_DUPLICATOR,
    MODE_BEAMCREATOR,
    MODE_ROPECREATOR,
    MODE_RANDOM,
    MODE_SCALING
};

class CWeaponLegacyMultiTool : public CBaseCombatWeapon
{
public:
    DECLARE_CLASS(CWeaponLegacyMultiTool, CBaseCombatWeapon);
    DECLARE_SERVERCLASS();
    DECLARE_DATADESC();

    CWeaponLegacyMultiTool();

    virtual void PrimaryAttack();
    virtual void SecondaryAttack();
    virtual void ItemPostFrame();
	virtual void Precache();

    void CreateBeam(const Vector& startPoint, const Vector& endPoint);
    void CreateRope(const Vector& startPoint, const Vector& endPoint, CBaseEntity* pStartEntity, CBaseEntity* pEndEntity);
    void AdjustObjectSize(float scaleFactor);
    void CloneEntity(CBaseEntity* pOriginal, const Vector& vSpawnPos);
    void SwitchMode();
    void SizeManipulationMode();
    void TakeScreenshot();
    void WeldEntities(CBaseEntity* pEntity1, CBaseEntity* pEntity2, const Vector& vec1, const Vector& vec2);
    void CreateLightBulb(const Vector& position, CBaseEntity* pTargetEntity);
    void CreateSparkEffect(const Vector& position);
    void RandomChaosMode();

private:
    Vector m_vFirstRopePoint;
    Vector m_vSecondRopePoint;
    CBaseEntity* m_pFirstRopeEntity;
    CBaseEntity* m_pSecondRopeEntity;
    bool m_bHasFirstRopePoint;

    int m_nCurrentBeamType;

    Vector m_vFirstPoint;
    Vector m_vSecondPoint;

    CBaseEntity* m_pClonedEntity;
    Vector m_vCloneOffset;

    int m_nCurrentFOVMode;

    ToolMode m_nCurrentMode;
    CHandle<CBaseEntity> m_hFirstEntity;
    Vector m_vecFirstPoint;
    bool m_bHasFirstPoint;
};

CWeaponLegacyMultiTool::CWeaponLegacyMultiTool()
{
    m_vFirstRopePoint = vec3_origin;
    m_vSecondRopePoint = vec3_origin;

    m_pFirstRopeEntity = NULL;
    m_pSecondRopeEntity = NULL;
    m_bHasFirstRopePoint = false;

    m_nCurrentBeamType = 0;

    m_vFirstPoint = vec3_origin;
    m_vSecondPoint = vec3_origin;

    m_pClonedEntity = NULL;
    m_vCloneOffset.Init(0, 0, 0);

    m_nCurrentFOVMode = 0;

    m_nCurrentMode = MODE_WELD; // whatever your enum default is
    m_hFirstEntity = NULL;
    m_vecFirstPoint = vec3_origin;
    m_bHasFirstPoint = false;
}

LINK_ENTITY_TO_CLASS(weapon_multitool_legacy, CWeaponLegacyMultiTool);
IMPLEMENT_SERVERCLASS_ST(CWeaponLegacyMultiTool, DT_WeaponLegacyMultiTool)
END_SEND_TABLE()
BEGIN_DATADESC(CWeaponLegacyMultiTool)
DEFINE_FIELD(m_nCurrentMode, FIELD_INTEGER),
DEFINE_FIELD(m_hFirstEntity, FIELD_EHANDLE),
DEFINE_FIELD(m_vecFirstPoint, FIELD_VECTOR),
DEFINE_FIELD(m_bHasFirstPoint, FIELD_BOOLEAN),
END_DATADESC()

void CWeaponLegacyMultiTool::PrimaryAttack()
{
    // Trace to detect entities or surfaces
    trace_t tr;
    Vector vecSrc = GetOwner()->EyePosition();
    Vector vecAiming = GetOwner()->EyeDirection3D();

    UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, GetOwner(), COLLISION_GROUP_NONE, &tr);

    if (tr.m_pEnt || tr.DidHitWorld())
    {
        if (m_nCurrentMode == MODE_SCALING)
        {
            // Gradually and smoothly increase the size when holding down the left mouse button
            AdjustObjectSize(1.02f);  // Increase size by 2% each tick for smooth scaling
            m_flNextPrimaryAttack = gpGlobals->curtime + 0.01f;  // Continuous scaling with a small delay
        }
        if (m_nCurrentMode == MODE_RANDOM)
        {
            // Call the RandomChaosMode function to apply a random effect
            RandomChaosMode();
            m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
        }
        if (m_nCurrentMode == MODE_ROPECREATOR)
        {
            CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
            if (!pPlayer)
                return;

            // Trace to detect the point the player is looking at
            trace_t tr;
            Vector vecSrc = pPlayer->EyePosition();
            Vector vecAiming = pPlayer->EyeDirection3D();

            UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

            if (tr.DidHit())
            {
                if (!m_bHasFirstRopePoint)
                {
                    // Set the first point
                    m_vFirstRopePoint = tr.endpos;
                    m_pFirstRopeEntity = tr.m_pEnt;
                    m_bHasFirstRopePoint = true;
                    CreateSparkEffect(tr.endpos);
                }
                else
                {
                    // Set the second point and create the rope
                    m_vSecondRopePoint = tr.endpos;
                    m_pSecondRopeEntity = tr.m_pEnt;
                    CreateRope(m_vFirstRopePoint, m_vSecondRopePoint, m_pFirstRopeEntity, m_pSecondRopeEntity);

                    // Reset the points for the next rope
                    m_bHasFirstRopePoint = false;
                    CreateSparkEffect(tr.endpos);
                }
            }
            m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
        }
            if (m_nCurrentMode == MODE_BEAMCREATOR)
            {
                // Get the player
                CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
                if (!pPlayer)
                    return;

                // Trace to detect the point the player is looking at
                trace_t tr;
                Vector vecSrc = pPlayer->EyePosition();
                Vector vecAiming = pPlayer->EyeDirection3D();

                UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

                if (tr.DidHit())
                {
                    if (!m_bHasFirstPoint)
                    {
                        // Set the first point
                        m_vFirstPoint = tr.endpos;
                        m_bHasFirstPoint = true;
                        CreateSparkEffect(tr.endpos);
                    }
                    else
                    {
                        // Set the second point and create the beam
                        m_vSecondPoint = tr.endpos;
                        CreateSparkEffect(tr.endpos);
                        CreateBeam(m_vFirstPoint, m_vSecondPoint);

                        // Reset the points for the next beam
                        m_bHasFirstPoint = false;

                    }
                }

                m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;  // Add a delay to prevent spamming
            }
            if (m_nCurrentMode == MODE_DUPLICATOR && m_pClonedEntity)
            {
                // Get the player
                CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
                if (!pPlayer)
                    return;

                // Get the player's eye position and aiming direction
                Vector vecSrc = pPlayer->EyePosition();
                Vector vecAiming = pPlayer->EyeDirection3D();

                // Determine the spawn position for the clone (a bit offset from the player)
                Vector vSpawnPos = vecSrc + vecAiming * 100 + m_vCloneOffset;

                // Clone the stored entity and spawn it
                CloneEntity(m_pClonedEntity, vSpawnPos);
                CreateSparkEffect(tr.endpos);

                m_flNextPrimaryAttack = gpGlobals->curtime + 0.1f;  // Add a delay to prevent spamming
            }
            if (m_nCurrentMode == MODE_WELD)
            {
                // Weld Mode
                CreateSparkEffect(tr.endpos);
                if (!m_bHasFirstPoint)
                {
                    m_hFirstEntity = tr.m_pEnt;
                    m_vecFirstPoint = tr.endpos;
                    m_bHasFirstPoint = true;
                }
                else
                {
                    WeldEntities(m_hFirstEntity, tr.m_pEnt, m_vecFirstPoint, tr.endpos);
                    m_hFirstEntity = nullptr;
                    m_bHasFirstPoint = false;
                }
                m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
            }
            else if (m_nCurrentMode == MODE_LIGHTBULBS)
            {
                // Light Bulbs Mode
                CreateLightBulb(tr.endpos, tr.m_pEnt);
                CreateSparkEffect(tr.endpos);
                m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
            }
    }
}

void CWeaponLegacyMultiTool::Precache()
{
    BaseClass::Precache();

    // =====================
    // MODELS
    // =====================

    PrecacheModel("models/props_junk/watermelon01.mdl");
    PrecacheModel("models/props_junk/ravenholmsign.mdl");
    PrecacheModel("models/props_junk/wood_crate001a.mdl");
    PrecacheModel("models/props_c17/doll01.mdl");
    PrecacheModel("models/props_c17/oildrum001.mdl");

    // Rope / beam related
    PrecacheModel("cable/cable.vmt");
    PrecacheModel("sprites/laserbeam.vmt");
    PrecacheModel("sprites/physbeam.vmt");
    PrecacheModel("sprites/strider_bluebeam.vmt"); // FIXED typo

    // =====================
    // SOUNDS
    // =====================

    PrecacheScriptSound("NPC_CScanner.TakePhoto");

    PrecacheSound("vo/npc/male01/question27.wav");
    PrecacheSound("vo/npc/male01/herecomehacks01.wav");
    PrecacheSound("vo/npc/male01/fantastic01.wav");
    PrecacheSound("vo/npc/male01/answer17.wav");

    PrecacheSound("physics/glass/glass_impact_bullet1.wav");
    PrecacheSound("ambient/alarms/klaxon1.wav");
    PrecacheSound("weapons/rpg/rocket1.wav");

    // =====================
    // EFFECTS
    // =====================

    PrecacheParticleSystem("cball_explode"); // Used by DispatchEffect
}

void CWeaponLegacyMultiTool::CreateBeam(const Vector& startPoint, const Vector& endPoint)
{
    struct BeamType
    {
        const char* material;  // Path to the material used for the beam
        int width;             // Width of the beam
        int r, g, b;           // Color of the beam
        float scrollRate;      // Scroll rate for the beam texture
        float brightness;      // Brightness of the beam
        float lifeTime;        // How long the beam lasts
    };

    // Define an array of beam types
    const BeamType beamTypes[] = {
        {"sprites/laserbeam.vmt", 5, 255, 0, 0, 10, 255, 10.0f},   // Red beam
        {"sprites/strider_bluebeam..vmt", 4, 0, 0, 255, 8, 255, 10.0f}, // Blue beam
        {"sprites/physbeam.vmt", 6, 0, 255, 0, 12, 255, 15.0f},   // Green beam
        // Add more beam types as needed
    };

    // Number of beam types
    const int numBeamTypes = sizeof(beamTypes) / sizeof(beamTypes[0]);

    // Get the current beam type
    const BeamType& beam = beamTypes[m_nCurrentBeamType];

    // Create the beam entity
    CBeam* pBeam = CBeam::BeamCreate(beam.material, beam.width);
    if (!pBeam)
        return;

    // Set the beam's start and end positions
    pBeam->PointsInit(startPoint, endPoint);

    // Set other beam properties
    pBeam->SetColor(beam.r, beam.g, beam.b);
    pBeam->SetScrollRate(beam.scrollRate);
    pBeam->SetBrightness(beam.brightness);
    pBeam->SetNoise(0);

    // Activate the beam
    pBeam->RelinkBeam();
    pBeam->LiveForTime(beam.lifeTime);  // The beam will disappear after a certain time
}

#include "cbase.h"
#include "rope.h"

void CWeaponLegacyMultiTool::CreateRope(const Vector& startPoint, const Vector& endPoint, CBaseEntity* pStartEntity, CBaseEntity* pEndEntity)
{
    // Ensure the entities are valid
    if (!pStartEntity || !pEndEntity)
    {
        Msg("Start or End entity is invalid!\n");
        return;
    }

    // Calculate the fixed length based on the distance between the start and end points
    float ropeLength = (startPoint - endPoint).Length();

    // Create the rope
    CRopeKeyframe* pRope = CRopeKeyframe::Create(pStartEntity, pEndEntity, ropeLength);
    if (!pRope)
    {
        Msg("Failed to create rope!\n");
        return;
    }

    // Set properties of the rope
    pRope->m_RopeLength = ropeLength;                     // Lock the rope length to the initial distance
    pRope->m_Width = 2.0f;                                // Set the width of the rope


    // Reduce slack to prevent the rope from extending
    pRope->m_Slack = 0;                                   // No slack means the rope will stay at this length
    pRope->SetupHangDistance(0.0f);                       // Set to 0 to minimize sag and lock length

    pRope->EnableWind(false);                             // Disable wind to prevent unintended movement
    pRope->EnableCollision();                         // Enable collision for the rope

    Msg("Rope and locked length created successfully!\n");
}



void CWeaponLegacyMultiTool::SecondaryAttack()
{
    if (m_nCurrentMode == MODE_SCALING)
    {
        // Gradually and smoothly decrease the size when holding down the right mouse button
        AdjustObjectSize(0.98f);  // Decrease size by 2% each tick for smooth scaling
        m_flNextSecondaryAttack = gpGlobals->curtime + 0.01f;  // Continuous scaling with a small delay
    }
    if (m_nCurrentMode == MODE_BEAMCREATOR)
    {
        int numBeamTypes = 3;
        // Cycle to the next beam type
        m_nCurrentBeamType = (m_nCurrentBeamType + 1) % numBeamTypes;

        // Provide feedback to the player
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, CFmtStr("Beam: %d", m_nCurrentBeamType + 1));

        m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;  // Add a delay to prevent spamming
    }
    if (m_nCurrentMode == MODE_DUPLICATOR)
    {
        // Get the player
        CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
        if (!pPlayer)
            return;

        // Trace to detect the entity the player is looking at
        trace_t tr;
        Vector vecSrc = pPlayer->EyePosition();
        Vector vecAiming = pPlayer->EyeDirection3D();

        UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

        if (tr.m_pEnt && !tr.m_pEnt->IsWorld())
        {
            // Store the entity to be cloned
            m_pClonedEntity = tr.m_pEnt;

            // Retrieve some data about the entity
            const char* className = m_pClonedEntity->GetClassname();
            const char* modelName = STRING(m_pClonedEntity->GetModelName());
            Vector position = m_pClonedEntity->GetAbsOrigin();
            QAngle angles = m_pClonedEntity->GetAbsAngles();

            // Print the data to the player’s HUD
            ClientPrint(pPlayer, HUD_PRINTTALK, CFmtStr("Entity Data:\n"));
            ClientPrint(pPlayer, HUD_PRINTTALK, CFmtStr("Classname: %s\n", className));
            ClientPrint(pPlayer, HUD_PRINTTALK, CFmtStr("Model: %s\n", modelName));
            ClientPrint(pPlayer, HUD_PRINTTALK, CFmtStr("Position: [%.1f, %.1f, %.1f]\n", position.x, position.y, position.z));
            ClientPrint(pPlayer, HUD_PRINTTALK, CFmtStr("Angles: [%.1f, %.1f, %.1f]\n", angles.x, angles.y, angles.z));

            // Provide additional feedback to the player
            CreateSparkEffect(tr.endpos);
        }
        else
        {
            ClientPrint(pPlayer, HUD_PRINTCENTER, "No valid entity found.");
        }

        m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;  // Add a delay to prevent spamming
    }
    else
    {
        BaseClass::SecondaryAttack();
    }
}

void CWeaponLegacyMultiTool::AdjustObjectSize(float scaleFactor)
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    trace_t tr;
    Vector vecSrc = pPlayer->EyePosition();
    Vector vecAiming = pPlayer->EyeDirection3D();

    UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

    if (tr.DidHit() && tr.m_pEnt && tr.m_pEnt->entindex() != 0)  // Ensure it's not the world entity
    {
        CBaseEntity* pEntity = tr.m_pEnt;
        if (pEntity)
        {
            // Retrieve the current scale from the entity's keyvalues or a default
            float currentScale = 1.0f;  // Default scale
            char scaleStr[32] = "1.0";  // Default string to hold scale

            // Correctly retrieve the scale value from keyvalues
            pEntity->GetKeyValue("modelscale", scaleStr, sizeof(scaleStr));

            // Convert the string scale value to a float
            currentScale = atof(scaleStr);

            // Calculate new scale
            float newScale = currentScale * scaleFactor;

            // Cap the scale between -200.0f and 2000.0f
            if (newScale > 2000.0f)
            {
                newScale = 2000.0f;
            }
            else if (newScale < -200.0f)
            {
                newScale = -200.0f;
            }

            // Convert the new scale to a string and apply it back as a keyvalue
            char newScaleStr[32];
            Q_snprintf(newScaleStr, sizeof(newScaleStr), "%f", newScale);
            pEntity->KeyValue("modelscale", newScaleStr);

            // Print the scale change to the client
            char scaleChangeMsg[64];
            Q_snprintf(scaleChangeMsg, sizeof(scaleChangeMsg), "Scaling object: New Scale Factor: %f", newScale);
            ClientPrint(pPlayer, HUD_PRINTCENTER, scaleChangeMsg);

            pEntity->Activate();  // Ensure the entity updates its scale and properties
        }
    }
}

void CWeaponLegacyMultiTool::ItemPostFrame()
{
    // Check if the player pressed the "R" key to switch modes
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (pPlayer && pPlayer->m_afButtonPressed & IN_RELOAD)
    {
        SwitchMode();
    }

    BaseClass::ItemPostFrame();
}


void CWeaponLegacyMultiTool::CloneEntity(CBaseEntity* pOriginal, const Vector& vSpawnPos)
{
    if (!pOriginal)
        return;

    // Create a new entity of the same class
    CBaseEntity* pClone = CreateEntityByName(pOriginal->GetClassname());
    if (!pClone)
        return;

    // Copy position and orientation
    pClone->SetAbsOrigin(vSpawnPos);
    pClone->SetAbsAngles(pOriginal->GetAbsAngles());

    // Copy key properties (you can expand this to include more properties as needed)
    pClone->SetModel(STRING(pOriginal->GetModelName()));
    pClone->SetOwnerEntity(pOriginal->GetOwnerEntity());

    // Copy more complex data
    if (pOriginal->VPhysicsGetObject())
    {
        pClone->SetMoveType(pOriginal->GetMoveType());
        pClone->SetSolid(pOriginal->GetSolid());
        pClone->VPhysicsInitNormal(SOLID_VPHYSICS, pOriginal->VPhysicsGetObject()->GetMass(), false);
    }

    // Spawn the clone in the world
    pClone->Spawn();

    // Copy custom properties (expand this to copy any specific attributes or data)
    // Example: Copy health if it's an NPC
    if (pOriginal->IsNPC())
    {
        CAI_BaseNPC* pOriginalNPC = static_cast<CAI_BaseNPC*>(pOriginal);
        CAI_BaseNPC* pCloneNPC = static_cast<CAI_BaseNPC*>(pClone);
        pCloneNPC->SetHealth(pOriginalNPC->GetHealth());
    }
}

void CWeaponLegacyMultiTool::SwitchMode()
{
    m_nCurrentMode = static_cast<ToolMode>((m_nCurrentMode + 1) % 8);  // Adjust the modulo based on the number of modes

    switch (m_nCurrentMode)
    {
    case MODE_WELD:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Weld");
        break;
    case MODE_LIGHTBULBS:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Light Bulbs");
        break;
    case MODE_DUPLICATOR:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Duplicator");
        break;
    case MODE_BEAMCREATOR:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Beam Creator");
        break;
    case MODE_ROPECREATOR:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Rope Creator (BETA)");
        break;
    case MODE_SCALING:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Size Changer (BETA)");
        break;
    case MODE_RANDOM:
        ClientPrint(ToBasePlayer(GetOwner()), HUD_PRINTCENTER, "Mode: Random Shit Happens (BETA)");
        break;
    default:
        break;
    }
}

void CWeaponLegacyMultiTool::SizeManipulationMode()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    trace_t tr;
    Vector vecSrc = pPlayer->EyePosition();
    Vector vecAiming = pPlayer->EyeDirection3D();

    UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

    if (tr.DidHit())
    {
        CBaseEntity* pEntity = tr.m_pEnt;
        if (pEntity)
        {
            // Get the current collision bounds
            Vector mins, maxs;
            pEntity->CollisionProp()->WorldSpaceAABB(&mins, &maxs);

            // Calculate the new scale factor
            float scaleFactor = 1.0f;
            if (pPlayer->m_afButtonPressed & IN_ATTACK) // Increase size
            {
                scaleFactor = 1.2f;
                CreateSparkEffect(tr.endpos);
            }
            else if (pPlayer->m_afButtonPressed & IN_ATTACK2) // Decrease size
            {
                scaleFactor = 0.8f;
                CreateSparkEffect(tr.endpos);
            }

            // Adjust the collision bounds
            Vector vecSize = maxs - mins;
            Vector newSize = vecSize * scaleFactor;
            Vector vecCenter = (mins + maxs) * 0.5f;

            Vector newMins = vecCenter - (newSize * 0.5f);
            Vector newMaxs = vecCenter + (newSize * 0.5f);

            // Set the new collision bounds
            pEntity->SetCollisionBounds(newMins, newMaxs);

            // Optionally adjust mass or other physics properties here
            if (pEntity->VPhysicsGetObject())
            {
                IPhysicsObject* pPhysics = pEntity->VPhysicsGetObject();
                pPhysics->SetMass(pPhysics->GetMass() * scaleFactor);  // Adjust mass proportionally
            }
        }
    }
}



void CWeaponLegacyMultiTool::TakeScreenshot()
{

    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    // Capture the screenshot
    engine->ClientCommand(false, "screenshot");
    Error("It is a error!");
    EmitSound("NPC_CScanner.TakePhoto");
}
void CWeaponLegacyMultiTool::WeldEntities(CBaseEntity* pEntity1, CBaseEntity* pEntity2, const Vector& vec1, const Vector& vec2)
{
    if (!pEntity1 || !pEntity2)
        return;

    IPhysicsObject* pPhysics1 = pEntity1->VPhysicsGetObject();
    IPhysicsObject* pPhysics2 = pEntity2->VPhysicsGetObject();

    if (pPhysics1 && pPhysics2)
    {
        constraint_fixedparams_t fixed;
        fixed.Defaults();
        fixed.InitWithCurrentObjectState(pPhysics1, pPhysics2);
        physenv->CreateFixedConstraint(pPhysics1, pPhysics2, nullptr, fixed);
    }
}

void CWeaponLegacyMultiTool::CreateLightBulb(const Vector& position, CBaseEntity* pTargetEntity)
{
    // Create a watermelon that emits light
    CBaseEntity* pEntity = CreateEntityByName("prop_physics");
    if (pEntity)
    {
        pEntity->SetModel("models/props_junk/watermelon01.mdl");
        pEntity->SetAbsOrigin(position);
        pEntity->Spawn();

        // Attach a dynamic light to the watermelon
        CBaseEntity* pLight = CreateEntityByName("light_dynamic");
        if (pLight)
        {
            pLight->SetAbsOrigin(position);
            pLight->KeyValue("brightness", "4");
            pLight->KeyValue("distance", "200");
            pLight->KeyValue("style", "0");
            pLight->KeyValue("color", "255 255 255");
            pLight->Spawn();
            pLight->SetParent(pEntity); // Parent the light to the watermelon
        }
        else
        {
            // Weld the watermelon to the world
            if (pTargetEntity->entindex() == 0)
            {
                pEntity->SetParent(pTargetEntity);
            }
        }
    }
}



void CWeaponLegacyMultiTool::CreateSparkEffect(const Vector& position)
{
    CEffectData data;
    data.m_vOrigin = position;
    DispatchEffect("cball_explode", data);
}

void CWeaponLegacyMultiTool::RandomChaosMode()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    trace_t tr;
    Vector vecSrc = pPlayer->EyePosition();
    Vector vecAiming = pPlayer->EyeDirection3D();

    UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 1000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

    if (tr.DidHit())
    {
        if (tr.m_pEnt->entindex() == 0)
        {
            Warning("Can't apply to the world!\n");
        }
        else
        {
            CBaseEntity* pEntity = tr.m_pEnt;
            if (pEntity)
            {
                // Generate a random number to determine which effect to apply
                int effect = random->RandomInt(1, 7);

                switch (effect)
                {
                case 1:
                    // Grow or shrink the object randomly
                {
                    float scaleFactor = random->RandomFloat(0.5f, 2.0f);
                    Vector origin = pEntity->GetAbsOrigin();
                    QAngle angles = pEntity->GetAbsAngles();
                    string_t modelName = pEntity->GetModelName();
                    UTIL_Remove(pEntity);
                    CBaseEntity* pNewEntity = CreateEntityByName(pEntity->GetClassname());
                    if (pNewEntity)
                    {
                        pNewEntity->SetAbsOrigin(origin);
                        pNewEntity->SetAbsAngles(angles);
                        pNewEntity->SetModel(STRING(modelName));
                        pNewEntity->KeyValue("modelscale", scaleFactor);
                        pNewEntity->Spawn();
                    }
                }
                break;
                case 2:
                    // Play a random funny sound
                {
                    const char* sounds[] = {
                        "vo/npc/male01/question27.wav", // Replace with funny sounds
                        "vo/npc/male01/herecomehacks01.wav",
                        "vo/npc/male01/fantastic01.wav",
                        "vo/npc/male01/answer17.wav"   // Dream about cheese sound
                    };
                    EmitSound("vo/npc/male01/answer17.wav");
                }
                break;
                case 3:
                    // Apply spinning effect using AngularImpulse
                {
                    AngularImpulse spinImpulse = AngularImpulse(0, 0, random->RandomInt(100, 500));

                    IPhysicsObject* pPhysics = pEntity->VPhysicsGetObject();
                    if (pPhysics)
                    {
                        pPhysics->AddVelocity(nullptr, &spinImpulse);
                    }
                }
                break;
                case 4:
                    // Launch the object into the air
                {
                    Vector force = Vector(0, 0, random->RandomFloat(300, 600));
                    pEntity->ApplyAbsVelocityImpulse(force);
                }
                break;
                case 5:
                    // Change the color of the object randomly
                {
                    color32 newColor = { random->RandomInt(0, 255), random->RandomInt(0, 255), random->RandomInt(0, 255), 255 };
                    pEntity->SetRenderColor(newColor.r, newColor.g, newColor.b);
                }
                break;
                case 6:
                    // Explode into harmless confetti
                {
                    pEntity->EmitSound("physics/glass/glass_impact_bullet1.wav");
                    // Spawn confetti effect here
                    // Example:
                    // DispatchParticleEffect("confetti", pEntity->GetAbsOrigin(), pEntity->GetAbsAngles());
                    UTIL_Remove(pEntity);
                }
                break;
                case 7:
                    // Transform into a random funny prop
                {
                    const char* funnyModels[] = {
                        "models/props_junk/ravenholmsign.mdl",
                        "models/props_junk/wood_crate001a.mdl",
                        "models/props_c17/doll01.mdl"
                    };
                    pEntity->SetModel(funnyModels[random->RandomInt(0, 2)]);
                }
                break;
                case 8:
                    // Rocket propulsion
                {
                    pEntity->EmitSound("weapons/rpg/rocket1.wav");
                    Vector force = vecAiming * random->RandomFloat(500, 1000);
                    pEntity->ApplyAbsVelocityImpulse(force);
                    // Add a trail effect if possible
                }
                break;
                case 9:
                    // Gravity flip
                {
                    IPhysicsObject* pPhysics = pEntity->VPhysicsGetObject();
                    if (pPhysics)
                    {
                        Vector force = Vector(0, 0, -pPhysics->GetMass() * 9.8f);  // Reverse gravity
                        pPhysics->AddVelocity(&force, nullptr);
                    }
                }
                break;
                case 10:
                    // Clown horn sound and change to clown prop
                {
                    pEntity->EmitSound("ambient/alarms/klaxon1.wav");
                    pEntity->SetModel("models/props_c17/oildrum001.mdl"); // Change to a clownish prop, replace with a better model if available
                }
                break;
                case 12:
                    // Random teleportation
                {
                    Vector newPos = pEntity->GetAbsOrigin() + Vector(random->RandomFloat(-100, 100), random->RandomFloat(-100, 100), 0);
                    pEntity->SetAbsOrigin(newPos);
                }
                break;
                default:
                    break;
                }
            }
        }
        CreateSparkEffect(tr.endpos);
    }

    m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;  // Delay before the next attack
}
