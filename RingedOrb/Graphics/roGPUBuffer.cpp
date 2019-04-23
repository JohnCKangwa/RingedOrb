#include "roGPUBuffer.h"
#include "roRootGraphics.h"
#include "..\Core\roErrorHandling.h"


roGPUBuffer::roGPUBuffer() : roBaseGPUResource() {

}

roGPUBuffer::~roGPUBuffer()
{
}

void roGPUBuffer::CreateVertexBuffer(UINT bufferSize, UINT vertexStride, bool uploadable) {
	m_BufferSize = bufferSize;
	m_ElementSize = vertexStride;
	CreateBuffer(bufferSize, uploadable);

	if (uploadable)
		MapCpuAddressPointer(0, 0);
}

void roGPUBuffer::CreateIndexBuffer(UINT bufferSize, bool is32bit, bool uploadable) {
	m_Is32bit = is32bit;
	m_BufferSize = bufferSize;

	if(is32bit)
		m_ElementSize = sizeof(UINT32);
	else
		m_ElementSize = sizeof(UINT16);

	CreateBuffer(bufferSize, uploadable);

	if (uploadable)
		MapCpuAddressPointer(0, 0);
}

void roGPUBuffer::CreateBuffer(UINT64 bufferSize, bool uploadable) {

	D3D12_HEAP_PROPERTIES heapProps;
	if (uploadable)
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	else
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask		= 1;
	heapProps.VisibleNodeMask		= 1;

	if (uploadable)
		m_resource_state = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	else
		m_resource_state = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	D3D12_RESOURCE_DESC resourceDesc = DeriveBufferDesc(bufferSize);

	HR(roRootGraphics::sm_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, m_resource_state, 
		nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())));
}

void roGPUBuffer::CreateConstantBuffer(UINT bufferSize, UINT elementSize) {
	m_BufferSize = bufferSize;
	m_ElementSize = elementSize;
	CreateBuffer(bufferSize, true);

	MapCpuAddressPointer(0, 0);
 }

D3D12_VERTEX_BUFFER_VIEW roGPUBuffer::GetVertexBufferView(UINT offset) {

	D3D12_GPU_VIRTUAL_ADDRESS vbAddress = m_resource.Get()->GetGPUVirtualAddress();
	vbAddress += sizeof(m_ElementSize) * offset;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation	= vbAddress;
	vbView.SizeInBytes		= (UINT)(m_BufferSize - (sizeof(m_ElementSize) * offset));
	vbView.StrideInBytes	= (UINT)m_ElementSize;

	return vbView;
}

D3D12_INDEX_BUFFER_VIEW roGPUBuffer::GetIndexBufferView(UINT offset) {

	D3D12_GPU_VIRTUAL_ADDRESS ibAddress = m_resource.Get()->GetGPUVirtualAddress();
	ibAddress += sizeof(m_ElementSize) * offset;

	D3D12_INDEX_BUFFER_VIEW ibView;
	ibView.BufferLocation	= ibAddress;
	if (m_Is32bit)
		ibView.Format		= DXGI_FORMAT_R32_UINT;
	else
		ibView.Format		= DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes		= (UINT)(m_BufferSize - (sizeof(m_ElementSize) * offset));

	return ibView;
	
}

D3D12_GPU_VIRTUAL_ADDRESS roGPUBuffer::GetCBufferAddressOffset(UINT offset) {

	D3D12_GPU_VIRTUAL_ADDRESS CbAddress = m_resource.Get()->GetGPUVirtualAddress();
	CbAddress += sizeof(m_ElementSize) * offset;

	return CbAddress;
}


void roGPUBuffer::CreateConstantBufferAsRing(UINT64 bufferSize) {
	m_BufferSize = bufferSize;
	m_ElementSize = 0;
	CreateBuffer(bufferSize, true);

	MapCpuAddressPointer(0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS roGPUBuffer::GetCBufferAddressOffsetAsRing(UINT64 offset) {

	D3D12_GPU_VIRTUAL_ADDRESS CbAddress = m_resource.Get()->GetGPUVirtualAddress();
	CbAddress += offset;

	return CbAddress ;
}

void roGPUBuffer::CopyToGPUAsRing(UINT64 offset, void* data, UINT memSize) {
	std::memcpy(m_GPUMappedResourcePointer + offset, data, memSize);
}

void roGPUBuffer::CreateReadBackBuffer(UINT bufferSize, UINT elementSize) {

	m_BufferSize = bufferSize;
	m_ElementSize = elementSize;
	
	D3D12_HEAP_PROPERTIES heapProps;

	heapProps.Type = D3D12_HEAP_TYPE_READBACK;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	m_resource_state = D3D12_RESOURCE_STATE_COPY_DEST;

	D3D12_RESOURCE_DESC resourceDesc = DeriveBufferDesc(bufferSize);

	HR(roRootGraphics::sm_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, m_resource_state, nullptr, 
		IID_PPV_ARGS(m_resource.GetAddressOf())));	

}

D3D12_RESOURCE_DESC roGPUBuffer::DeriveBufferDesc(UINT64 width, D3D12_RESOURCE_FLAGS flags, UINT64 alignment) {

	D3D12_RESOURCE_DESC bufferDesc;

	bufferDesc.Width			= width;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment		= alignment;
	bufferDesc.Flags			= flags;
	bufferDesc.Format			= DXGI_FORMAT_UNKNOWN;
	bufferDesc.Height			= 1;
	bufferDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.MipLevels		= 1;
	bufferDesc.SampleDesc.Count	= 1;
	bufferDesc.SampleDesc.Quality = 0;
	
	return bufferDesc;
}

void roGPUBuffer::MapCpuAddressPointer(UINT index, UINT numElements) {
	D3D12_RANGE* pRange;

	if (numElements == 0) {
		pRange = nullptr;
	}
	else {
		pRange = new D3D12_RANGE();
		pRange->Begin = index * m_ElementSize;
		pRange->End = pRange->Begin + numElements * m_ElementSize;
	}
	m_resource.Get()->Map(0, pRange, reinterpret_cast<void**>(&m_GPUMappedResourcePointer));
	SAFE_DELETE(pRange);
}

void roGPUBuffer::UnmapCpuAddressPointer() {
	if(m_resource)
		m_resource.Get()->Unmap(0, 0);
}

void roGPUBuffer::CopyToGPU(UINT index, UINT numElements, const void* data) {
	if(numElements == 0)
		std::memcpy(&m_GPUMappedResourcePointer[index * m_ElementSize], data, m_BufferSize - (index * m_ElementSize));
	else
		std::memcpy(&m_GPUMappedResourcePointer[index * m_ElementSize], data, numElements * m_ElementSize);
}

void roGPUBuffer::CopyToCPU(UINT index, UINT numElements, void* data) {

	MapCpuAddressPointer(index, numElements);
	if(numElements == 0)
		std::memcpy(data, &m_GPUMappedResourcePointer[index * m_ElementSize], m_BufferSize - (index * m_ElementSize));
	else
		std::memcpy(data, &m_GPUMappedResourcePointer[index * m_ElementSize], m_ElementSize * numElements);

	UnmapCpuAddressPointer();
}

void roGPUBuffer::DiscardResource() {
	if (m_IsUploadBuffer)
		UnmapCpuAddressPointer();

	m_GPUMappedResourcePointer = nullptr;
	roBaseGPUResource::DiscardResource();
}
