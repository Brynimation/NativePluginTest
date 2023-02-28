#include "Graphics.h"

Graphics::Graphics(ID3D11Device* device) 
{
    this->device = device;
}
bool Graphics::InitialiseDirectD3D11() 
{
    return true;
}

bool Graphics::InitialiseShaders() 
{
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    UINT numElements = ARRAYSIZE(inputElementDesc);
    if (!vertexShader.Initialise(device, vertexShader.shaderFileName, inputElementDesc, numElements)) 
    {
        return false;
    }
    if (!pixelShader.Initialise(device, pixelShader.shaderFileName)) 
    {
        return false;
    }
}
bool Graphics::InitialiseScene() 
{
    //Initialise vertex buffer
    MeshVertex vert1;
    MeshVertex vert2;
    MeshVertex vert3;
    vert1.pos[0] = 0.5;
    vert1.pos[1] = 0.5;
    vert1.pos[2] = 0.5;
    vert1.colour[0] = 1.0;
    vert1.colour[1] = 1.0;
    vert1.colour[2] = 1.0;
    vert1.colour[3] = 1.0;
    MeshVertex verts[1]
    {
        vert1
    };
    D3D11_BUFFER_DESC vertexBufferDesc; //STRUCT: describes a buffer resource
    //&desc gets the memory address at which desc is stored
    memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc)); 
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(MeshVertex) * ARRAYSIZE(verts); //size of vertex buffer in bytes - sizeof(vertex) * arraySize'
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //bind current buffer to the vertex buffer
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    //populate the vertex buffer with our vertices from unity
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    MeshVertex* pVerts = &verts[0]; //Get pointer to the source vertices
    vertexBufferData.pSysMem = pVerts;
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

    if (FAILED(hr)) 
    {
        return false;
    }
    return true;
}

void Graphics::Draw() 
{
  
    device->GetImmediateContext(&context);
    context->VSSetShader(vertexShader.vertexShader, NULL, 0);
    context->PSSetShader(pixelShader.pixelShader, NULL, 0);

    //Set input assembler data
    context->IASetInputLayout(vertexShader.inputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    UINT stride = sizeof(MeshVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    //ctx->UpdateSubresource(vertexBuffer, 0, NULL, verts, triangleCount * 3 * kVertexSize, 0);
    context->Draw(sizeof(vertexBuffer), (UINT)0);
}

//create vertex buffer
//Draw  ASX