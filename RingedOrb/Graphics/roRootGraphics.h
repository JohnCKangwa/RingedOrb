#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"

using namespace Microsoft::WRL;

class roImageBuffer;
class roDepthBuffer;
struct roApplication;
struct roGraphicsUI;

struct FrameContext {
	uint64_t FenceValue = 0;
};

struct roRootGraphics {
	friend roApplication;
	friend roGraphicsUI;

	
	static bool Initialize(int FrameCount);
	static bool OnResize();
	static roImageBuffer* GetCurrentBackBuffer();
	static roDepthBuffer* GetDepthBuffer();
	static void Present();
	static void DestroyFrameBuffers();
	static void Shutdown();

private:
	static void LoadDefaultDisplaySettings();
	static void EnumerateDisplaySettings();
	static IDXGIAdapter1* GetGraphicsAdapter(IDXGIFactory4* factory);
	static bool CreateDevice();
	static bool CreateSwapChain();
public:
	static ID3D12Device* sm_Device;

private:
	static ComPtr<IDXGISwapChain> sm_DXGISwapChain;
	static ComPtr<IDXGIFactory4> sm_DXGIFactory;
	static ComPtr<IDXGIAdapter1> sm_DXGIAdapter;

public:
	static int sm_FrameCount;
	static int sm_CurrentFrameIndex;
	static FrameContext* sm_FrameContext;

private:
	static DXGI_FORMAT sm_RenderTargetFormat;
	static roImageBuffer* sm_SwapChainBuffers;
	static roDepthBuffer* sm_DepthBuffer;

	static std::vector<DXGI_MODE_DESC> sm_DisplayModes;
	static DXGI_MODE_DESC sm_CurrentDisplayMode;

};