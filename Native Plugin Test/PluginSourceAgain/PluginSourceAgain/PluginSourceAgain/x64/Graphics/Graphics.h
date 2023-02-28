#pragma once
#include "IUNITYGRAPHICS.H"
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"
#include <d3dcompiler.h>
#include <wrl/async.h> //gives access to CompPtrs.

class Graphics {
public:
	bool InitialiseDirectD3D11();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

};