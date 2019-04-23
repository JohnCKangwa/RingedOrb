#pragma once
#include "roGraphicsRingBuffer.h"
#include "roConstantBuffer.h"


class roConstantRingBuffer : public roGraphicsRingBuffer {
public: 
	roConstantRingBuffer(UINT64 fixedBlockSize, UINT64 bufferSize);
	UINT64 UploadConstantData(void* data, UINT memsize);
	D3D12_GPU_VIRTUAL_ADDRESS GetAddressAtOffset(UINT64 offset);
	void MarkEndOfFrame(int frameIdx);
	void Destroy();

private:
	roConstantBufferAsRing m_ConstantBuffer;
};