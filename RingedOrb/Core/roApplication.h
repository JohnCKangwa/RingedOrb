#pragma once
#include "roWindow.h"


extern LRESULT CALLBACK WindowProcedureFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class roGraphicsComponent;
class roEventQueue;
class roUserComponent;

struct roApplication {

	static bool Initialize(HINSTANCE hInstance);
	static bool CreateGraphicsComponent(roEventQueue* eventQueue, void* cpuProfiler);
	static int Run();
	static LRESULT AppWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void Shutdown();

private:
	static void InitializeEngineConfigurations();

private:
	static HINSTANCE sm_Hinstance;
	static roGraphicsComponent* sm_GraphicsComponent;
	static roEventQueue* sm_EventQueueSystem;
	static void* sm_CPUProfiler;
	static roUserComponent* sm_UserComponent;
};
