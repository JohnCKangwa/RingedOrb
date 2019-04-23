#include "roConstantRingBuffer.h"


roConstantRingBuffer::roConstantRingBuffer(UINT64 fixedBlockSize, UINT64 bufferSize) : roGraphicsRingBuffer(fixedBlockSize, bufferSize),
m_ConstantBuffer(bufferSize) { 

}

UINT64 roConstantRingBuffer::UploadConstantData(void* data, UINT memsize) {

	UINT64 freeMemOffset = roGraphicsRingBuffer::Allocate(memsize);
	m_ConstantBuffer.UploadData(freeMemOffset, data, memsize);

	return freeMemOffset;
}

D3D12_GPU_VIRTUAL_ADDRESS roConstantRingBuffer::GetAddressAtOffset(UINT64 offset) {
	return m_ConstantBuffer.GetView(offset);
}

void roConstantRingBuffer::MarkEndOfFrame(int frameIdx) {
	roGraphicsRingBuffer::MarkTail(frameIdx);
}

void roConstantRingBuffer::Destroy() {
	m_ConstantBuffer.Destroy();
}
