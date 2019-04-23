#include "roCommonBuffers.h"

roReadBackBuffer::roReadBackBuffer(UINT bufferSize, UINT elementSize) : roGPUBuffer() {
	roGPUBuffer::CreateReadBackBuffer(bufferSize, elementSize);
}

void roReadBackBuffer::GetData(UINT index, UINT numElements, void* data) {
	roGPUBuffer::CopyToCPU(index, numElements, data);
}

void roReadBackBuffer::Destoy() {
	roGPUBuffer::DiscardResource();
}
