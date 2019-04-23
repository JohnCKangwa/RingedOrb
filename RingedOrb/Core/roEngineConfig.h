#pragma once
#include "pch.h"

struct roEngineConfig {

	static std::wstring sm_EngineName;

	static bool sm_FullScreen;
	static bool sm_Vsync;
	static bool sm_4xMSAA;

	static bool sm_EngineAcitiveState;
	static bool sm_CachedActiveState;
};