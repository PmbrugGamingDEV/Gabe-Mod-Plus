#include "cbase.h"
#include <Awesomium/WebCore.h>
#include <Awesomium/WebConfig.h>

using namespace Awesomium;

// ----------------------------------------------------
// Global state
// ----------------------------------------------------

static WebCore* g_WebCore = nullptr;
static int g_iAwesomiumViewCount = 0;

// ----------------------------------------------------
// Initialization
// ----------------------------------------------------

void Awesomium_EnsureInitialized()
{
    if (g_WebCore)
        return;

    WebConfig config;

    // Optional config tuning
    // config.log_level = kLogLevel_Normal;
    // config.remote_debugging_port = 0;

    g_WebCore = WebCore::Initialize(config);

    if (g_WebCore)
    {
        Msg("[Awesomium] WebCore initialized.\n");
    }
    else
    {
        Warning("[Awesomium] WebCore failed to initialize!\n");
    }
}

// ----------------------------------------------------
// Per-frame update (call from panel Think())
// ----------------------------------------------------

void Awesomium_Update()
{
    if (g_WebCore)
        g_WebCore->Update();
}

// ----------------------------------------------------
// Shutdown (called automatically when last view dies)
// ----------------------------------------------------

static void Awesomium_Shutdown()
{
    if (!g_WebCore)
        return;

    g_WebCore->Shutdown();
    g_WebCore = nullptr;

    Msg("[Awesomium] WebCore shutdown.\n");
}

// ----------------------------------------------------
// View reference tracking
// ----------------------------------------------------

void Awesomium_RegisterView()
{
    g_iAwesomiumViewCount++;
}

void Awesomium_UnregisterView()
{
    g_iAwesomiumViewCount--;

    if (g_iAwesomiumViewCount <= 0)
    {
        g_iAwesomiumViewCount = 0;
        Awesomium_Shutdown();
    }
}

// ----------------------------------------------------
// Accessor (optional)
// ----------------------------------------------------

WebCore* Awesomium_GetCore()
{
    return g_WebCore;
}
