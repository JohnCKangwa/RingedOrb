#pragma once
#include"pch.h"
#include <string.h>

struct roWindow {
	friend struct roApplication;

	static bool Initialize(HINSTANCE hInstance, void* wndProc, int clientWidth, int clientHeight);
	static void SetWindowDimensions(int width, int height);

	static HWND GetWindowHandle();
	static int GetHeight();
	static int GetWidth();
	static void Shutdown(HINSTANCE hInstance);


private:
	static HWND sm_WindowHandle;
	static int sm_Width;
	static int sm_Height;
	static bool sm_Minimized;
};

