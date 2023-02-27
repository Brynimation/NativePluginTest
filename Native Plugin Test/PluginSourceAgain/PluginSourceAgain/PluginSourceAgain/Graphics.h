#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"
#include <d3dcompiler.h>
#include "Data.cpp"
#include "Shaders.h"

class Graphics {
public:
	Graphics(ID3D11Device* device);
	bool InitialiseDirectD3D11();
	bool InitialiseShaders();
	bool InitialiseScene();
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTarget;
	ID3D11InputLayout* inputLayout;
	VertexShader vertexShader;
	PixelShader pixelShader;
	ID3D11Buffer* vertexBuffer;
};