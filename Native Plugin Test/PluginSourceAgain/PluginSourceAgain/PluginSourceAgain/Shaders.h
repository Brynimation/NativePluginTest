#pragma once
#include "d3d11.h"
#include <d3dcompiler.h>
#include <filesystem>
#pragma comment(lib, "D3DCompiler.lib")

class VertexShader 
{
public:
	bool Initialise(ID3D11Device *device, LPCWSTR shaderPath, D3D11_INPUT_ELEMENT_DESC inputElementDesc[], UINT numElements);
	LPCWSTR shaderFileName = L"VertexShader.cso";
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
	ID3DBlob* shaderBuffer;
};

class PixelShader 
{
public:
	bool Initialise(ID3D11Device* device, LPCWSTR shaderPath);
	LPCWSTR shaderFileName = L"PixelShader.cso";
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3DBlob* shaderBuffer;
};