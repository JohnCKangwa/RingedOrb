#pragma once
#include "..\Core\pch.h"

class roGPUContext12;

class roGPUContextManager {
	friend roGPUContext12;
public:
	static roGPUContext12* GetGPUContext(std::string name);
	static void Shutdown();

private:
	static void DiscardGPUContext(roGPUContext12* gpu_context);

private:
	static std::vector<std::unique_ptr<roGPUContext12>> sm_GpuContextPool;
	static std::vector<roGPUContext12*>					sm_FreeGPUContextPool;
	static std::mutex sm_GPUContexManagerMutex;
};