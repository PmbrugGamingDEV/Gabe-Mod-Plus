//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared code for Gabe Mod
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "util.h"
#include "engine/IEngineSound.h"
#include "entitylist.h"
#include "vphysics_interface.h"
#include "collisionutils.h"
#include "props.h"
#include "soundent.h"
#include "ai_basenpc.h"
#include "vphysics/constraints.h"
#include "physics.h"
#include "player.h"
#include "game.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"