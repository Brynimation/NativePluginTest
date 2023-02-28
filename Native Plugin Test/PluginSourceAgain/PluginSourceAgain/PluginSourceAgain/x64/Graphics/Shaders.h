#pragma once
#include "d3d11.h"
#include <d3dcompiler.h>
#include <filesystem>
#pragma comment (lib, "D3DCompiler.lib")

class VertexShader
{
public:
	bool Initialise(ID3D11 &device, std::string shaderpath);
	std::wstring shaderFileName = L"VertexShader.cso";
	ID3D11VertexShader* vertexShader;
	ID3DBlob* shaderBuffer;
};
