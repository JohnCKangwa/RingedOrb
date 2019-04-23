#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"

namespace RingedOrb {

	struct roGraphicsWorkInfo {
		std::string Name;
		uint64_t FenceValue;
	};

	static std::vector<roGraphicsWorkInfo> g_RecordedGraphicsWorkInformation;
}


class roGPUContext12;
class roGPUContextManager;
class roCommandAllocator;

#define MAX_WAITABLE_QUEUES 2
//supports 1 command queue at the moment(generic gpu command submission).
class roCommandQueue
{
	friend roGPUContext12;
	friend roGPUContextManager;	
	friend class roGraphicsComponent;
	friend struct roRootGraphics;

public:
	static void Initialize();
	static ID3D12CommandQueue* GetCmdQueue();

public:
	static ID3D12CommandAllocator* GetCommandAllocator();
	static void WaitForGPU();
	static bool IsFenceComplete(uint64_t fenceValue);
	static uint64_t SignalFence();
	static void WaitForFenceValue(uint64_t fenceValue);
private:
	static void ExecuteCommandList(ID3D12CommandList* commandlist);
	static void ExecuteCommandListOnWaitableQueue(ID3D12CommandList* commandlist, int queueIdx, std::string workToWaitOn);
	static void Shutdown();
private:
	static ID3D12CommandQueue*					sm_CommandQueue;
	static ID3D12CommandQueue*					sm_WaitableCommandQueue[MAX_WAITABLE_QUEUES];
	static ID3D12Fence*							sm_Fence;
	static uint64_t								sm_FenceValue;
	static HANDLE								sm_FenceEventHandle;
	static std::mutex							sm_cmdQueueMutex;
	static std::mutex							sm_WatableCmdQueueMutex;
};