/*Huge amount of this is boiler plate code from the following links:
* https://github.com/Unity-Technologies/NativeRenderingPlugin
 https://docs.unity3d.com/Manual/NativePluginInterface.html
*/

#include "IUnityGraphics.h";
#include "IUnityGraphicsD3D11.h";
#include "d3d11.h";
#include "DirectXMath.h"
#include "D3D11RendererAPI.h";
#include <d3dcompiler.h>
#include <experimental/filesystem>;
#include "Data.cpp";





static IUnityInterfaces* s_Interfaces;
static IUnityGraphics* s_Graphics;
static UnityGfxRenderer s_RendererType;
static D3D11RendererAPI* renderer;
static ConstantBufferData g_ConstantBufferData;

static float g_Time; //global variable: current time
static void* g_VertexBufferHandle = NULL;
static int g_VertexBufferCount = 0;

static void* g_ConstantBufferHandle = NULL;





static std::vector<MeshVertex> g_Vertices; //vertex buffer

// IUnityInterface is a registry of interfaces we choose to expose to plugins.
//
// USAGE:
// ---------
// To retrieve an interface a user can do the following from a plugin, assuming they have the header file for the interface:
//
// IMyInterface * ptr = registry->Get<IMyInterface>();

extern "C" {

	//This function is the one called from our C# script when we call GL.IssuePluginEvent()
	UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() 
	{
		return OnRenderEvent;
	}

	//This function is called when the plugin is loaded into unity. Any external script 
	//that handles unity events must export UnityPluginLoad and UnityPluginUnload functions
	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		s_Interfaces = unityInterfaces->Get<IUnityInterfaces>();
		s_Graphics = unityInterfaces->Get<IUnityGraphics>();
		s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

		//The above provides us with access to the graphics device and hence the ability to
		//access all stages of the D3D11 rendering pipeline
	}

	//called every frame from the update function in our useRenderingPlugin.cs script
	void UNITY_INTERFACE_API UNITY_INTERFACE_EXPORT SetTimeFromUnity(float time)
	{
		g_Time = time;
	}

	void UNITY_INTERFACE_API UNITY_INTERFACE_EXPORT SetShaderUniformsFromUnity(ConstantBufferData cbd) 
	{
		g_ConstantBufferData = cbd;
	}
	void UNITY_INTERFACE_API UNITY_INTERFACE_EXPORT SetMeshBuffersFromUnity
	(void *vertexBufferHandle, int vertexCount, float* sourceVerts,
	float *sourceColours)
	{
		g_VertexBufferHandle = vertexBufferHandle;
		g_VertexBufferCount = vertexCount;
		
		g_Vertices.resize(vertexCount);
		//Fill the vertex buffer from all of our c# arrays
		for(int i = 0; i < vertexCount; i++)
		{
			//create a reference variable, vert, to ease future assignment
			MeshVertex& vert = g_Vertices[i];
			vert.pos[0] = sourceVerts[0];
			vert.pos[1] = sourceVerts[1];
			vert.pos[2] = sourceVerts[2];
			/*Incrementing pointers in c++ means the pointer points to the next
			memory address. As arrays are blocks of memory stored contiguously,
			we can increment a pointer to iterate over an array.*/
			sourceVerts += 3;
			vert.colour[0] = sourceColours[0];
			vert.colour[1] = sourceColours[1];
			vert.colour[2] = sourceColours[2];
			vert.colour[3] = sourceColours[3];
			sourceColours += 4;

		}
	}
}

static void Draw() 
{
	renderer->Draw(g_ConstantBufferData);
}
static void UNITY_INTERFACE_API OnRenderEvent(int eventId) 
{
	if (renderer && !renderer->BufferEmpty()) 
	{
		Draw();
	}
}
static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType) 
{
	if (eventType == kUnityGfxDeviceEventInitialize) 
	{
		//only support D3D11 for the time being
		s_RendererType = s_Graphics->GetRenderer();
		if (!(s_RendererType == kUnityGfxRendererD3D11)) 
		{
			return;
		}
		renderer = new D3D11RendererAPI(g_Vertices);
	}
	//if our renderer is not null, process the event
	if (renderer) 
	{
		renderer->OnGraphicsDeviceEvent(eventType, s_Interfaces);
	}
}