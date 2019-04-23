#pragma once
#include "roGPUBuffer.h"

class roConstantBuffer : public roGPUBuffer {
public:
	roConstantBuffer() = delete;
	~roConstantBuffer();

	roConstantBuffer(UINT bufferSize, UINT elementSize);
	void UploadData(int index, UINT numElements, const void* data);
	D3D12_GPU_VIRTUAL_ADDRESS GetView(UINT offset);
	void Destroy();
};

class roConstantBufferAsRing : public roGPUBuffer {
public:
	roConstantBufferAsRing() = delete;
	~roConstantBufferAsRing();

	roConstantBufferAsRing(UINT64 bufferSize);
	void UploadData(UINT64 offset, void* data, UINT memsize);
	D3D12_GPU_VIRTUAL_ADDRESS GetView(UINT64 offset);
	void Destroy();
};