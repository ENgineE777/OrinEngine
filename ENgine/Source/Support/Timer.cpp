#include "Timer.h"
#include <stdio.h>
#include "stb_sprintf.h"

#ifdef PLATFORM_WIN
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <Windows.h>
#endif

namespace Orin::Timer
{
	#ifdef PLATFORM_WIN
	static BOOL g_first_time = 1;
	static LARGE_INTEGER g_counts_per_sec;

	int clock_gettime(struct timespec *ct)
	{
		LARGE_INTEGER count;

		if (g_first_time)
		{
			g_first_time = 0;

			if (0 == QueryPerformanceFrequency(&g_counts_per_sec))
			{
				g_counts_per_sec.QuadPart = 0;
			}
		}

		if (g_counts_per_sec.QuadPart <= 0 || 0 == QueryPerformanceCounter(&count))
		{
			return -1;
		}

		ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
		ct->tv_nsec = (long)(((count.QuadPart % g_counts_per_sec.QuadPart) * 1E9) / g_counts_per_sec.QuadPart);

		return 0;
	}
	#endif

	struct timespec lastTimeSpec;
	double lastTime = -1.0;
	float  deltaTime = 0.0f;
	int    lastFps = 0;
	int    curFps = 0;
	float  fpsTimer = 0.0f;
	double totalTime = 0.0;

	int   stampMin = 0;
	float stampSec = 0;
	char  stampStr[32] = "00:00 - ";

	float CountDeltaTime()
	{
		struct timespec currTime;

		#ifdef PLATFORM_WIN
		clock_gettime(&currTime);
		#endif

		if (lastTime < -0.5)
		{
			deltaTime = 0.0f;
		}
		else
		{
			long iNanoSeconds = (currTime.tv_sec > lastTimeSpec.tv_sec) ? (1000000000 - lastTimeSpec.tv_nsec) + currTime.tv_nsec : currTime.tv_nsec - lastTimeSpec.tv_nsec;
			deltaTime = (float)iNanoSeconds / 1000000000.0f;
		}

		lastTimeSpec = currTime;

		totalTime += deltaTime;
		stampSec += deltaTime;

		if (stampSec > 60.0f)
		{
			int num = (int)(stampSec / 60.0f);
			stampMin += num;
			stampSec -= num * 60.0f;
		}

		stbsp_snprintf(stampStr, 32, "%i:%2.3f - ", stampMin, stampSec);

		if (deltaTime < 0.0f) deltaTime = 0.0f;

		if (deltaTime > 0.3f) deltaTime = 0.3f;

		curFps++;
		fpsTimer += deltaTime;

		if (fpsTimer > 1.0f)
		{
			lastFps = curFps;
			curFps = 0;
			fpsTimer -= 1.0f;
		}

		lastTime = 0;

		return deltaTime;
	}

	int GetFPS()
	{
		return lastFps;
	}

	float GetDeltaTime()
	{
		return deltaTime;
	}

	const char* GetTimeStamp()
	{
		return stampStr;
	}
}
