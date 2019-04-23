#pragma once
#include "pch.h"

struct roTimer {
public:
	static void Initialize() {
		if (sm_IsCreated)
			return;

		sm_DeltaTime = -1.0f;

		sm_CurrentTime = 0;
		int64_t countsPerSecond;

		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
		sm_SecondsPerCount = 1.0f / (float)countsPerSecond;

		uint64_t currtime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currtime);
		sm_BaseTime = currtime;
		sm_PreviousTime = currtime;

		sm_IsCreated = true;
	}
	static void Update() {

		QueryPerformanceCounter((LARGE_INTEGER*)&sm_CurrentTime);
		sm_DeltaTime = (sm_CurrentTime - sm_PreviousTime) * sm_SecondsPerCount;
		sm_PreviousTime = sm_CurrentTime;

		if (sm_DeltaTime < 0.0f) {
			sm_DeltaTime = 0.0f;
		}

	}
	static float DeltaTime() {
		return sm_DeltaTime;
	}

	static float TotalTime() {
		QueryPerformanceCounter((LARGE_INTEGER*)&sm_CurrentTime);
		return (float)(sm_CurrentTime - sm_BaseTime) * sm_SecondsPerCount;
	}

private:
	static uint64_t sm_BaseTime;
	static uint64_t sm_CurrentTime;
	static uint64_t sm_PreviousTime;
	static float sm_SecondsPerCount;
	static float sm_DeltaTime;
	static bool sm_IsCreated;
};
