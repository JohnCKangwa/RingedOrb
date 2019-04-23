#pragma once
#include "..\Core\pch.h"
#include "..\Core\roProfiler.h"

struct roConstantRingBuffer;
class 	roGraphicsUI;
class roGraphicsManager;
class roEventQueue;
 
class roGraphicsComponent {
public:
	roGraphicsComponent();

	void Initialize(roEventQueue* eventQueue, RingedOrb::roCPUProfiler* cpuProfiler);
	void InitializeResources();
	void OnResize();
	void Update();
	void Render();

	void Shutdown();
	void ShutdownResources();

private:
	void MoveToNextFrame(uint64_t CurrFramefenceValue);
	void WaitForTailFrame();
	void FlushGPU();

private:
	static bool sm_IsCreated;
	roConstantRingBuffer* m_pConstantBufferRing;
	roGraphicsUI* m_pGraphicsUI;
	roGraphicsManager* m_pGraphicsManager;
	RingedOrb::roCPUProfiler* m_pCpuProfiler = nullptr;
};