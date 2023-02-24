#include "IUnityInterface.h"
#include "IUnityGraphics.h";
#include "IUnityGraphicsD3D11.h";
#include "d3d11.h";
#include <d3dcompiler.h>
#include <experimental/filesystem>;

struct ConstantBufferData 
{
    float M[16];
    float V[16];
    float P[16];
};
class D3D11RendererAPI
{
public:
    D3D11RendererAPI()
    void OnGraphicsDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* intefaces);
    void GetUsesReverseZ();
    void Draw();
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
    ID3D11DepthStencilState* depthStencilState;
};


//SAL Annotations are a set of annotations that describe how a function uses its return types.
//HRESULT is a data type that represents the completion status of a function.
//LPCSTR is a Long Pointer to a Constant String
//LPCWSTR is a Long Pointer to a Constant Wide String. It's "Wide" because its characters are stored in 2 bytes instead of chars

HRESULT D3D11RendererAPI::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
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


void D3D11RendererAPI::CreateResources()
{

    //Get current directory
    LPCWSTR vertexShaderPath = L"\\Shaders\\VertexShader.hlsl";
    LPCWSTR pixelShaderPath = L"\\Shaders\\PixelShader.hlsl";

    //Initialise vertex buffer
    D3D11_BUFFER_DESC desc; //STRUCT: describes a buffer resource
    //&desc gets the memory address at which desc is stored
    memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT; //read n write by GPU
    desc.ByteWidth = 1024; //size of vertex buffer in bytes
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //bind current buffer to the vertex buffer
    device->CreateBuffer(&desc, NULL, &vertexBuffer);

    //Initialise constant buffer - used to efficiently supply shader constants
    //to the pipeline.
    desc.Usage = D3D11_USAGE_DYNAMIC; //read from gpu, write by cpu
    desc.ByteWidth = 1024;//64; //holds the world matrix?
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
        };
    }
}
void D3D11RendererAPI::ReleaseResources()
{

}

void D3D11RendererAPI::Draw()
{

}
void D3D11RendererAPI::OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType, IUnityInterfaces* interfaces)
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

