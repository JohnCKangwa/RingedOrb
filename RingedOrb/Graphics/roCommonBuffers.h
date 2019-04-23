#pragma once
#include "roGPUBuffer.h"


class roReadBackBuffer : public roGPUBuffer {

public:
	roReadBackBuffer(UINT bufferSize, UINT elementSize);
	void GetData(UINT index, UINT numElements, void* data);
	void Destoy();
};