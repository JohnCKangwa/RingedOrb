#include "roConstantBuffer.h"



roConstantBuffer::roConstantBuffer(UINT bufferSize, UINT elementSize) : roGPUBuffer() {
	roGPUBuffer::CreateConstantBuffer(bufferSize, elementSize);
}

void roConstantBuffer::UploadData(int index, UINT numElements, const void* data) {
	roGPUBuffer::CopyToGPU(index, numElements, data);
}

D3D12_GPU_VIRTUAL_ADDRESS roConstantBuffer::GetView(UINT offset) {
	return roGPUBuffer::GetCBufferAddressOffset(offset);
}

void roConstantBuffer::Destroy() {
	roGPUBuffer::DiscardResource();
}

roConstantBuffer::~roConstantBuffer(){}

///////////////////////////////////////////////////////////////////////////////

roConstantBufferAsRing::~roConstantBufferAsRing()
{
}

roConstantBufferAsRing::roConstantBufferAsRing(UINT64 bufferSize) : roGPUBuffer() {
	roGPUBuffer::CreateConstantBufferAsRing(bufferSize);
}

void roConstantBufferAsRing::UploadData(UINT64 offset, void* data, UINT memsize) {
	roGPUBuffer::CopyToGPUAsRing(offset, data, memsize);
}

D3D12_GPU_VIRTUAL_ADDRESS roConstantBufferAsRing::GetView(UINT64 offset) {
	return roGPUBuffer::GetCBufferAddressOffsetAsRing(offset);
}

void roConstantBufferAsRing::Destroy() {
	roGPUBuffer::DiscardResource();
}
