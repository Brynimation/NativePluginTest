#include "Shaders.h"

bool VertexShader::Initialise(ID3D11Device* device, LPCWSTR shaderFileName, D3D11_INPUT_ELEMENT_DESC inputElementDesc[], UINT numElements)
{
	LPCWSTR shaderFolder = L"";
#ifdef _DEBUG 
	shaderFolder = L"..\\x64\\Debug\\"; //shader path of vertex shader in debug mode
#else  
	shaderFolder = L"..\\x64\\Release\\"; //shader path of vertex shader in build mode
#endif
	std::wstring fullPath = std::wstring(shaderFolder) + std::wstring(shaderFileName); //have to convert the LPCWSTRs to wstring to concatenate
	LPCWSTR fullPathLong = fullPath.c_str(); //c_str() converts the wstrings back to LPCWSTRs
	HRESULT hr = D3DReadFileToBlob(fullPathLong, &shaderBuffer); //Reads the compiled shader bytecode from a file to the shader buffer.
	if (FAILED(hr)) 
	{
		return false;
	}
	hr = device->CreateVertexShader(shaderBuffer, sizeof(shaderBuffer), nullptr, &vertexShader);//store all the data in the shader buffer in the vertexShader
	if (FAILED(hr)) 
	{
		return false;
	}
	hr = device->CreateInputLayout(inputElementDesc, numElements, shaderBuffer, sizeof(shaderBuffer), &inputLayout); //define the structure output by the input assembler and input to the vertex shader
	if (FAILED(hr)) 
	{
		return false;
	}
	return true;
}

bool PixelShader::Initialise(ID3D11Device* device, LPCWSTR shaderFileName) 
{
	LPCWSTR shaderFolder = L"";
#ifdef _DEBUG 
	shaderFolder = L"..\\x64\\Debug\\"; //shader path of pixel shader in debug mode
#else  
	shaderFolder = L"..\\x64\\Release\\"; //shader path of pixel shader in build mode
#endif
	std::wstring fullPath = std::wstring(shaderFolder) + std::wstring(shaderFileName); //have to convert the LPCWSTRs to wstring to concatenate
	LPCWSTR fullPathLong = fullPath.c_str(); //c_str() converts the wstrings back to LPCWSTRs
	HRESULT hr = D3DReadFileToBlob(fullPathLong, &shaderBuffer); //Reads the compiled shader bytecode from a file to the shader buffer.
	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreatePixelShader(shaderBuffer, sizeof(shaderBuffer), nullptr, &pixelShader);//store all the data in the shader buffer in the vertexShader
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
