#include <stdlib.h>
#include <string>
#include <iostream>
#include "IUnityInterface.h"
#include "IUnityGraphics.h"

/*We use the IUnityGraphics interface to give our plugin access to generic graphics device
functionality.

To declare an array in c++: type arrName[numItems]
type arrName[5] = {1,2,3,4,5} //can't contain more than 5 elements due to the size constraint
*/
static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_RendererType = kUnityGfxRendererNull;

// Unity plugin load event
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