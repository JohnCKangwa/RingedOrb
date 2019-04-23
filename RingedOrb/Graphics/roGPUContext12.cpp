#include "roGPUContext12.h"
#include "roRootGraphics.h"
#include "roCommandAllocator.h"
#include "roBaseGPUResource.h"
#include "..\Core\roErrorHandling.h"
#include "..\Core\roWindow.h"

roGPUContext12::roGPUContext12(std::string name) : roGPUProfiler(roCommandQueue::GetCmdQueue()) {

	auto cmdAlloc = roCommandQueue::GetCommandAllocator();

	HR(roRootGraphics::sm_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	m_currCommandAllocator = cmdAlloc;
	m_Name = name;
}

roGPUContext12::~roGPUContext12(){}

void roGPUContext12::Reset(std::string& name) {
	//problem area leading to application growage :-/
	auto cmdAlloc = roCommandQueue::GetCommandAllocator();

	m_CommandList->Reset(cmdAlloc, 0);
	m_currCommandAllocator = cmdAlloc;
	m_Name = name;
}

void roGPUContext12::CopyResource(roBaseGPUResource* src, roBaseGPUResource* dst) {
	m_CommandList->CopyResource(dst->GetResource(), src->GetResource());
}

void roGPUContext12::CopyBufferRegion(roBaseGPUResource* src, UINT64 srcOffset, roBaseGPUResource* dst, UINT64 dstOffset, UINT64 numBytes) {
	m_CommandList->CopyBufferRegion(dst->GetResource(), dstOffset, src->GetResource(), srcOffset, numBytes);
}



void roGPUContext12::SetCustomViewport(UINT width, UINT height, UINT topLeftX, UINT topLeftY) {
	D3D12_VIEWPORT viewport;
	viewport.Width		= static_cast<float>(width);
	viewport.Height		= static_cast<float>(height);
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;
	viewport.TopLeftX	= (float)topLeftX;
	viewport.TopLeftY	= (float)topLeftY;
	m_CommandList->RSSetViewports(1, &viewport); 
}


void roGPUContext12::SetViewports(D3D12_VIEWPORT* viewports, uint32_t numViewports) {
	m_CommandList->RSSetViewports(numViewports, viewports);
}

void roGPUContext12::ExecuteCommands() {
	m_CommandList->Close();
	roCommandQueue::ExecuteCommandList(m_CommandList.Get());
}

void roGPUContext12::ExecuteOnWaitableQueue(int queueIdx, std::string workToWaitOn) {
	m_CommandList->Close();
	roCommandQueue::ExecuteCommandListOnWaitableQueue(m_CommandList.Get(), queueIdx, workToWaitOn);
}

uint64_t roGPUContext12::DiscardCommandAllocator() {

	uint64_t fenceVal = roCommandQueue::SignalFence();
	roCommandAllocator::DiscardAllocator(m_currCommandAllocator, fenceVal);
	m_currCommandAllocator = nullptr;

	return fenceVal;
}

uint64_t roGPUContext12::Signal() const {
	return roCommandQueue::SignalFence();
}

ID3D12GraphicsCommandList * roGPUContext12::GetCommandList() const {
	return m_CommandList.Get();
}
