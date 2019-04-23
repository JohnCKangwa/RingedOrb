#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"

using namespace Microsoft::WRL;


class roBaseGPUResource
{
	friend class roGPUContext12;
	friend class roGraphicsComponent;
public:
	roBaseGPUResource();
	~roBaseGPUResource();


public:
	virtual void DiscardResource();

	ID3D12Resource* GetResource() const;

protected:
	ComPtr<ID3D12Resource> m_resource;
	D3D12_RESOURCE_STATES m_resource_state;
};
