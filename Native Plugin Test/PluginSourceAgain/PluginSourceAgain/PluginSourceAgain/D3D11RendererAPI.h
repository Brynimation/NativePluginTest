#include "IUNITYGRAPHICS.H"
#include "d3d11.h"
#include "IUnityGraphicsD3D11.h"
#include <d3dcompiler.h>
#include <filesystem>
#include <vector>
#include "Data.cpp"


class D3D11RendererAPI
{
public:
    D3D11RendererAPI();
    D3D11RendererAPI(std::vector<MeshVertex> verts);
    void OnGraphicsDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* intefaces);
    void GetUsesReverseZ();
    void Draw(ConstantBufferData cbData);
    void BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeigth, int* outputRowPitch);
    void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* datPtr);
    void* BeginModifyVB(void* bufferHandle, size_t outBufferSize);
    void EndModifyVB(void* bufferHandle);
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
    bool BufferEmpty();
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
    std::vector<MeshVertex> verts;
};


//SAL Annotations are a set of annotations that describe how a function uses its return types.
//HRESULT is a data type that represents the completion status of a function.
//LPCSTR is a Long Pointer to a Constant String
//LPCWSTR is a Long Pointer to a Constant Wide String. It's "Wide" because its characters are stored in 2 bytes instead of chars

