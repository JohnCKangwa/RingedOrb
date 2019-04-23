#pragma once
#include "roBaseGPUResource.h"


class roImageBuffer : public roBaseGPUResource
{
public:
	roImageBuffer();
	void CreateImageFromSwapChain(ID3D12Resource* resource);
	void CreateImageFromFile();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;

	void DiscardResource(bool isSwapChain);


private:
	void CreateResourceDescriptors(bool bGetDescFromResource);

private:
	D3D12_RESOURCE_DESC m_ResourceDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle;

};