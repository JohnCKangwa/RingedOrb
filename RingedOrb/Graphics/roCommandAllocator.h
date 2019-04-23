#pragma once
#include "DX12IncludeFile.h"
#include "..\Core\pch.h"
#include "roRootGraphics.h"
 
class roCommandAllocator {
	friend class roGPUContext12;
	friend class roCommandQueue;

	struct AllocInfo {
		uint64_t FenceValue					= 0;
		ID3D12CommandAllocator* CmdAlloc	= nullptr;
	};

public:
	static ID3D12CommandAllocator* GetAllocator(uint64_t fenceValue);
	static void Shutdown();
private:
	static void DiscardAllocator(ID3D12CommandAllocator* cmdAlloc, uint64_t fenceVal);
private:
	static std::vector<ID3D12CommandAllocator*>  sm_CommandAllocators;
	//check fence value for requests made to this
	static std::queue<AllocInfo>										sm_UsedCommandAllocators;
	static std::mutex													sm_AllocMutex;
};