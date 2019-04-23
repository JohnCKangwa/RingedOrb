#pragma once
#include "pch.h"
#include "..\Graphics\roCommonBuffers.h"

namespace RingedOrb {
	using namespace Microsoft::WRL;

	enum roGPUProfileInfoType {
		PROFILE_INFO_TYPE_GPU_FRAME_TIME = 0,
		PROFILE_INFO_TYPE_GPU_COUNT = 1
	};

	enum roCPUProfileInfoType {
		PROFILE_INFO_TYPE_CPU_UPDATE_TIME = 0,
		PROFILE_INFO_TYPE_CPU_RENDER_TIME = 1,
		PROFILE_INFO_TYPE_CPU_FRAME_TIME = 2,
		PROFILE_INFO_TYPE_CPU_COUNT = 3
	};

	struct roProfilerInfo {

		UINT64 TimeStampStart;
		UINT64 TimeStampEnd;
		float TimeElapsed;
		bool IsRecording = false;
		bool Activated	= false;
	};
	//gloabals

	extern int									g_NumCPUProfiles;
	extern int									g_NumGPUProfiles;
	extern ID3D12QueryHeap*						g_TimeStampQueryHeap;
	extern roReadBackBuffer*					g_TimeSamplesGPUBuffer;
	extern std::vector<roProfilerInfo>			g_CPUProfiles;
	extern std::vector<roProfilerInfo>			g_GPUProfiles;
	extern double								g_GPUSecondsPerCount;
	extern double								g_CPUSecondsPerCount;


	class roGPUProfiler {
	public:
		roGPUProfiler() = delete;
		roGPUProfiler& operator=(const roGPUProfiler& rhs) = delete;
		roGPUProfiler(ID3D12CommandQueue* cmdQueue);

		inline void Begin(roGPUProfileInfoType profInfoType, ID3D12GraphicsCommandList* cmdList) {

			auto &profileInfo = g_GPUProfiles[(UINT)profInfoType];
			UINT profileIdx = (UINT)profInfoType;

			if (profileInfo.IsRecording) {
				THROW(L"Cannot call profiler : begin Twice");
			}

			cmdList->EndQuery(g_TimeStampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 2 * profileIdx);

			profileInfo.IsRecording = true;
			profileInfo.Activated	= true;
		}

		inline void End(roGPUProfileInfoType profInfoType, ID3D12GraphicsCommandList* cmdList, int frameIdx) {
			auto &profileInfo = g_GPUProfiles[(int)profInfoType];
			int profileIdx = (int)profInfoType;

			if (!profileInfo.IsRecording) {
				THROW(L"Please call Begin to start recording");
			}

			cmdList->EndQuery(g_TimeStampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, (UINT)profInfoType * 2 + 1);

			uint64_t dstOffset = ((frameIdx * g_NumGPUProfiles * 2) + (UINT)profInfoType * 2) * sizeof(uint64_t);

			cmdList->ResolveQueryData(g_TimeStampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, (UINT)profInfoType * 2, 2,
				g_TimeSamplesGPUBuffer->GetResource(), dstOffset);

			profileInfo.IsRecording = false;
		}

	};

	static void InitializeGPUProfiles(ID3D12Device* device, int numFrames) {
		g_NumGPUProfiles = (int)PROFILE_INFO_TYPE_GPU_COUNT;

		D3D12_QUERY_HEAP_DESC heapDesc{};
		heapDesc.Count = g_NumGPUProfiles * 2;
		heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		heapDesc.NodeMask = 0;

		HR(device->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&g_TimeStampQueryHeap)));

		g_TimeSamplesGPUBuffer = new roReadBackBuffer(g_NumGPUProfiles * sizeof(uint64_t) * 2 * numFrames, sizeof(uint64_t));

		for (int i = 0; i < g_NumGPUProfiles; i++)
			g_GPUProfiles.push_back(roProfilerInfo());
	}

	static void ResolveGPUProfileStats(roGPUProfileInfoType profInfoType, int completedFrameIdx) {

		auto &profileInfo = g_GPUProfiles[(int)profInfoType];
		int profileIdx = (int)profInfoType;

		if (!profileInfo.Activated)
			return;

		if (profileInfo.IsRecording == true && profileInfo.Activated != true) {
			THROW(L"call end to stop profiling")
		}

		UINT BufferIdx = (completedFrameIdx * g_NumGPUProfiles) * 2 + (profileIdx * 2);

		uint64_t gpuCounter(0);

		g_TimeSamplesGPUBuffer->GetData(BufferIdx, 1, &gpuCounter);
		profileInfo.TimeStampStart = gpuCounter;
		g_TimeSamplesGPUBuffer->GetData(BufferIdx + 1, 1, &gpuCounter);
		profileInfo.TimeStampEnd = gpuCounter;

		profileInfo.TimeElapsed = (float)(g_GPUSecondsPerCount * (double)(profileInfo.TimeStampEnd - profileInfo.TimeStampStart));
		
	}


	static void ShutdownGPUProfiles() {
		g_TimeSamplesGPUBuffer->Destoy();
		SAFE_DELETE(g_TimeSamplesGPUBuffer);
		SAFE_RELEASE(g_TimeStampQueryHeap);

	}


	// CPU Profiles
	static void InitializeCPUProfiles() {
		g_NumCPUProfiles = (int)PROFILE_INFO_TYPE_CPU_COUNT;
		for (int i = 0; i < g_NumCPUProfiles; i++)
			g_CPUProfiles.push_back(roProfilerInfo());
	}


	class roCPUProfiler {
	public:
		roCPUProfiler& operator=(const roCPUProfiler& rhs) = delete;
		roCPUProfiler();

		inline void Begin(roCPUProfileInfoType profInfoType) {

			auto &profileInfo = g_CPUProfiles[(int)profInfoType];
			int profileIdx = (int)profInfoType;

			if (profileInfo.IsRecording) {
				THROW(L"Cannot call profiler : begin Twice");
			}

			UINT64 currCounter(0);
			QueryPerformanceCounter((LARGE_INTEGER*)&currCounter);
			profileInfo.TimeStampStart = currCounter;

			profileInfo.IsRecording = true;
			profileInfo.Activated = true;
		}

		inline void End(roCPUProfileInfoType profInfoType) {
			auto &profileInfo = g_CPUProfiles[(int)profInfoType];
			int profileIdx = (int)profInfoType;

			if (!profileInfo.IsRecording)
				THROW(L"Please call Begin to start recording");

			UINT64 currCounter(0);
			QueryPerformanceCounter((LARGE_INTEGER*)&currCounter);
			profileInfo.TimeStampEnd = currCounter;

			profileInfo.TimeElapsed = (float)((g_CPUSecondsPerCount * (double)(profileInfo.TimeStampEnd - profileInfo.TimeStampStart)));

			profileInfo.IsRecording = false;
		}
	};		
}