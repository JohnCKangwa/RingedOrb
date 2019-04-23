#pragma once
#include "roGPUBuffer.h"


class roVertexBuffer : public roGPUBuffer {

public:
	roVertexBuffer() = delete;
	~roVertexBuffer();
	roVertexBuffer(UINT bufferSize, UINT vertexStride, bool uploadable);
	void UploadData(int index, UINT numElements, const void* data);
	D3D12_VERTEX_BUFFER_VIEW GetView(UINT offset);
	void Destroy();
};


class roIndexBuffer : public roGPUBuffer {
public:
	roIndexBuffer() = delete;
	~roIndexBuffer();
	roIndexBuffer(UINT bufferSize, bool is32bit, bool uploadable);
	void UploadData(int index, UINT numElements, const void* data);
	D3D12_INDEX_BUFFER_VIEW GetView(UINT offset);
	void Destroy();
};
