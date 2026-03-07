#include "cbase.h"
#include "multitool_shared.h"

// ---------------------------------------------------
// MODE NAMES
// ---------------------------------------------------

const char* g_szModeNames[] =
{
    "Remove",
    "Distance (Source Engine Units)",
    "Color",
    "Constraints",
    "Ignite",
    "Duplicate",
    "Explode",
    "Point Message",
    "Light Watermelon",
    "Decal",
    "Render FX",
    "Faceposer (NPCs)"
};

// ---------------------------------------------------
// COLORS
// ---------------------------------------------------

const ColorPreset g_ColorPresets[] =
{
    {255,0,0,"Red"},
    {255,165,0,"Orange"},
    {255,255,0,"Yellow"},
    {0,255,0,"Green"},
    {0,0,255,"Blue"},
    {75,0,130,"Indigo"},
    {128,0,128,"Purple"},
    {238,130,238,"Violet"},
    {255,192,203,"Pink"},
    {0,0,0,"Black"},
    {255,255,255,"White"},
    {0,0,0,"Rainbow"}
};

const int COLOR_COUNT =
sizeof(g_ColorPresets) / sizeof(g_ColorPresets[0]);

// ---------------------------------------------------
// DECALS
// ---------------------------------------------------

const char* g_DecalNames[] =
{
    "YellowBlood",
    "Bigshot",
    "RedGlowFade",
    "BeerSplash",
    "Blood",
    "Scorch",
    "ManhackCut",
    "FadingScorch",
    "Rollermine.Crater",
    "Impact.Concrete",
    "Impact.Metal",
    "Impact.Glass",
    "Impact.Sand"
};

const int DECAL_COUNT =
sizeof(g_DecalNames) / sizeof(g_DecalNames[0]);

// ---------------------------------------------------
// RENDER FX
// ---------------------------------------------------

const RenderFxEntry g_RenderFxModes[] =
{
    { kRenderFxNone, "None" },
    { kRenderFxPulseSlow, "Pulse Slow" },
    { kRenderFxPulseFast, "Pulse Fast" },
    { kRenderFxPulseSlowWide, "Pulse Wide" },
    { kRenderFxFadeSlow, "Fade Slow" },
    { kRenderFxFadeFast, "Fade Fast" },
    { kRenderFxSolidSlow, "Solid Slow" },
    { kRenderFxSolidFast, "Solid Fast" },
    { kRenderFxStrobeSlow, "Strobe Slow" },
    { kRenderFxStrobeFast, "Strobe Fast" },
    { kRenderFxStrobeFaster, "Strobe Faster" },
    { kRenderFxFlickerSlow, "Flicker Slow" },
    { kRenderFxFlickerFast, "Flicker Fast" },
    { kRenderFxNoDissipation, "No Dissipation" },
    { kRenderFxDistort, "Distort" },
    { kRenderFxHologram, "Hologram" },
    { kRenderFxExplode, "Explode" },
    { kRenderFxGlowShell, "Glow Shell" },
    { kRenderFxClampMinScale, "Clamp Min Scale" }
};

const int RENDERFX_COUNT =
sizeof(g_RenderFxModes) / sizeof(g_RenderFxModes[0]);

// ---------------------------------------------------
// FACE PRESETS
// ---------------------------------------------------

extern const FlexPair g_Face_Smile[];
extern const FlexPair g_Face_Sad[];
extern const FlexPair g_Face_Angry[];
extern const FlexPair g_Face_Scared[];
extern const FlexPair g_Face_Surprised[];
extern const FlexPair g_Face_Neutral[];

const FacePreset g_FacePresets[] =
{
    { "Smile", g_Face_Smile, 4 },
    { "Sad", g_Face_Sad, 4 },
    { "Angry", g_Face_Angry, 4 },
    { "Scared", g_Face_Scared, 5 },
    { "Surprised", g_Face_Surprised, 4 },
    { "Neutral", g_Face_Neutral, 1 }
};

const int FACE_COUNT =
sizeof(g_FacePresets) / sizeof(g_FacePresets[0]);