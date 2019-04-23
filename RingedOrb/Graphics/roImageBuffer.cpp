#include "roImageBuffer.h"
#include "roDescriptorHeapManager.h"
#include "roRootGraphics.h"
roImageBuffer::roImageBuffer() : roBaseGPUResource() {}

void roImageBuffer::CreateImageFromSwapChain(ID3D12Resource * resource) {

	if (!m_resource)
		DiscardResource(true);

	m_resource.Attach(resource);

	CreateResourceDescriptors(true);
}

D3D12_CPU_DESCRIPTOR_HANDLE roImageBuffer::GetRTV() const {
	return m_rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE roImageBuffer::GetSRV() const {
	return m_srvHandle;
}

void roImageBuffer::DiscardResource(bool isSwapChain) {

	if (!m_resource)
		return;

	if (m_rtvHandle.ptr >= (D3D12_GPU_VIRTUAL_ADDRESS)0) {
		roDescriptorHeapManager::DiscardHandle(m_rtvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_rtvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	}
	else if (m_srvHandle.ptr >= (D3D12_GPU_VIRTUAL_ADDRESS)0) {
		roDescriptorHeapManager::DiscardHandle(m_srvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_srvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	}

	if (isSwapChain)
		m_resource.Detach();
	else
		roBaseGPUResource::DiscardResource();
}

void roImageBuffer::CreateResourceDescriptors(bool bGetDescFromResource) {

	m_rtvHandle = roDescriptorHeapManager::GetFreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	roRootGraphics::sm_Device->CreateRenderTargetView(m_resource.Get(), 0, m_rtvHandle);
}