#include "roApplication.h"
#include "roWindow.h"
#include "roEngineConfig.h"
#include "..\Graphics\roRootGraphics.h"
#include "..\Graphics\roGraphicsComponent.h"
#include "roProfiler.h"
#include "..\Graphics\GUI\roGraphicsUI.h"
#include "..\Core\roEventSystem.h"
#include "roTimer.h"
#include "..\User Mode\roUserComponent.h"

bool roApplication::Initialize(HINSTANCE hInstance) {

	InitializeEngineConfigurations();
	roRootGraphics::EnumerateDisplaySettings();
	if (!roWindow::Initialize(hInstance, WindowProcedureFunc, 1280, 800)) 
		return false;

	using namespace RingedOrb;

	sm_CPUProfiler = new roCPUProfiler();
	RingedOrb::InitializeCPUProfiles();

	sm_EventQueueSystem = new roEventQueue();

	if (!CreateGraphicsComponent(sm_EventQueueSystem, sm_CPUProfiler)) return false;

	roEngineConfig::sm_EngineAcitiveState = true;
	roEngineConfig::sm_CachedActiveState  = true;

	roTimer::Initialize();
	sm_UserComponent = new roUserComponent(sm_EventQueueSystem);

	return true;
}

bool roApplication::CreateGraphicsComponent(roEventQueue* eventQueue, void* cpuProfiler) {
	sm_GraphicsComponent = new roGraphicsComponent();
	sm_GraphicsComponent->Initialize(eventQueue, (RingedOrb::roCPUProfiler*)cpuProfiler);
	
	return true;
}

void roApplication::InitializeEngineConfigurations() {
	roEngineConfig::sm_EngineName			= L"RingedOrb";

	roEngineConfig::sm_FullScreen			= false;
	roEngineConfig::sm_Vsync				= false;
	roEngineConfig::sm_EngineAcitiveState	= false;

}

int roApplication::Run() {
	
	MSG msg{};

	while (WM_QUIT != msg.message) {
		using namespace RingedOrb;

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			roTimer::Update();
			if (roEngineConfig::sm_EngineAcitiveState) {

				sm_UserComponent->Update();

				sm_EventQueueSystem->DispatchEvents();

				//auto cpuProfiler = (roCPUProfiler*)sm_CPUProfiler;

 				sm_GraphicsComponent->Update();
				sm_GraphicsComponent->Render();
 			}
		}
	}
	
	return 1;
}

void roApplication::Shutdown() {	
	
	sm_GraphicsComponent->Shutdown();
	sm_GraphicsComponent = nullptr;
	roWindow::Shutdown(sm_Hinstance);
	sm_Hinstance = 0;

	auto cpuProfile = (RingedOrb::roCPUProfiler*)sm_CPUProfiler;
	SAFE_DELETE(cpuProfile);
	SAFE_DELETE(sm_EventQueueSystem);
	SAFE_DELETE(sm_UserComponent);
}

LRESULT roApplication::AppWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static int windowWidth = 0; 
	static int windowHeight = 0; 

	ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);

	switch (message) {
		
	case WM_SIZE:
		windowWidth = ((WORD)(((DWORD_PTR)(lParam)) & 0xffff));
		windowHeight = ((WORD)((((DWORD_PTR)(lParam)) >> 16) & 0xffff));
		
		if (windowHeight > 0 && windowWidth > 0) {
			roWindow::sm_Width	= windowWidth;
			roWindow::sm_Height = windowHeight;
		}		

		if (roEngineConfig::sm_EngineAcitiveState) {
			if (wParam == SIZE_MINIMIZED) {
				roEngineConfig::sm_EngineAcitiveState = false;
				roWindow::sm_Minimized = true;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				roWindow::sm_Minimized = false;
				if (roEngineConfig::sm_CachedActiveState) {
					roEngineConfig::sm_EngineAcitiveState = true;
					sm_GraphicsComponent->OnResize();
				}
			}
			else if (wParam == SIZE_RESTORED) {
				if (roWindow::sm_Minimized) {
					roWindow::sm_Minimized = false;
					if (roEngineConfig::sm_CachedActiveState) {
						roEngineConfig::sm_EngineAcitiveState = true;
						sm_GraphicsComponent->OnResize();
					}
				}
				else if (roWindow::sm_Minimized == false) {
					if (roEngineConfig::sm_CachedActiveState) {
						roEngineConfig::sm_EngineAcitiveState = true;
						sm_GraphicsComponent->OnResize();
					}
				}
			}
		}
		break;
	case WM_ACTIVATE:
		if (((WORD)(((DWORD_PTR)(wParam)) & 0xffff)) == WA_INACTIVE) {
			if (roEngineConfig::sm_CachedActiveState)
				roEngineConfig::sm_EngineAcitiveState = false;
		}
		else {
			roEngineConfig::sm_EngineAcitiveState = true;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


LRESULT WindowProcedureFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return roApplication::AppWindowProcedure(hWnd, msg, wParam, lParam);
}

HINSTANCE roApplication::sm_Hinstance						= NULL;
roGraphicsComponent* roApplication::sm_GraphicsComponent	= nullptr;
void* roApplication::sm_CPUProfiler							= nullptr;
roEventQueue* roApplication::sm_EventQueueSystem			= nullptr;
roUserComponent* roApplication::sm_UserComponent = nullptr;

