#include "roBaseGPUResource.h"

roBaseGPUResource::roBaseGPUResource() {
	m_resource			= nullptr;
	m_resource_state	= D3D12_RESOURCE_STATE_COMMON;
}

roBaseGPUResource::~roBaseGPUResource() {
}

void roBaseGPUResource::DiscardResource() {
	if (!m_resource)
		return;

	m_resource.Reset();
}

ID3D12Resource* roBaseGPUResource::GetResource() const {
	return m_resource.Get();
}
