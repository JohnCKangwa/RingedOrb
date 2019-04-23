#pragma once
#include "roBaseGPUResource.h"

class roDepthBuffer : public roBaseGPUResource
{
public:
	roDepthBuffer();
	~roDepthBuffer();

	bool CreateDepthBuffer(uint32_t width, uint32_t
		height, uint32_t arraySize , UINT mipLevels, UINT sampleCount,
		UINT sampleQuality);

	void DiscardResource() override;


public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;

private:
	void CreateResourceDescriptors();

	D3D12_RESOURCE_DESC TextureDescription(uint32_t width,
		uint32_t height, uint32_t arraySize, DXGI_FORMAT format,
		UINT mipLevels, UINT sampleCount, UINT sampleQuality);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle;
};

