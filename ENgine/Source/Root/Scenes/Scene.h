
#pragma once

#include "Root/TaskExecutor/TaskExecutor.h"
#include "Root/Files/Files.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_scene
	*/

	/**
	\brief Scene

	This is representation of a scene.

	*/

	class SceneEntity;

	class CLASS_DECLSPEC Scene
	{
		friend class SceneEntity;
		friend class SceneManager;

	public:

		struct Group
		{
			eastl::string name;
			eastl::vector<SceneEntity*> entities;
		};

	#ifndef DOXYGEN_SKIP

	#ifdef ORIN_EDITOR
		char projectScenePath[512];
	#endif

		TaskExecutor::SingleTaskPool* taskPool = nullptr;
		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;

	#endif

	private:

		struct PostPlayDelegate
		{
			int level = 0;
			Object* entity;
			Object::DelegateSimple call;
		};

		eastl::vector<PostPlayDelegate> postPlayList;

		eastl::vector<SceneEntity*> entities;

		bool playing = false;

		eastl::map<eastl::string, Group> groups;

		void DelFromGroup(Group& group, SceneEntity* obj, Scene* newScene = nullptr);

		uint16_t uid = 0;

		char sceneName[512];

		void LoadEntities(JsonReader& reader, const char* name, eastl::vector<SceneEntity*>& entities);
		void SaveEntities(JsonWriter& writer, const char* name, eastl::vector<SceneEntity*>& entities);

	public:

	#ifndef DOXYGEN_SKIP
		Scene() = default;
		virtual ~Scene() = default;

		void Init();
	
		const char* GetName();

		void Clear();
		void Load(const char* name);
		void Save(const char* name);

		void Play();
		void AddPostPlay(int level, Object* entity, Object::DelegateSimple call);

	#ifdef ORIN_EDITOR
		const eastl::vector<SceneEntity*>& GetEntities();

		void Export();
	#endif

		void EnableTasks(bool enable);
	#endif

		/**
		\brief Create a scene entity

		\param[in] name Name of a type
		\param[in] setNameAndUDID Controls if set name and UDID is needed

		\return Pointer to a scene object
		*/
		SceneEntity* CreateEntity(const char* name, bool prefabInstance, bool setNameAndUDID = true);

		/**
		\brief Add entity to a scene

		\param[in] entity Pointer to an entity
		*/
		void AddEntity(SceneEntity* entity, SceneEntity* entityBefore = nullptr);

		/**
		\brief Find a scene entity by UID

		\param[in] uid UID of a scene object

		\return Pointer to a scene object
		*/
		SceneEntity* FindEntity(uint32_t uid);

		/**
		\brief Find a child by type
		\param[in] name Name of a child, if not set then just checking for type
		\return Return pointer to a child
		*/
		template<class T>
		T* FindEntity(const char* name = nullptr, bool searchInChilds = true)
		{
			for (auto entity : entities)
			{
				T* casted = nullptr;

				if (!name || (name && StringUtils::IsEqual(entity->GetName(), name)))
				{
					casted = dynamic_cast<T*>(entity);
				}

				if (casted)
				{
					return casted;
				}

				if (searchInChilds)
				{
					T* childCasted = entity->FindChild<T>(name);

					if (childCasted)
					{
						return childCasted;
					}
				}
			}

			return nullptr;
		}

		/**
		\brief Delete a scene object from a scene

		\param[in] obj Pointer to a scene object
		\param[in] release_obj Should Release be called during deletion
		*/
		void DeleteEntity(SceneEntity* obj, bool releaseObj);

		/**
		\brief Find a scene object in group by name

		\param[in] group_name Name of a group
		\param[in] name Name of a scene object

		\return Pointer to a scene object
		*/
		SceneEntity* FindInGroup(const char* groupName, const char* name);

		/**
		\brief Get Find a scene object in group by name

		\param[out] groups Array with all scene groups which are including included scenes
		\param[in] name Name of a group

		*/
		void GetGroup(eastl::vector<Group*>& groups, const char* name);

		/**
		\brief Adding a scene object to a group

		\param[in] obj Pointer to a scene object
		\param[in] name Name of a group
		*/
		void AddToGroup(SceneEntity * obj, const char* name);

		/**
		\brief Deleting a scene object from a group

		\param[in] obj Pointer to a scene object
		\param[in] name Name of a group
		*/
		void DelFromGroup(SceneEntity * obj, const char* name);

		/**
		\brief Deleting a scene object from all groups

		\param[in] obj Pointer to a scene object
		\param[in] new_scene Moving a scene object to all groups in new scene
		*/
		void DelFromAllGroups(SceneEntity * obj, Scene* newScene = nullptr);

		/**
		\brief Checking if scene is playing

		\return Returns true if a scene is playing. Otherwise false will be returned
		*/
		bool IsPlaying();

	#ifndef DOXYGEN_SKIP

		void GenerateUID(SceneEntity* obj);

		void Release();
	#endif
	};
}
