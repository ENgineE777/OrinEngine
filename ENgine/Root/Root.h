
#pragma once

#include "Root/Files/Files.h"
#include "Root/Fonts/Fonts.h"
#include "Root/Render/Render.h"
#include "Root/TaskExecutor/TaskExecutor.h"

namespace Oak
{
	class Root
	{
		#ifdef PLATFORM_WIN
		char  logsDir[1024];
		float dt = 0.0f;
		#endif

	public:

		Files files;
		Fonts fonts;
		Render render;
		TaskExecutor taskExecutor;

		Root();
		~Root() = default;
		void Init();

		void Log(const char* name, const char* text, ...);

		void Update();
		void CountDeltaTime();
		float GetDeltaTime();
		int GetFPS();

		void Release();
	};

	extern Root root;
}
