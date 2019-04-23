#pragma once
#include "roBaseGPUResource.h"

class roGPUBuffer : public roBaseGPUResource {

protected:
	roGPUBuffer();
	~roGPUBuffer();

	void CreateVertexBuffer(UINT bufferSize, UINT vertexStride, bool uploadable = false);
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(UINT offset);

	void CreateIndexBuffer(UINT bufferSize, bool is32bit, bool uploadable);
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(UINT offset);

	void CreateConstantBuffer(UINT bufferSize, UINT elementSize);
	D3D12_GPU_VIRTUAL_ADDRESS GetCBufferAddressOffset(UINT offset);

	void CreateConstantBufferAsRing(UINT64 bufferSize);
	D3D12_GPU_VIRTUAL_ADDRESS GetCBufferAddressOffsetAsRing(UINT64 offset);
	void CopyToGPUAsRing(UINT64 offset, void* data, UINT memSize);

	void CreateReadBackBuffer(UINT bufferSize, UINT elementSize);

	D3D12_RESOURCE_DESC DeriveBufferDesc(UINT64 width, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, UINT64 alignment = 0);

	void MapCpuAddressPointer(UINT index, UINT numElements);
	void UnmapCpuAddressPointer();

	void CopyToGPU(UINT index, UINT numElements,const void* data);
	void CopyToCPU(UINT index, UINT numElements, void* data);

	void DiscardResource() override;

private:
	void CreateBuffer(UINT64 bufferSize, bool uploadable = false);

private:
	BYTE* m_GPUMappedResourcePointer;
	UINT m_ElementSize;
	UINT64 m_BufferSize;
	bool m_IsUploadBuffer = false;
	bool m_Is32bit = false;
};