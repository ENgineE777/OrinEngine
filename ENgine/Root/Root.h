
#pragma once

namespace Oak
{
	class Root
	{
		#ifdef PLATFORM_WIN
		char  logsDir[1024];
		float dt = 0.0f;
		#endif

	public:

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
