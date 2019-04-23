#include "roGeometryBuffer.h"



roVertexBuffer::roVertexBuffer(UINT bufferSize, UINT vertexStride, bool uploadable = false) : roGPUBuffer() {
	CreateVertexBuffer(bufferSize, vertexStride, uploadable);
}

void roVertexBuffer::UploadData(int index, UINT numElements, const void* data) {
	CopyToGPU(index, numElements, data);
}

D3D12_VERTEX_BUFFER_VIEW roVertexBuffer::GetView(UINT offset) {
	return roGPUBuffer::GetVertexBufferView(offset);
}

void roVertexBuffer::Destroy() {
	roGPUBuffer::DiscardResource();
}

roVertexBuffer::~roVertexBuffer(){}



roIndexBuffer::roIndexBuffer(UINT bufferSize, bool is32bit, bool uploadable) {
	CreateIndexBuffer(bufferSize, is32bit, uploadable);
}

void roIndexBuffer::UploadData(int index, UINT numElements, const void* data) {
	CopyToGPU(index, numElements, data);
}

D3D12_INDEX_BUFFER_VIEW roIndexBuffer::GetView(UINT offset) {
	return roGPUBuffer::GetIndexBufferView(offset);
}

void roIndexBuffer::Destroy() {
	roGPUBuffer::DiscardResource();
}

roIndexBuffer::~roIndexBuffer(){}

