#include "Graphics.h"

Graphics::Graphics(ID3D11Device* device) 
{
    this->device = device;
}
bool Graphics::InitialiseDirectD3D11() 
{

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
    vert1.pos[0] = 0.5;
    vert1.pos[1] = 0.5;
    vert1.pos[2] = 0.5;
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

//create vertex buffer
//Draw