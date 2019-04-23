#include "roProfiler.h"

namespace RingedOrb {

	int									g_NumCPUProfiles;
	int									g_NumGPUProfiles;
	ID3D12QueryHeap*					g_TimeStampQueryHeap;
	roReadBackBuffer*					g_TimeSamplesGPUBuffer;
	std::vector<roProfilerInfo>			g_CPUProfiles;
	std::vector<roProfilerInfo>			g_GPUProfiles;
	double								g_GPUSecondsPerCount(.0f);
	double								g_CPUSecondsPerCount(.0f);



	roGPUProfiler::roGPUProfiler( ID3D12CommandQueue* cmdQueue) {

		uint64_t countsPerSecond;

		HR(cmdQueue->GetTimestampFrequency(&countsPerSecond));
		g_GPUSecondsPerCount = 1.0 / static_cast<double>(countsPerSecond);
	}



	roCPUProfiler::roCPUProfiler() {
		UINT64 countsPerSecond;


		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
		g_CPUSecondsPerCount = 1.0f / (float)countsPerSecond;

	}

}