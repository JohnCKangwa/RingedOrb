#include "roGraphicsUI.h"
#include "..\..\Core\roWindow.h"
#include "..\roRootGraphics.h"
#include "ImGui_Library\imgui.h"
#include "ImGui_Library\imgui_impl_win32.h"
#include "ImGui_Library\imgui_impl_dx12.h"
#include "..\..\Core\roProfiler.h"
#include "..\..\Core\roTimer.h"
#include "roGUIData.h"

void roGraphicsUI::Initialize(int frameCount) {

	D3D12_DESCRIPTOR_HEAP_DESC fontHeapDesc;
	fontHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	fontHeapDesc.NodeMask		= 0;
	fontHeapDesc.NumDescriptors = frameCount;
	fontHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	roRootGraphics::sm_Device->CreateDescriptorHeap(&fontHeapDesc, IID_PPV_ARGS(&sm_FontHeap));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui_ImplWin32_Init(roWindow::GetWindowHandle());
	ImGui_ImplDX12_Init(roRootGraphics::sm_Device, frameCount, roRootGraphics::sm_RenderTargetFormat, sm_FontHeap->GetCPUDescriptorHandleForHeapStart(),
		sm_FontHeap->GetGPUDescriptorHandleForHeapStart());

	ImGui::StyleColorsDark();
}

void roGraphicsUI::Update() {
	using namespace RingedOrb;

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static float cpuFrameTimeValue;
	static float gpuFrameTimeValue;
	static float cpuFrameTimeValueHigh;
	static float gpuFrameTimeValueHigh;

	static std::string gpuFrameTime		= "GPU Frame Time: 0.000000";
	static std::string cpuFrameTime		= "CPU Frame Time: 0.000000";
	static std::string drawCount		= "Draw Calls    : ";
	static std::string gpuFrameTimeHigh = "GPU High      :";
	static std::string cpuFrameTimeHigh = "CPU High      : ";	

	static float elapsedTime = .0f;

	if (roTimer::TotalTime() - elapsedTime > .10f) {
		elapsedTime = roTimer::TotalTime();
		
		cpuFrameTimeValue = g_CPUProfiles[PROFILE_INFO_TYPE_CPU_FRAME_TIME].TimeElapsed * 1000.0f;
		cpuFrameTime = "CPU Frame Time: " + std::to_string(cpuFrameTimeValue) + " ms";

		gpuFrameTimeValue = g_GPUProfiles[PROFILE_INFO_TYPE_GPU_FRAME_TIME].TimeElapsed * 1000.0f;
		gpuFrameTime = "GPU Frame Time: " + std::to_string(g_GPUProfiles[PROFILE_INFO_TYPE_GPU_FRAME_TIME].TimeElapsed * 1000.0f) + " ms";

		if (cpuFrameTimeValue > cpuFrameTimeValueHigh)
			cpuFrameTimeValueHigh = cpuFrameTimeValue;

		if (gpuFrameTimeValue > gpuFrameTimeValueHigh)
			gpuFrameTimeValueHigh = gpuFrameTimeValue;


		cpuFrameTimeHigh = "CPU Frame High: " + std::to_string(cpuFrameTimeValueHigh);
		gpuFrameTimeHigh = "GPU Frame High: " + std::to_string(gpuFrameTimeValueHigh);
	}

	ImGui::SetNextWindowPos(ImVec2(7, 10));
	ImGui::Begin("Profiles", false, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav));
	ImGui::Text(gpuFrameTime.c_str());
	ImGui::Text(cpuFrameTime.c_str());
	//ImGui::Text(cpuFrameTimeHigh.c_str());
	ImGui::Text(drawCount.c_str());
	ImGui::Text("Z-Prepass     : 0.000");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::Text("");
	ImGui::End();

	
	ImGui::Begin("Editor Panel", false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	bool bModelOpnDiag = false;
	bModelOpnDiag = ImGui::Button("Load Model");

	static int selected = 0;
	ImGui::BeginChild("meshes", ImVec2(192, 100), true);	
	for (int i = 0; i < roGUIData::sm_MeshData.size(); i++) {
		auto &modelDat = roGUIData::sm_MeshData[i];

		if (ImGui::Selectable(modelDat.first.c_str(), selected == i)) {
			selected = i;
			roGUIData::sm_MeshData[i].second = true;
		}
		if (selected != i) {
			roGUIData::sm_MeshData[i].second = false;
		}
		if (selected == i) {
			roGUIData::sm_MeshData[i].second = true;
		}
	}
	ImGui::EndChild();


	roGUIData::SetOpenModelDialogState(bModelOpnDiag);
	if (ImGui::CollapsingHeader("REntities")) {
		bool bAddEntity = false;
		bAddEntity = ImGui::Button("Add  ");
		roGUIData::SetAddEnityState(bAddEntity);
		ImGui::SameLine();
		ImGui::Button("Delete");
	}
		
	ImGui::End();
}

void roGraphicsUI::RenderOverlay(ID3D12GraphicsCommandList* commandList) {
	Update();
	commandList->SetDescriptorHeaps(1, &sm_FontHeap);
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void roGraphicsUI::Shutdown() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SAFE_RELEASE(sm_FontHeap)
}