#include "roRootGraphics.h"
#include "..\Core\roErrorHandling.h"
#include "..\Core\roEngineConfig.h"
#include "roImageBuffer.h"
#include "roDepthBuffer.h"
#include "roCommandQueue.h"
#include "roDescriptorHeapManager.h"
#include "..\Core\roWindow.h"

UINT g_DxgiFactoryFlag = 0;

void roRootGraphics::EnumerateDisplaySettings() {
	using namespace Microsoft::WRL;

	//temporary adapter(naive approach :-( )
	IDXGIFactory4* pFactory = nullptr;
	IDXGIAdapter1* pAdapter = nullptr;
	IDXGIOutput* pOutput = nullptr;


	if (roEngineConfig::sm_EngineAcitiveState)
		return;

	HR(CreateDXGIFactory( IID_PPV_ARGS(&pFactory)));

	DXGI_ADAPTER_DESC1 adapterDesc;
	pAdapter = GetGraphicsAdapter(pFactory);
	pAdapter->GetDesc1(&adapterDesc);


	if (pAdapter == nullptr) {
		THROW(L"Adapter error");
	}

	if (adapterDesc.Flags == DXGI_ADAPTER_FLAG_SOFTWARE) {

		DXGI_MODE_DESC dxgi_desc;
		dxgi_desc.RefreshRate.Numerator = 60;
		dxgi_desc.RefreshRate.Denominator = 1;

		SAFE_RELEASE(pAdapter);
		SAFE_RELEASE(pFactory);

		return;
	}

	HR(pAdapter->EnumOutputs(0, &pOutput));
	
	UINT numDisplayModes = 0;
	HR(pOutput->GetDisplayModeList(sm_RenderTargetFormat, DXGI_ENUM_MODES_INTERLACED,
		&numDisplayModes, nullptr)); 

	DXGI_MODE_DESC *modes = new DXGI_MODE_DESC[numDisplayModes];

	sm_DisplayModes.resize(numDisplayModes);
	HR(pOutput->GetDisplayModeList(sm_RenderTargetFormat, DXGI_ENUM_MODES_INTERLACED,
		&numDisplayModes, modes));

	for (uint32_t i = 0; i < numDisplayModes; i++) {
		sm_DisplayModes.push_back(modes[i]);
	}
	delete[] modes;

	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pOutput);
	SAFE_RELEASE(pFactory);
}

IDXGIAdapter1* roRootGraphics::GetGraphicsAdapter(IDXGIFactory4* factory) {

		IDXGIAdapter1* pAdapter = nullptr;

		for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &pAdapter); i++) {
			DXGI_ADAPTER_DESC1 desc;
			HR(pAdapter->GetDesc1(&desc));
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
			SAFE_RELEASE(pAdapter);
		}

		if (!pAdapter) {

			static bool roDisplayedMsg = false;
			
			if (!roDisplayedMsg) {
				MessageBox(0, L"This device doesnt support minimum dx12 features, A software device will be used and will be extremely slow",
					L"Device not support x12", 0);
				roDisplayedMsg = true;
			}

			for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &pAdapter); i++) {
				DXGI_ADAPTER_DESC1 desc;
				HR(pAdapter->GetDesc1(&desc));

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
					HR(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
					break;
				}
				SAFE_RELEASE(pAdapter);
			}

		}

		return pAdapter;
	}

void roRootGraphics::LoadDefaultDisplaySettings() {
	DEVMODE current_screen_settings;

	ZeroMemory(&current_screen_settings, sizeof(DEVMODE));
	current_screen_settings.dmSize = sizeof(current_screen_settings);
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &current_screen_settings) != 0) {
		for (auto iter : sm_DisplayModes) {
			if (iter.Height == current_screen_settings.dmPelsHeight &&
				iter.Width == current_screen_settings.dmPelsWidth) {
				sm_CurrentDisplayMode = iter;
				return;
			}
		}
	}

}

bool roRootGraphics::CreateDevice() {
	
#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugInterface.GetAddressOf()))))
		debugInterface->EnableDebugLayer();
	else
		return false;

	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) {

		g_DxgiFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	HR(CreateDXGIFactory2(g_DxgiFactoryFlag, IID_PPV_ARGS(sm_DXGIFactory.ReleaseAndGetAddressOf())));

	sm_DXGIAdapter.Reset();
	sm_DXGIAdapter.Attach(GetGraphicsAdapter(sm_DXGIFactory.Get()));

	HR(D3D12CreateDevice(sm_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&sm_Device)));

	return true;
}

bool roRootGraphics::CreateSwapChain() {
	sm_DXGISwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC1 f;
	
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width						= roWindow::GetWidth();
	scd.BufferDesc.Height						= roWindow::GetHeight();

	if (roEngineConfig::sm_Vsync) {
		scd.BufferDesc.RefreshRate.Numerator	= sm_CurrentDisplayMode.RefreshRate.Numerator;
		scd.BufferDesc.RefreshRate.Denominator	= sm_CurrentDisplayMode.RefreshRate.Denominator;
	}
	else {
		scd.BufferDesc.RefreshRate.Numerator	= 0;
		scd.BufferDesc.RefreshRate.Denominator	= 1;
	}

	scd.BufferDesc.Format						= sm_RenderTargetFormat;
	scd.BufferDesc.ScanlineOrdering				= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling						= DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count						= 1;
	scd.SampleDesc.Quality						= 0;
	scd.BufferUsage								= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount								= sm_FrameCount;
	scd.OutputWindow							= roWindow::GetWindowHandle();
	if (roEngineConfig::sm_FullScreen)
		scd.Windowed							= false;
	else
		scd.Windowed							= true;
	scd.SwapEffect								= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	scd.Flags									= 0;


	auto cmdQueue = roCommandQueue::GetCmdQueue();
	HR(sm_DXGIFactory->CreateSwapChain(cmdQueue, &scd, &sm_DXGISwapChain));

	sm_DXGISwapChain->SetFullscreenState(FALSE, 0);

	
	return true;
}

bool roRootGraphics::Initialize(int frameCount) {

	sm_FrameCount			= frameCount;
	sm_FrameContext			= new FrameContext[sm_FrameCount];
	sm_SwapChainBuffers		= nullptr;

	CreateDevice();


	roCommandQueue::Initialize();
	int swap_result = CreateSwapChain();
	roDescriptorHeapManager::Initialize();

	OnResize();

	return true;
}

bool roRootGraphics::OnResize() {

	assert(sm_DXGISwapChain);
	LoadDefaultDisplaySettings();

	sm_CurrentFrameIndex = 0;
	for (int i = 0; i < sm_FrameCount; i++)
		sm_FrameContext[i].FenceValue = 0;

	sm_DXGISwapChain->ResizeBuffers(sm_FrameCount, roWindow::GetWidth(),
		roWindow::GetHeight(), sm_RenderTargetFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	DestroyFrameBuffers();
	sm_SwapChainBuffers = new roImageBuffer[sm_FrameCount];

	ID3D12Resource* backbuffer;

	for (int i = 0; i < sm_FrameCount; i++) {
		sm_DXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));
		sm_SwapChainBuffers[i].CreateImageFromSwapChain(backbuffer);
		SAFE_RELEASE(backbuffer);
	}

	sm_DepthBuffer = new roDepthBuffer();
	sm_DepthBuffer->CreateDepthBuffer(roWindow::GetWidth(),
		roWindow::GetHeight(), 1, 1, 1, 0);

	return true;
}

roImageBuffer* roRootGraphics::GetCurrentBackBuffer() {
	return &sm_SwapChainBuffers[sm_CurrentFrameIndex];
}

roDepthBuffer* roRootGraphics::GetDepthBuffer() {
	return sm_DepthBuffer;
}

void roRootGraphics::Present() {
	if (roEngineConfig::sm_Vsync)
		sm_DXGISwapChain->Present(1, 0);
	else
		sm_DXGISwapChain->Present(0, 0);
}

void roRootGraphics::DestroyFrameBuffers() {

	if (sm_SwapChainBuffers) {
		for (int i = 0; i < sm_FrameCount; i++)
			sm_SwapChainBuffers[i].DiscardResource(true);
	}
	SAFE_DELETE_ARRAY(sm_SwapChainBuffers);

	if (sm_DepthBuffer) {
		sm_DepthBuffer->DiscardResource();
		SAFE_DELETE(sm_DepthBuffer);
	}
}

void roRootGraphics::Shutdown() {

	delete[] sm_FrameContext;

	DestroyFrameBuffers();

	roCommandQueue::Shutdown();
	sm_DXGISwapChain.Reset();
	roDescriptorHeapManager::Shutdown();

	sm_DXGIAdapter.Reset();
	sm_DXGIFactory.Reset();


#ifdef _DEBUG
	{
		SAFE_RELEASE(sm_Device)

			ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#else
	{
		SAFE_RELEASE(sm_Device)
	}
#endif
}

ID3D12Device* roRootGraphics::sm_Device;

ComPtr<IDXGISwapChain> roRootGraphics::sm_DXGISwapChain;
ComPtr<IDXGIFactory4> roRootGraphics::sm_DXGIFactory;
ComPtr<IDXGIAdapter1> roRootGraphics::sm_DXGIAdapter;

int roRootGraphics::sm_FrameCount;
int roRootGraphics::sm_CurrentFrameIndex;
FrameContext* roRootGraphics::sm_FrameContext;

DXGI_FORMAT roRootGraphics::sm_RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
roImageBuffer* roRootGraphics::sm_SwapChainBuffers = nullptr;
roDepthBuffer* roRootGraphics::sm_DepthBuffer;

std::vector<DXGI_MODE_DESC> roRootGraphics::sm_DisplayModes;
DXGI_MODE_DESC roRootGraphics::sm_CurrentDisplayMode;
