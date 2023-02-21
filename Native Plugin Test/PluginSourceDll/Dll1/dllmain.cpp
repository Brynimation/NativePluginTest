#include <stdlib.h>
#include "pch.h"
#include <string>
#include <iostream>
#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include "d3d11.h" //direct113d header
#include "IUnityGraphicsD3D11.h" //specific unity graphics api
/*We use the IUnityGraphics interface to give our plugin access to generic graphics device
functionality.
UNITY_INTERFACE_EXPORT is a macro, defined as _declspec(dllexport). This allows
us to export data, functions, classes and class members from a dll.
UNITY_INTERFACE_API is a macro, defined as _stdcall. This is a calling convention
used to call Win32 API functions.
A calling convention is an implementation-level scheme for how subroutines receive
parameters from, and return results to, their caller.
Any functions we want to be able to call from our C# script should be preceded by
these two macros.
*/

typedef void (*FuncPtr)(const char*);
FuncPtr Debug;


static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_RendererType = kUnityGfxRendererNull;

ID3D11Device* device = NULL;
ID3D11DeviceContext* context = NULL;

extern "C" UNITY_INTERFACE_EXPORT void SetDebugFunction(FuncPtr fp)
{
    Debug = fp;
}

static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
    case kUnityGfxDeviceEventInitialize:
    {
        s_RendererType = s_Graphics->GetRenderer();
        //TODO: user initialization code
        break;
    }
    case kUnityGfxDeviceEventShutdown:
    {
        s_RendererType = kUnityGfxRendererNull;
        //TODO: user shutdown code
        break;
    }
    case kUnityGfxDeviceEventBeforeReset:
    {
        //TODO: user Direct3D 9 code
        break;
    }
    case kUnityGfxDeviceEventAfterReset:
    {
        //TODO: user Direct3D 9 code
        break;
    }
    };
}

// Unity plugin load event. This is a callback; a function that
// unity calls when the plugin is loaded.
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces * unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = unityInterfaces->Get<IUnityGraphics>();

    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    // to not miss the event in case the graphics device is already initialized
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

// Unity plugin unload event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}


extern "C" static void UNITY_INTERFACE_API OnRenderEvent(int eventId)
{
    Debug("Hello world!");
}
//This returns a function that unity can call whenever is needed

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API getEventFunction()
{
    Debug("Hello world!");
}
/*UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API getEventFunction()
{
    return OnRenderEvent;
}*/