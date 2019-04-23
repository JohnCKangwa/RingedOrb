#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"
//Heaps in this class are CPU visible.
//copy to on demand GPU ring buffer-based descriptor as needed 
using namespace Microsoft::WRL;

class roDescriptorHeapManager
{
	friend class roImageBuffer;
	friend class roDepthBuffer;
public:
	static void Initialize();
	static D3D12_CPU_DESCRIPTOR_HANDLE GetFreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type);
	static void Shutdown();
private:
	static void DiscardHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_DESCRIPTOR_HEAP_TYPE type);
	static void RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

private:
	static std::vector<ID3D12DescriptorHeap*>			sm_DescriptorHeaps
		[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	static std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>		sm_unusedHandles
		[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	static uint32_t										sm_currOffset
		[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	static UINT											sm_numMaxDescriptors;
	static std::mutex									sm_descriptorMutex;
	static UINT											sm_descriptorIncrementSize
		[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};
