#include "roCommandAllocator.h"
#include "..\Core\roErrorHandling.h"


ID3D12CommandAllocator* roCommandAllocator::GetAllocator(uint64_t fenceValue) {

	std::lock_guard<std::mutex> lg(sm_AllocMutex);

	ID3D12CommandAllocator* cmdAlloc = nullptr;

	if (sm_UsedCommandAllocators.size() != 0) {
		auto cmdAllocTemp = sm_UsedCommandAllocators.front();

		if (fenceValue >= cmdAllocTemp.FenceValue) {
			cmdAllocTemp.CmdAlloc->Reset();
			cmdAlloc = cmdAllocTemp.CmdAlloc;
			sm_UsedCommandAllocators.pop();
			return cmdAlloc;
		}
 	}

	ID3D12CommandAllocator* cmdAlloc_;
	
	roRootGraphics::sm_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(&cmdAlloc_));
	cmdAlloc = cmdAlloc_;
	sm_CommandAllocators.push_back(cmdAlloc_);

	
	return cmdAlloc;
}

void roCommandAllocator::Shutdown() {

	for (auto& alloc : sm_CommandAllocators) {
		SAFE_RELEASE(alloc);
	}
}

void roCommandAllocator::DiscardAllocator(ID3D12CommandAllocator* cmdAlloc, uint64_t fenceVal) {
	std::lock_guard<std::mutex> lg(sm_AllocMutex);

	AllocInfo cmdAllocInfo;

	cmdAllocInfo.CmdAlloc	= cmdAlloc;
	cmdAllocInfo.FenceValue = fenceVal;

	sm_UsedCommandAllocators.push(cmdAllocInfo);
}

std::vector<ID3D12CommandAllocator*> roCommandAllocator::sm_CommandAllocators;
std::queue<roCommandAllocator::AllocInfo> roCommandAllocator::sm_UsedCommandAllocators;
std::mutex roCommandAllocator::sm_AllocMutex;