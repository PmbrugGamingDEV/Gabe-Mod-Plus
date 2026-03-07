#pragma once

#include "cbase.h"

// ==========================================================
// ENUMS
// ==========================================================

enum MultitoolMode_t
{
    MODE_REMOVE = 0,
    MODE_DISTANCE,
    MODE_COLOR,
    MODE_CONSTRAINT,
    MODE_IGNITE,
    MODE_DUPLICATE,
    MODE_EXPLODE,
    MODE_POINTMESSAGE,
    MODE_LIGHT_WATERMELON,
    MODE_DECAL,
    MODE_MATERIAL,
    MODE_FACEPOSER,
    MODE_MAX
};

// ==========================================================
// COLOR PRESETS
// ==========================================================

struct ColorPreset
{
    int r, g, b;
    const char* name;
};

extern const ColorPreset g_ColorPresets[];
extern const int COLOR_COUNT;

// ==========================================================
// DECALS
// ==========================================================

extern const char* g_DecalNames[];
extern const int DECAL_COUNT;

// ==========================================================
// RENDER FX
// ==========================================================

struct RenderFxEntry
{
    RenderFx_t fx;
    const char* name;
};

extern const RenderFxEntry g_RenderFxModes[];
extern const int RENDERFX_COUNT;

// ==========================================================
// FACE PRESETS
// ==========================================================

struct FlexPair
{
    const char* flex;
    float weight;
};

struct FacePreset
{
    const char* name;
    const FlexPair* pairs;
    int pairCount;
};

extern const FacePreset g_FacePresets[];
extern const int FACE_COUNT;

// ==========================================================
// MODE DISPLAY NAMES
// ==========================================================

extern const char* g_szModeNames[];