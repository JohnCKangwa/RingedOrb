#include "roGraphicsComponent.h"
#include "roGPUContextManager.h"
#include "roGPUContext12.h"
#include "roCommandAllocator.h"
#include "roImageBuffer.h"
#include "roDepthBuffer.h"
#include "roRootGraphics.h"
#include "..\Core\roWindow.h"
#include "roCommandQueue.h"
#include "roDescriptorHeapManager.h"
#include "GUI\roGraphicsUI.h"
#include "roRenderer.h"
#include "roGraphicsResources.h"
#include "..\Core\roProfiler.h"
#include "..\Core\roTimer.h"
#include "roConstantRingBuffer.h"
#include "roGraphicsManager.h"

using namespace RingedOrb;

roGraphicsComponent::roGraphicsComponent() {

}

void roGraphicsComponent::Initialize(roEventQueue* eventQueue, RingedOrb::roCPUProfiler* cpuProfiler) {
	m_pCpuProfiler = cpuProfiler;
	sm_IsCreated = true;
	int frameCount = 3;

	roRootGraphics::Initialize(frameCount);

	m_pGraphicsUI = new roGraphicsUI();
	m_pGraphicsUI->Initialize(frameCount);
	RingedOrb::InitializeGPUProfiles(roRootGraphics::sm_Device, frameCount);

	m_pConstantBufferRing = nullptr;
	InitializeResources();

	roGraphicsRenderer::Initialize(m_pConstantBufferRing);

	m_pGraphicsManager = new roGraphicsManager(eventQueue);

}

void roGraphicsComponent::InitializeResources() {
	roGraphicsResourceLoader::LoadDefaults();
	//ring buffer for constant buffer
	UINT64 frameCount = roRootGraphics::sm_FrameCount;
	UINT64 maxCBsize = 256UL;
	UINT64 aproxNumCBs = 50000UL;
	UINT64 fixedBlockSize = 0UL;

	m_pConstantBufferRing = new roConstantRingBuffer(fixedBlockSize, maxCBsize * aproxNumCBs * frameCount);
}

void roGraphicsComponent::OnResize() {
	if (sm_IsCreated) {
		FlushGPU();
		roRootGraphics::OnResize();
	}
}


void roGraphicsComponent::Update() {

	roGraphicsRenderer::UpdateMainPassConstant();
	roGraphicsRenderer::UpdateRenderableEntities();

}

void roGraphicsComponent::Render() {

	m_pCpuProfiler->Begin(PROFILE_INFO_TYPE_CPU_FRAME_TIME);


	auto currbuffer = roRootGraphics::GetCurrentBackBuffer();
	auto depthbuffer = roRootGraphics::GetDepthBuffer();
	auto gpuContext = roGPUContextManager::GetGPUContext("something");



	gpuContext->BeginProfile(PROFILE_INFO_TYPE_GPU_FRAME_TIME);
	gpuContext->TransitionResource(currbuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	gpuContext->ClearRenderTarget(currbuffer);
	gpuContext->ClearDepthStencil(depthbuffer);
	gpuContext->SetRenderTargets(currbuffer, depthbuffer);
	gpuContext->SetDefaultViewport();
	gpuContext->SetScissorRect();

	roGraphicsRenderer::Draw(gpuContext);

	m_pGraphicsUI->RenderOverlay(gpuContext->GetCommandList());

	gpuContext->TransitionResource(currbuffer, D3D12_RESOURCE_STATE_PRESENT);
	gpuContext->EndProfile(PROFILE_INFO_TYPE_GPU_FRAME_TIME, roRootGraphics::sm_CurrentFrameIndex);
	gpuContext->Finish(true);



	roRootGraphics::Present();
	uint64_t fenceValue = gpuContext->Signal();

	m_pCpuProfiler->End(PROFILE_INFO_TYPE_CPU_FRAME_TIME);

	WaitForTailFrame();

	MoveToNextFrame(fenceValue);

}

void roGraphicsComponent::MoveToNextFrame(uint64_t CurrFramefenceValue) {

	int currFrameIdx = roRootGraphics::sm_CurrentFrameIndex;
	m_pConstantBufferRing->MarkEndOfFrame(currFrameIdx);
	RingedOrb::g_RecordedGraphicsWorkInformation.clear();
	roRootGraphics::sm_FrameContext[roRootGraphics::sm_CurrentFrameIndex].FenceValue = CurrFramefenceValue;
	roRootGraphics::sm_CurrentFrameIndex = (roRootGraphics::sm_CurrentFrameIndex + 1) % roRootGraphics::sm_FrameCount;

}

void roGraphicsComponent::WaitForTailFrame() {
	using namespace RingedOrb;

	int tailFrameIdx = (roRootGraphics::sm_CurrentFrameIndex + 1) % roRootGraphics::sm_FrameCount;
	auto PrevFenceVal = roRootGraphics::sm_FrameContext[tailFrameIdx].FenceValue;

	roCommandQueue::WaitForFenceValue(PrevFenceVal);	
	//resolve completed gpu profiles
	static float elapsedTime = .0f;

	if (roTimer::TotalTime() - elapsedTime > 0.20f) {
		elapsedTime = roTimer::TotalTime();
		for (int i = 0; i < g_NumGPUProfiles; i++)
			RingedOrb::ResolveGPUProfileStats((roGPUProfileInfoType)i, tailFrameIdx);
	}

}

void roGraphicsComponent::FlushGPU() {
	roCommandQueue::WaitForGPU();
}

void roGraphicsComponent::Shutdown() {
	if (sm_IsCreated) {
		FlushGPU();

		RingedOrb::ShutdownGPUProfiles();
		ShutdownResources();
		m_pGraphicsUI->Shutdown();
		SAFE_DELETE(m_pGraphicsUI);
		roGraphicsRenderer::Shutdown();
		roGPUContextManager::Shutdown();
		roRootGraphics::Shutdown();
	}
}

void roGraphicsComponent::ShutdownResources() {
	//release ring buffer
	if(m_pConstantBufferRing)
		m_pConstantBufferRing->Destroy();

	SAFE_DELETE(m_pConstantBufferRing);
	//resources
	roGraphicsResourceCleanUp::Shutdown();
	SAFE_DELETE(m_pGraphicsManager);
}

bool roGraphicsComponent::sm_IsCreated = false;
