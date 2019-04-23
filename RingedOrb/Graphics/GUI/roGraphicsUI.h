#pragma once
#include "..\DX12IncludeFile.h"
#include "..\..\Core\pch.h"

using namespace Microsoft::WRL;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct roGraphicsUI {

	void Initialize(int frameCount);
	void RenderOverlay(ID3D12GraphicsCommandList* commandList);
	void Shutdown();

private:
	void Update();
private:
	ID3D12DescriptorHeap* sm_FontHeap;
};