#include "roWindow.h"
#include "..\resource.h"
#include "roEngineConfig.h"

bool roWindow::Initialize(HINSTANCE hInstance, void* wndProc, int clientWidth, int clientHeight) {
	
	SetWindowDimensions(clientWidth, clientHeight);

	WNDCLASSEX wcex		= {};
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC/*CS_CLASSDC*/;
	wcex.lpfnWndProc	= (WNDPROC)wndProc;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon			= static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName	= roEngineConfig::sm_EngineName.c_str();
	wcex.hIconSm		= static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

	if (!RegisterClassEx(&wcex)) {
		MessageBox(0, L"Could not register window", L"Error", 0);
		return false;
	}

	RECT windowRect = { 0,0,static_cast<LONG>(clientWidth), static_cast<LONG>(clientHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	sm_WindowHandle = CreateWindowEx(0, roEngineConfig::sm_EngineName.c_str(), L"RingedOrb", WS_OVERLAPPEDWINDOW, 10, 10,
		/*windowRect.right - windowRect.left*/clientWidth, /*windowRect.bottom - windowRect.top*/clientHeight, NULL, NULL, hInstance, NULL);

	if (!sm_WindowHandle) {
		MessageBox(0, L"Could not create window", L"Error", 0);
		return false;
	}

	ShowWindow(sm_WindowHandle, SW_SHOWDEFAULT);
	UpdateWindow(sm_WindowHandle);

	return true;
}

void roWindow::SetWindowDimensions(int width, int height) {
	sm_Width = width; sm_Height = height;
}

HWND roWindow::GetWindowHandle() {
	return sm_WindowHandle;
}

int roWindow::GetHeight() {
	return sm_Height;
}

int roWindow::GetWidth() {
	return sm_Width;
}

void roWindow::Shutdown(HINSTANCE hInstance) {
	DestroyWindow(sm_WindowHandle);
	sm_WindowHandle = NULL;
	UnregisterClass(roEngineConfig::sm_EngineName.c_str(), hInstance);
}

HWND roWindow::sm_WindowHandle;
int roWindow::sm_Width;
int roWindow::sm_Height;
bool roWindow::sm_Minimized;
