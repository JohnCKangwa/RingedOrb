#include "roDepthBuffer.h"
#include "..\Graphics\roRootGraphics.h"
#include "..\Core\roErrorHandling.h"
#include "roRootGraphics.h"
#include "roDescriptorHeapManager.h"


roDepthBuffer::roDepthBuffer() : roBaseGPUResource() { 

	m_dsvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	m_srvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	m_resource_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
}

roDepthBuffer::~roDepthBuffer() {}

bool roDepthBuffer::CreateDepthBuffer(uint32_t width, uint32_t
	height, uint32_t arraySize = 1, UINT mipLevels = 1, UINT sampleCount = 1,
	UINT sampleQuality = 0) {


	DiscardResource();

	D3D12_RESOURCE_DESC depthStencilDesc = TextureDescription(width, height, arraySize, DXGI_FORMAT_R24G8_TYPELESS,
		mipLevels, sampleCount, sampleQuality);

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth	= 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	
	HR(roRootGraphics::sm_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE, &depthStencilDesc, m_resource_state, &clearValue, 
		IID_PPV_ARGS(m_resource.GetAddressOf())));

	CreateResourceDescriptors();

	return true;
}

void roDepthBuffer::DiscardResource() {

	if (!m_resource)
		return;
	//invalidate descriptors
	if (m_dsvHandle.ptr >= (D3D12_GPU_VIRTUAL_ADDRESS)0) {
		roDescriptorHeapManager::DiscardHandle(m_dsvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_dsvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	}
	else if (m_srvHandle.ptr >= (D3D12_GPU_VIRTUAL_ADDRESS)0) {
		roDescriptorHeapManager::DiscardHandle(m_srvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_dsvHandle.ptr = (D3D12_GPU_VIRTUAL_ADDRESS)-1;
	}


	roBaseGPUResource::DiscardResource();
}

void roDepthBuffer::CreateResourceDescriptors() {

	m_dsvHandle = roDescriptorHeapManager::GetFreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags				= D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension		= D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice	= 0;
	roRootGraphics::sm_Device->CreateDepthStencilView(m_resource.Get(), &dsvDesc, m_dsvHandle);
}

D3D12_RESOURCE_DESC roDepthBuffer::TextureDescription(uint32_t width, uint32_t
	height, uint32_t arraySize, DXGI_FORMAT format,UINT mipLevels = 1, UINT sampleCount = 1,
	UINT sampleQuality = 0) {

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width				= width;
	desc.Height				= height;
	desc.DepthOrArraySize	= arraySize;
	desc.MipLevels			= mipLevels;
	desc.SampleDesc.Count	= sampleCount;
	desc.SampleDesc.Quality	= sampleQuality;
	desc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Alignment			= 0;

	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Flags	= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	return desc;
}

D3D12_CPU_DESCRIPTOR_HANDLE roDepthBuffer::GetDSV() const {
	return m_dsvHandle;
}
D3D12_CPU_DESCRIPTOR_HANDLE roDepthBuffer::GetSRV() const {
	return m_srvHandle;
}