
#pragma once

#include "Root/Assets/Assets.h"
#include "Root/Controls/Controls.h"
#include "Root/Files/Files.h"
#include "Root/Fonts/Fonts.h"
#include "Root/Memory/MemoryManager.h"
#include "Root/Render/Render.h"
#include "Root/Scenes/SceneManager.h"
#include "Root/TaskExecutor/TaskExecutor.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_root
	*/

	/**
	\brief Root

	This is a main object which allows to access to every subsytem of the engine.
	Engine supports multi logs, i.e logs from differentent entities can be targeted to a seprated
	log file. Logs on Windows stored in subfolder "From dd_mm_yyyy hh_mm" in directore Logs.

	*/

	class Root
	{
		#ifndef DOXYGEN_SKIP

		#ifdef PLATFORM_WIN
		char  logsDir[1024];
		float dt = 0.0f;
		#endif

		#ifdef PLATFORM_WIN
		eastl::string rootPath;
		bool redirectLog = true;
		#endif

		#endif

	public:

		/**
			\brief Access to Assets
		*/
		Assets assets;

		/**
		\brief Access to controls service
		*/
		Controls controls;

		/**
		\brief Access to files service
		*/
		Files files;

		/**
		\brief Access to fonts service
		*/
		Fonts fonts;

		/**
		\brief Access to memory manger
		*/
		MemoryManager memory;

		/**
		\brief Access to render service
		*/
		Render render;

		/**
		\brief Access to scene manager
		*/
		SceneManager scenes;

		/**
		\brief Access to task executor service
		*/
		TaskExecutor taskExecutor;

		/**
		\brief Should be childs clipped by size of a widget

		\param[in] name a name of log file
		\param[in] text formated string of output message

		*/
		void Log(const char* name, const char* text, ...);

		#ifndef DOXYGEN_SKIP

		Root();
		~Root() = default;
		bool Init(void* renderData);

		void Update();
		void CountDeltaTime();
		float GetDeltaTime();
		int GetFPS();

		void SetRootPath(const char* rootPath);
		const char* GetRootPath();

		#endif

		void Release();
	};

	extern Root root;
}
