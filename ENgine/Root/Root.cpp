
#include "Root.h"
#include "Support/Timer.h"
#include "Support/StringUtils.h"
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef PLATFORM_WIN
#include <windows.h>
#endif

namespace Oak
{
	Root root;

	Root::Root()
	{
		logsDir[0] = 0;
	}

	void Root::Init()
	{
		srand((unsigned int)time(nullptr));

		#ifdef PLATFORM_WIN
		char curDir[1024];
		GetCurrentDirectoryA(1024, curDir);
		StringUtils::Printf(logsDir, 1024, "%s/Logs", curDir);
		CreateDirectoryA(logsDir, nullptr);

		time_t t = time(0);
		struct tm now;
		localtime_s(&now, &t);

		StringUtils::Printf(logsDir, 1024, "%s/Logs/From %i_%i_%i %i_%i", curDir, now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec);
		CreateDirectoryA(logsDir, nullptr);
		#endif

		files.Init();

		controls.Init("ENgine/Controls/hardware_win", true);
		controls.LoadAliases("ENgine/Controls/engine_aliases");

		render.Init("DX11", nullptr);

		StringUtils::Init();

		Timer::CountDeltaTime();
	}

	void Root::Log(const char* name, const char* text, ...)
	{
		static char buffer[4096];
		va_list args;
		va_start(args, text);

		vsnprintf(buffer, sizeof(buffer) - 4, text, args);

		va_end(args);

		#ifdef PLATFORM_WIN
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");

		char path[1024];
		StringUtils::Printf(path, 1024, "%s/%s.txt", logsDir, name);

		FILE* f = nullptr;
		fopen_s(&f, path, "a");

		if (f)
		{
			fwrite(Timer::GetTimeStamp(), strlen(Timer::GetTimeStamp()), 1, f);
			fwrite(buffer, strlen(buffer), 1, f);
			fwrite("\n", strlen("\n"), 1, f);
			fclose(f);
		}
		#endif
	}

	void Root::CountDeltaTime()
	{
		Timer::CountDeltaTime();
		dt = Timer::GetDeltaTime();
	}

	void Root::Update()
	{
		controls.Update(dt);
		render.Execute(dt);
	}

	float Root::GetDeltaTime()
	{
		return dt;
	}

	int Root::GetFPS()
	{
		return Timer::GetFPS();
	}

	void Root::Release()
	{
	}
}
