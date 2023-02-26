#include "D3D11RendererAPI.h"
//Here's to help you tomorrow: https://www.youtube.com/watch?v=sjt1Lury6vk

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

D3D11RendererAPI::D3D11RendererAPI(std::vector<MeshVertex> verts) 
{
    this->verts = verts;
    device = nullptr;
    shaderFilePath = L"";
    context = nullptr;
    vertexBuffer = nullptr;
    vertexShaderBlob = nullptr;
    constantBuffer = nullptr;
    vertexShader = nullptr;
    pixelShader = nullptr;
    inputLayout = nullptr;
    rasterizerState = nullptr;
    blendState = nullptr;
    depthStencilState = nullptr;
}

bool D3D11RendererAPI::BufferEmpty() 
{
    return verts.empty();
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
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = 1024; //size of vertex buffer in bytes - sizeof(vertex) * arraySize'
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //bind current buffer to the vertex buffer

    //populate the vertex buffer with our vertices from unity
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    MeshVertex* pVerts = &verts[0]; //Get pointer to the source vertices
    vertexBufferData.pSysMem = pVerts;
    device->CreateBuffer(&desc, &vertexBufferData, &vertexBuffer);

    //Initialise constant buffer - used to efficiently supply shader constants
    //to the pipeline.
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = 1024; //sizeof(cbufferData) + ((16 - sizeof(cbufferData))%16)
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    device->CreateBuffer(&desc, NULL, &constantBuffer);

    //create vertex shader
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = CompileShader(vertexShaderPath, "main", "vs_5_0", &vsBlob);
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
    hr = CompileShader(pixelShaderPath, "main", "ps_5_0", &psBlob);
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
        device->CreateInputLayout(inputElementDesc, 2, vsBlob, sizeof(vsBlob), &inputLayout);
    }
    // render states (below is all boilerplate code)
    D3D11_RASTERIZER_DESC rsdesc;
    memset(&rsdesc, 0, sizeof(rsdesc));
    rsdesc.FillMode = D3D11_FILL_SOLID;
    rsdesc.CullMode = D3D11_CULL_NONE;
    rsdesc.DepthClipEnable = TRUE;
    device->CreateRasterizerState(&rsdesc, &rasterizerState);

    D3D11_DEPTH_STENCIL_DESC dsdesc;
    memset(&dsdesc, 0, sizeof(dsdesc));
    dsdesc.DepthEnable = TRUE;
    dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsdesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;// 
    device->CreateDepthStencilState(&dsdesc, &depthStencilState);

    D3D11_BLEND_DESC bdesc;
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.RenderTarget[0].BlendEnable = FALSE;
    bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
    device->CreateBlendState(&bdesc, &blendState);
}

void D3D11RendererAPI::ReleaseResources()
{
    this->verts = verts;
    if (device) device->Release();
    if(context)context->Release(); 
    if(vertexBuffer)vertexBuffer->Release();
    if(vertexShaderBlob)vertexShaderBlob->Release();
    if (constantBuffer)constantBuffer->Release();
    if(vertexShader)vertexShader->Release();
    if(pixelShader)pixelShader->Release();
    if(inputLayout)inputLayout->Release();
    if(rasterizerState)rasterizerState->Release();
    if(blendState)blendState->Release();
    if(depthStencilState)depthStencilState->Release();
}
void D3D11RendererAPI::Draw(ConstantBufferData cbData)
{
    //Get the device context, which allows us to actually issue rendering commands
    ID3D11DeviceContext* context = NULL;
    device->GetImmediateContext(&context);

    //Set basic render state
    context->OMSetDepthStencilState(depthStencilState, 0);
    context->RSSetState(rasterizerState);
    context->OMSetBlendState(blendState, NULL, 0xFFFFFFFF);


    //Update the constant buffer with all our shader uniforms
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    //The Map function gets a pointer to the data contained in a subresource
    //(ie, our constant buffer), and denies the GPU access to that subresource.
    //
    HRESULT hr = context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    //We now copy the data within cbData into the pData field of our mapped sub resource
    CopyMemory(mappedResource.pData, &cbData, sizeof(ConstantBufferData));
    //We now unmap the subresource to allow the gpu to access it once more
    context->Unmap(constantBuffer, 0);

    //Set shaders
    context->VSSetConstantBuffers(0, 1, &constantBuffer);
    context->VSSetShader(vertexShader, NULL, 0);
    context->PSSetShader(pixelShader, NULL, 0);

    //Set input assembler data
    context->IASetInputLayout(inputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    UINT stride = sizeof(MeshVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->Draw(verts.size(), (UINT)0);
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
        break;
    }
    }
}