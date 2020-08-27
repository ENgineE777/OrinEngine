#pragma once

#ifdef PLATFORM_WIN
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <Windows.h>
#endif

namespace Oak
{
	class Timer
	{
		static struct timespec lastTimeSpec;
		static double lastTime;
		static float deltaTime;
		static int lastFps;
		static int curFps;
		static float fpsTimer;
		static double totalTime;

		static int stampMin;
		static float stampSec;
		static char stampStr[32];

	public:

		static float CountDeltaTime();
		static float GetDeltaTime();
		static int GetFPS();
		static const char* GetTimeStamp();
	};
}
