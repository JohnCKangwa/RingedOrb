#include "roTimer.h"


uint64_t roTimer::sm_BaseTime = 0;
uint64_t roTimer::sm_CurrentTime = 0;
uint64_t roTimer::sm_PreviousTime = 0;
float roTimer::sm_SecondsPerCount = .0f;
float roTimer::sm_DeltaTime	= .0f;
bool roTimer::sm_IsCreated = false;