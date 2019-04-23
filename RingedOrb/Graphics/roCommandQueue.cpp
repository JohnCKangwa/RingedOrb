#include "..\Graphics\roCommandQueue.h"
#include "..\Graphics\roRootGraphics.h"
#include "..\Core\roErrorHandling.h"
#include "roCommandAllocator.h"
#include "..\Core\pch.h"


void roCommandQueue::Initialize() {

	assert(roRootGraphics::sm_Device);

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.NodeMask	= 0;
	cmdQueueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;

	roRootGraphics::sm_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&sm_CommandQueue));

	for (int queueIdx = 0; queueIdx < MAX_WAITABLE_QUEUES; queueIdx++) {
		roRootGraphics::sm_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&sm_WaitableCommandQueue[queueIdx]));
	}

	HR(roRootGraphics::sm_Device->CreateFence(sm_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&sm_Fence)));
	sm_FenceEventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

}

void roCommandQueue::ExecuteCommandList(ID3D12CommandList * commandlist) {
	//correct this wrong logic
	std::lock_guard<std::mutex> lg(sm_cmdQueueMutex);

	sm_CommandQueue->ExecuteCommandLists(1, &commandlist);	
}

void roCommandQueue::ExecuteCommandListOnWaitableQueue(ID3D12CommandList* commandlist, int queueIdx, std::string workToWaitOn) {
	bool workFound = false;
	uint64_t fenceValueToWaitOn = 0;

	for (auto& workIdx : RingedOrb::g_RecordedGraphicsWorkInformation) {
		if (workIdx.Name == workToWaitOn) {
			fenceValueToWaitOn = workIdx.FenceValue;
			workFound = true;
			break;
		}
	}

	if(!workFound)
		THROW(L"Work to wait on not found")

	auto& waitableQueue = sm_WaitableCommandQueue[queueIdx];
	waitableQueue->Wait(sm_Fence, fenceValueToWaitOn);

	waitableQueue->ExecuteCommandLists(1, &commandlist);
}

ID3D12CommandQueue * roCommandQueue::GetCmdQueue() {
	assert(sm_CommandQueue);
	return sm_CommandQueue;
}

ID3D12CommandAllocator * roCommandQueue::GetCommandAllocator() {

	auto currCompletedValue = sm_Fence->GetCompletedValue();
	auto cmdAlloc = roCommandAllocator::GetAllocator(currCompletedValue);
	
	return cmdAlloc;
}

void roCommandQueue::WaitForGPU() {

	if (!IsFenceComplete(sm_FenceValue)) {

		sm_Fence->SetEventOnCompletion(sm_FenceValue, sm_FenceEventHandle);
		WaitForSingleObject(sm_FenceEventHandle, INFINITE);
	}
}

bool roCommandQueue::IsFenceComplete(uint64_t fenceValue) {
	bool result = (sm_Fence->GetCompletedValue() >= sm_FenceValue);
	return result;
}

uint64_t roCommandQueue::SignalFence() {
	++sm_FenceValue;
	sm_CommandQueue->Signal(sm_Fence, sm_FenceValue);
	return sm_FenceValue;
}

void roCommandQueue::WaitForFenceValue(uint64_t fenceValue) {
	if (!IsFenceComplete(fenceValue)) {

		sm_Fence->SetEventOnCompletion(fenceValue, sm_FenceEventHandle);
		WaitForSingleObject(sm_FenceEventHandle, INFINITE);
	}
}

void roCommandQueue::Shutdown() {

	roCommandAllocator::Shutdown();

	CloseHandle(sm_FenceEventHandle);
	SAFE_RELEASE(sm_Fence);
	SAFE_RELEASE(sm_CommandQueue);

	for (int cmdQIdx = 0; cmdQIdx < MAX_WAITABLE_QUEUES; cmdQIdx++)
		SAFE_RELEASE(sm_WaitableCommandQueue[cmdQIdx]);

}

ID3D12CommandQueue* roCommandQueue::sm_CommandQueue = nullptr;
ID3D12CommandQueue*	roCommandQueue::sm_WaitableCommandQueue[MAX_WAITABLE_QUEUES];
ID3D12Fence* roCommandQueue::sm_Fence = nullptr;
uint64_t roCommandQueue::sm_FenceValue;
HANDLE roCommandQueue::sm_FenceEventHandle = {};
std::mutex roCommandQueue::sm_cmdQueueMutex;
std::mutex roCommandQueue::sm_WatableCmdQueueMutex;