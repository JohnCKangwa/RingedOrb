#include "roGPUContextManager.h"
#include "roGPUContext12.h"
#include "roCommandAllocator.h"
#include "roCommandQueue.h"

roGPUContext12* roGPUContextManager::GetGPUContext(std::string name) {
	std::lock_guard<std::mutex> lg(sm_GPUContexManagerMutex);

	roGPUContext12* gpuContext = nullptr;

	if (!sm_FreeGPUContextPool.empty()) {
		gpuContext = sm_FreeGPUContextPool.back();
		gpuContext->Reset(name);
		sm_FreeGPUContextPool.pop_back();
		return gpuContext;
	}
	
	std::unique_ptr<roGPUContext12> _gpuContext 
		= std::make_unique<roGPUContext12>(name);

	gpuContext = _gpuContext.get();
	sm_GpuContextPool.push_back(std::move(_gpuContext));

	return gpuContext;
}

void roGPUContextManager::Shutdown() {
	sm_FreeGPUContextPool.clear();

	for (int i = 0; i < sm_GpuContextPool.size(); i++) {
		sm_GpuContextPool[i].reset();
	}
}

void roGPUContextManager::DiscardGPUContext(roGPUContext12* gpu_context) {
	std::lock_guard<std::mutex> lg(sm_GPUContexManagerMutex);

	sm_FreeGPUContextPool.push_back(gpu_context);
}

std::vector<std::unique_ptr<roGPUContext12>> roGPUContextManager::	sm_GpuContextPool;
std::vector<roGPUContext12*> roGPUContextManager::					sm_FreeGPUContextPool;
std::mutex roGPUContextManager::									sm_GPUContexManagerMutex;
