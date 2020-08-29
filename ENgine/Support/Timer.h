#pragma once

#ifdef PLATFORM_WIN
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <Windows.h>
#endif

namespace Oak::Timer
{
	float CountDeltaTime();
	float GetDeltaTime();
	int GetFPS();
	const char* GetTimeStamp();
}
