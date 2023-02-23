#include <stdlib.h>
#include <filesystem>
#include "pch.h"
#include <experimental/filesystem>
#include <string>
#include <iostream>
#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include <d3dcompiler.h>
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

class dllmain
{
public:
    void OnGraphicsDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* intefaces);
    void GetUsesReverseZ();
    void DrawSimpleTriangles();
    void BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeigth, int* outputRowPitch);
    void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* datPtr);
    void* BeginModifyVB(void* bufferHandle, size_t outBufferSize);
    void EndModifyVB(void* bufferHandle);
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
private:
    void CreateResources();
    void ReleaseResources();

private:
    LPCWSTR shaderFilePath;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ID3D11Buffer* vertexBuffer;
    ID3DBlob* vertexShaderBlob;
    ID3D11Buffer* constantBuffer;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11RasterizerState* rasterizerState;
    ID3D11BlendState* blendState;
    ID3D11DepthStencilState* DepthStencilState;
};

/*typedef void (*FuncPtr)(const char*);
FuncPtr Debug;


static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_RendererType = kUnityGfxRendererNull;

ID3D11Device* device = NULL;
ID3D11DeviceContext* context = NULL; */

/*extern "C" UNITY_INTERFACE_EXPORT void SetDebugFunction(FuncPtr fp)
{
    Debug = fp;
}*/

//SAL Annotations are a set of annotations that describe how a function uses its return types.
//HRESULT is a data type that represents the completion status of a function.
//LPCSTR is a Long Pointer to a Constant String
//LPCWSTR is a Long Pointer to a Constant Wide String. It's "Wide" because its characters are stored in 2 bytes instead of chars

HRESULT dllmain::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
    if (!srcFile || !entryPoint || !profile || !blob)
        return E_INVALIDARG;

    *blob = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif

    const D3D_SHADER_MACRO defines[] =
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint, profile, flags, 0, &shaderBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return hr;
    }

    *blob = shaderBlob;

    return hr;
}


void dllmain::CreateResources()
{

    //Get current directory
    LPCWSTR vertexShaderPath = L"\\Shaders\\VertexShader.hlsl";
    LPCWSTR pixelShaderPath = L"\\Shaders\\PixelShader.hlsl";

    //Initialise vertex buffer
    D3D11_BUFFER_DESC desc; //STRUCT: describes a buffer resource
    //&desc gets the memory address at which desc is stored
    memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = 1024; //size of vertex buffer in bytes
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //bind current buffer to the vertex buffer
    device->CreateBuffer(&desc, NULL, &vertexBuffer);

    //Initialise constant buffer - used to efficiently supply shader constants
    //to the pipeline.
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = 64; //holds the world matrix?
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    device->CreateBuffer(&desc, NULL, &constantBuffer);

    //create vertex shader
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = CompileShader(vertexShaderPath, "vert", "vs_4_0_level_9_1", &vsBlob);
    if (FAILED(hr))
    {
        printf("Failed compiling vertex shader %08X\n", hr);
        return;
    }
    hr = device->CreateVertexShader(vsBlob, sizeof(vsBlob), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        printf("Failed to create vertex shader\n");
    }

    //Create pixel shader
    ID3DBlob* psBlob = nullptr;
    hr = CompileShader(pixelShaderPath, "frag", "ps_4_0_level_9_1", &psBlob);
    if (FAILED(hr)) 
    {
        printf("Failed compiling pixel shader %08X\n", hr);
    }
    hr = device->CreatePixelShader(vsBlob, sizeof(vsBlob), nullptr, &pixelShader);
    if (FAILED(hr))
    {
        printf("Failed to create pixel shader\n");
    }

    //Create input layout
    if (vertexShader) 
    {
        //Description of a single element for the input assembler stage
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = 
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        }
    }
}
void dllmain::ReleaseResources()
{

}
void dllmain::OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType, IUnityInterfaces* interfaces)
{
    switch (eventType)
    {
    case kUnityGfxDeviceEventInitialize:
    {
        IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
        device = d3d->GetDevice();
        CreateResources();
        break;
    }
    case kUnityGfxDeviceEventShutdown:
    {
        ReleaseResources();
    }
    }
}


/*static void UNITY_INTERFACE_API
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
    };*/


    // Unity plugin load event. This is a callback; a function that
    // unity calls when the plugin is loaded.
    /*extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
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
