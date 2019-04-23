#include "roDescriptorHeapManager.h"
#include "roRootGraphics.h"

//meeds refactor and includment of dynamic descriptors with staging
void roDescriptorHeapManager::Initialize() {
	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		sm_descriptorIncrementSize[i] = roRootGraphics::sm_Device->GetDescriptorHandleIncrementSize((D3D12_DESCRIPTOR_HEAP_TYPE)i);

		RequestNewHeap((D3D12_DESCRIPTOR_HEAP_TYPE)i);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE roDescriptorHeapManager::GetFreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type) {

	std::lock_guard<std::mutex> lg(sm_descriptorMutex);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuhandle;
	if (sm_unusedHandles[type].size() > 0) {
		cpuhandle = sm_unusedHandles[type].back();
		sm_unusedHandles[type].pop_back();
		return cpuhandle;
	}	
	else if (sm_currOffset[type] >= (sm_numMaxDescriptors - 1)) {
		RequestNewHeap(type);
		sm_currOffset[type] = 0;
	}

	std::size_t currHeapIndex = sm_DescriptorHeaps[type].size() - 1;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _cpuhandle(sm_DescriptorHeaps[type][currHeapIndex]->GetCPUDescriptorHandleForHeapStart());
	_cpuhandle.Offset((INT)sm_currOffset[type], sm_descriptorIncrementSize[type]);
	sm_currOffset[type]++;	

	return _cpuhandle;
}

void roDescriptorHeapManager::Shutdown() {

	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		for (auto iter : sm_DescriptorHeaps[i])
			SAFE_RELEASE(iter);
	}
}

void roDescriptorHeapManager::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {

	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	desc.NumDescriptors = sm_numMaxDescriptors;
	desc.Type = type;
	
	ID3D12DescriptorHeap* descriptorheap;
	roRootGraphics::sm_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorheap));

	sm_DescriptorHeaps[type].push_back(descriptorheap);

}

void roDescriptorHeapManager::DiscardHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_DESCRIPTOR_HEAP_TYPE type) {	

	std::lock_guard<std::mutex> lg(sm_descriptorMutex);

	sm_unusedHandles[type].push_back(handle);
}

 std::vector<ID3D12DescriptorHeap*>
	roDescriptorHeapManager::										sm_DescriptorHeaps
	 [D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
 std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>roDescriptorHeapManager::	sm_unusedHandles
	 [D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
 uint32_t roDescriptorHeapManager::									sm_currOffset
	 [D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
 UINT roDescriptorHeapManager::										sm_numMaxDescriptors	
	 = 2500;
	std::mutex roDescriptorHeapManager::							sm_descriptorMutex;
 UINT roDescriptorHeapManager::										sm_descriptorIncrementSize
	 [D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

