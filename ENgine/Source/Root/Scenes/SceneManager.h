
#pragma once

#include "Scene.h"

namespace Oak
{
	/**

	\ingroup gr_code_root_scene
	*/

	/**
	\brief SceneManger

	This manager handles loading and unloading scenes according data from a project.
	Scenes in a project have different pathes but to load/unload scene a file name without
	extenstion is needed.
	*/

	class CLASS_DECLSPEC SceneManager : public Object
	{
		friend class SceneEntity;
		friend class Assets;
		friend class Editor;

		struct SceneHolder
		{
			eastl::string path;
			Scene* scene = nullptr;
		};

		eastl::vector<SceneHolder*> orderToUnload;
		eastl::vector<SceneHolder*> scenesToLoad;
		eastl::vector<SceneHolder*> scenesToDelete;

		eastl::map<eastl::string, SceneHolder> scenes;

		TaskExecutor::GroupTaskPool* groupTaskPool;

		void LoadScene(SceneHolder* holder);
		void UnloadScene(SceneHolder* holder);

		void RegisterScene(const eastl::string& path, const eastl::string& name);
		uint16_t GenerateUID();

	public:

	#ifndef DOXYGEN_SKIP
		void Init();
		void LoadProject(const char* projectName);

		TaskExecutor::SingleTaskPool* AddTaskPool(const char* file, int line);
		void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

		void Execute(float dt);
		void UnloadAll();
	#endif

		/** 
		\brief Load a scene

		\param[in] name Name of a scene (filename without extension)
		*/
		void LoadScene(const char* name);

		/**
		\brief Get a scene

		\param[in] name Name of a scene (filename without extension)
		*/
		Scene* GetScene(const char* name);

		/**
		\brief Set visibility for scene objects from scene groups in all loaded scenes

		\param[in] group Name of a group
		\param[in] state State which will be set for secene objects
		*/
		void SetScenesGroupsVisibilty(const char* group, bool set);

		/**
		\brief Unload a scene

		\param[in] name Name of a scene (filename without extension)
		*/
		void UnloadScene(const char* name);

		#ifndef DOXYGEN_SKIP
		void Release();
		#endif
	};
}