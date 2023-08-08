
#pragma once


#include "Root/IRoot.h"
#include "Root/Assets/Assets.h"
#include "Root/Controls/Controls.h"
#include "Root/Files/Files.h"
#include "Root/Fonts/Fonts.h"
#include "Root/Memory/MemoryManager.h"
#include "Root/Meshes/Meshes.h"
#include "Root/Particles/Particles.h"
#include "Root/Render/Render.h"
#include "Root/Physics/Physics.h"
#include "Root/Scenes/SceneManager.h"
#include "Root/Scripts/Scripts.h"
#include "Root/Sounds/Sounds.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include "Root/CrashHandler.h"

namespace Orin
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

	class Root : public IRoot
	{
		#ifndef DOXYGEN_SKIP

		#ifdef PLATFORM_WIN
		char  logsDir[1024];
		float dt = 0.0f;
		float dtScale = 1.f;
		#endif

		eastl::string rootPath;
		eastl::string rootPathAssets;
		eastl::string rootPathBinaries;
		eastl::string rootPathVcProj;
		#ifdef PLATFORM_WIN
		bool redirectLog = true;
		#endif

		PhysScene* pscene = nullptr;
		bool quitRequested = false;

		#endif

	public:

		/**
			\brief Access to Assets
		*/
		Assets assets;

		Assets* GetAssets() override { return &assets; };

		Controls controls;

		Controls* GetControls() override { return &controls; };

		/**
		\brief Access to files service
		*/
		Files files;

		/**
		\brief Access to fonts service
		*/
		Fonts fonts;

		Fonts* GetFonts() override { return &fonts; };

		/**
		\brief Access to fonts service
		*/
		Localization localization;
		Localization* GetLocalization() override  { return &localization; };

		/**
		\brief Access to memory manger
		*/
		MemoryManager memory;

		/**
		\brief Access to meshes service
		*/
		Meshes meshes;

		Meshes* GetMeshes() override { return &meshes; };

		/**
		\brief Access to render service
		*/
		Render render;

		Render* GetRender() override { return &render; };

		/**
		\brief Access to particles service
		*/
		Particles particles;

		Physics physics;

		PhysScene* GetPhysScene() override { return pscene; };

		/**
		\brief Access to scene manager
		*/
		SceneManager scenes;

		SceneManager* GetSceneManager() override { return &scenes; };

		/**
			\brief Access to scipts manager
		*/
		Scripts scripts;

		/**
			\brief Access to sound manager
		*/
		Sounds sounds;

		virtual Sounds* GetSounds() override { return &sounds; };

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

		CrashHandler crashHandler;

		#ifndef DOXYGEN_SKIP

		Root();
		~Root() = default;
		bool Init(void* renderData);

		void Update();
		void CountDeltaTime();
		float GetDeltaTime();
		int GetFPS();

		void PreparePhysScene();
		void ClearPhysScene();
	
		void SetRootPath(const char* rootPath);
		const char* GetPath(Path path) override;

		#endif

		void SetQuitRequest(bool set) override { quitRequested = set; };
		bool IsQuitRequested() override { return quitRequested; };

		void Release();
	};

	#ifdef OAK_EXPORTING
	extern Root root;
	#endif
}
