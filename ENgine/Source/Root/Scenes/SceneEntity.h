
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData.h"
#include "Scene.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include "Root/Files/Files.h"
#include "Support/Transform.h"
#include <typeinfo>
#include "Scene.h"
#include "Root/Assets/AssetPrefab.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_scene
	*/

	/**
	\brief SceneEntity

	This is base class of an scene entity.

	*/

	struct SceneEntityRefBase;

	class CLASS_DECLSPEC SceneEntity : public Object
	{
		friend class Scene;
		friend class AssetScene;
		friend class AssetPrefab;
		friend struct SceneEntityRefBase;

	protected:

		Scene* scene = nullptr;

		eastl::string name;
		eastl::string layerName;
		uint32_t uid = 0;
		bool visible = true;
		bool currentVisible = false;

		Transform transform;

		SceneEntity* parent = nullptr;
		eastl::vector<SceneEntity*> childs;

		AssetPrefabRef prefabRef;

#ifdef ORIN_EDITOR
		bool edited = false;
#endif

		struct Delegate
		{
			eastl::string name;
			std::size_t typeHash;
			void* owner;
			uint8_t data[256];
		};

		eastl::vector<Delegate> delegates;

	public:

#ifndef DOXYGEN_SKIP

		eastl::string groupName;

		const char* className = nullptr;
		const char* scriptClassName = nullptr;

		bool prefabInstance = false;

		SceneEntity() = default;
		virtual ~SceneEntity() = default;

		virtual void SetName(const char* name);
		virtual void SetUID(uint32_t uid);

		virtual MetaData* GetMetaData() = 0;
#endif

		/**
		\brief Initialization of a scene object. This is a place where all task should be registered
		*/
		virtual void Init() = 0;

		/**
		\brief Get name of a scene object

		\return Name of a scene object
		*/
		const char* GetName();

		/**
		\brief Get UID of a scene object. This UID is unique among all scene objects in a project

		\return UID of a scene object
		*/
		virtual uint32_t GetUID();

		/**
		\brief Set visibility of a scene object

		\param[in] set New visibility state of a scene object
		*/
		void SetVisiblity(bool set);

		/**
			\brief Update visibility state
		*/
		void UpdateVisibility();

		/**
			\brief Called on changing of visibily

			\param[in] set New visibility state of a scene object
		*/
		virtual void OnVisiblityChange(bool set);

		/**
		\brief Get current visibility state of a scene object

		\return Current visibility state of a scene object
		*/
		virtual bool IsVisible();

		/**
		\brief Get transformation of scene entity

		\return Transformation of scene entity
		*/
		virtual Transform& GetTransform();

		/**
		\brief This method called after each change of properties of scene object via editor
		*/
		virtual void ApplyProperties();

		/**
		\brief Load properties from JSON

		\param[in] reader JSON helper class for reading JSON
		*/
		virtual void Load(JsonReader& reader);

		/**
			\brief Post load after all entities were created
		*/
		virtual void PostLoad();

		/**
		\brief Save properties into JSON

		\param[in] writer JSON helper class for writing JSON
		*/
		virtual void Save(JsonWriter& writer);

		/**
		\brief Get task pool

		\param[in] render Should be used task registered in render pool
		*/
		virtual TaskExecutor::SingleTaskPool* Tasks(bool renderTask = false);


		/**
		\brief This method called when scene is starting to play

		\return Result of switching a scene object in palyng state.
		*/
		virtual void Play();

#ifdef ORIN_EDITOR
		/**
		\brief Set new scene

		\param[in] setScene new scene
		*/
		void SetScene(Scene* setScene);

		/**
		\brief Copy data from source entity

		\param[in] source Source entity
		*/
		virtual void Copy(SceneEntity* source);

		/**
		\brief Enable or disable editor mode

		\param[in] ed Editor mode value
		*/
		virtual void SetEditMode(bool ed);

		/**
		\brief Check if entity in editor mode

		\return if entity in editor mode
		*/
		virtual bool IsEditMode();

		/**
			\brief Check recursevly if an entity is a child

			\param[in] ed Editor mode value

			\return Retirn trun if an entity is a child
		*/
		virtual bool ContainEntity(SceneEntity* entity);

		/**
		\brief This method called when scene resoources are exporting
		*/
		virtual void Export();

		virtual bool ImGuiProperties();
		virtual bool CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir);

		virtual void GetBBox(Math::Vector3& vMin, Math::Vector3& vMax);

		virtual void OnMouseMove(Math::Vector2 ms);
		virtual	void OnLeftMouseDown();
		virtual void OnLeftMouseUp();
		virtual	bool OnRightMouseDown();
		virtual void OnRightMouseUp();
		virtual	void OnMiddleMouseDown();
		virtual void OnMiddleMouseUp();
#endif
		static void OpenSourcePrefab(void* owner);
		static void FillLayersList(eastl::vector<eastl::string>& names);

		/**
		\brief Get pointer to a scene owner

		\return Pointer to a scene owner
		*/
		inline Scene* GetScene()
		{
			return scene;
		}

		/**
			\brief Set parent
			\param[in] Pointer to a parent
		*/
		virtual void SetParent(SceneEntity* parent, SceneEntity* entityBefore = nullptr);

		/**
			\brief Replase child in parent by self
			\param[in] parent Pointer to a parent
			\param[in] index Index of child of a parent to replce self
		*/
		void SetSelfAsChild(SceneEntity* parent, int index);

		/**
			\brief Get parent
			\return Pointer to a parent
		*/
		virtual SceneEntity* GetParent();

		/**
			\brief Get root of prefab instance
			\return Pointer to a root of prefab instance
		*/
		SceneEntity* GetPrefabRoot();

		/**
			\brief Get list of childs

			\return return list of childs
		*/
		virtual eastl::vector<SceneEntity*>& GetChilds();

		/**
			\brief Find a child by UID
			\param[in] uid UID of a child
			\return Return pointer to a child
		*/
		SceneEntity* FindChild(uint32_t uid);

		/**
			\brief Find a child by type
			\param[in] name Name of a child, if not set then just checking for type
			\return Return pointer to a child
		*/
		template<class T>
		T* FindChild(const char* name = nullptr)
		{
			for (auto entity : childs)
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

				T* childCasted = entity->FindChild<T>(name);

				if (childCasted)
				{
					return childCasted;
				}
			}

			return nullptr;
		}

		/**
			\brief Find all childs by type
			\param[out] list of childs of certein type
			\return Return pointer to a child
		*/
		template<class T>
		void FindChilds(eastl::vector<T*>& out)
		{
			for (auto entity : childs)
			{
				T* casted = dynamic_cast<T*>(entity);

				if (casted)
				{
					out.push_back(casted);
				}

				entity->FindChilds<T>(out);
			}
		}

		/**
			\brief Register callback by name
			\param[in] name Name of a callback
			\param[in] callback Provided callback
			\return Return pointer to a child
		*/
		template<class T>
		void AddDelegate(eastl::string name, void* owner, T callback)
		{
			auto typeHash = typeid(T).hash_code();

			bool found = false;

			for (auto& deligate : delegates)
			{
				if (deligate.typeHash == typeHash && StringUtils::IsEqual(deligate.name.c_str(), name.c_str()))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				delegates.push_back(Delegate());

				Delegate& entry = delegates.back();

				entry.name = name;
				entry.typeHash = typeHash;
				entry.owner = owner;
				T* call = (T*)entry.data;
				*call = callback;
			}
		}

		/**
			\brief Get callback by name and type
			\param[in] name Name of a callback
			\return Return a callback
		*/
		template<class T, typename... Args>
		void CallDelegates(eastl::string name, Args&&... args)
		{
			auto typeHash = typeid(T).hash_code();

			for (auto& delegate : delegates)
			{
				if (delegate.typeHash != typeHash || !StringUtils::IsEqual(delegate.name.c_str(), name.c_str()))
				{
					continue;
				}

				T callable = *((T*)delegate.data);
				callable(eastl::forward<Args>(args)...);
			}
		}

		void DeleteDeligates(void* owner)
		{
			for (int index = 0; index < delegates.size(); index++)
			{
				if (delegates[index].owner == owner)
				{
					delegates.erase(delegates.begin() + index);
					index--;
				}
			}
		}

		eastl::vector<SceneEntityRefBase*> referenced;

		/**
		\brief SceneObject should released only via this method
		*/
		virtual void Release();
	};

	struct SceneEntityRefBase
	{
		uint32_t uid = 0;
		virtual void SetEntity(SceneEntity* setEntity) = 0;
		virtual SceneEntity* GetSceneEntity() = 0;
	};

	template<class T>
	struct SceneEntityRef : SceneEntityRefBase
	{
		T* entity = nullptr;
		void SetEntity(SceneEntity* setEntity) override
		{
			if (entity)
			{
				for (int i = 0; i < entity->referenced.size(); i++)
				{
					if (entity->referenced[i] == this)
					{
						entity->referenced.erase(entity->referenced.begin() + i);
						break;
					}
				}
			}

			entity = dynamic_cast<T*>(setEntity);

			if (entity)
			{
				entity->referenced.push_back(this);
			}

			uid = entity ? entity->GetUID() : 0;
		};

		operator bool() const
		{
			return entity != nullptr;
		}

		T* operator->() const
		{
			return entity;
		}

		SceneEntity* GetSceneEntity() override { return entity; };
	};

	CLASSFACTORYDEF(SceneEntity)
		eastl::vector<eastl::string> groups;
		static void SortGrouped()
		{
			auto& decls = GropedDecls();

			for (int i = 0; i < decls.size(); i++)
			{
				if (decls[i]->groups.size() == 0)
				{
					int index = 0;
					const char* group = decls[i]->GetGroup();
					char subGroup[128];
					const char* nextGroup = strstr(group, "/");

					while (nextGroup)
					{
						int len = (int)(nextGroup - group);
						memcpy(subGroup, group, len);
						subGroup[len] = 0;
						decls[i]->groups.push_back(eastl::string(subGroup));

						nextGroup++;
						group = nextGroup;
						nextGroup = strstr(group, "/");
					}

					decls[i]->groups.push_back(eastl::string(group));
				}
			}

			for (int i = 0; i < decls.size() - 1; i++)
			{
				for (int j = i + 1; j < decls.size(); j++)
				{
					bool sameGroup = StringUtils::IsEqual(decls[i]->GetGroup(), decls[j]->GetGroup());

					if ((!sameGroup && !StringUtils::CompareABC(decls[i]->GetGroup(), decls[j]->GetGroup())) || (sameGroup && !StringUtils::CompareABC(decls[i]->GetName(), decls[j]->GetName())))
					{
						ClassFactorySceneEntity* tmp = decls[i];
						decls[i] = decls[j];
						decls[j] = tmp;
					}
				}
			}
		}
		static eastl::vector<ClassFactorySceneEntity*>& GropedDecls()
		{
			static eastl::vector<ClassFactorySceneEntity*> decls;
			if (decls.size() != Decls().size())
			{
				decls = Decls();
			}
			return decls;
		}
		virtual const char* GetGroup() = 0;
	CLASSFACTORYDEF_END()

	#define ENTITYREG(baseClass, className, groupName, shortName)\
	CLASSREGEX(baseClass, className, className, shortName)\
			const char* GetGroup() { return groupName; };\
	CLASSREGEX_END(baseClass, className)

	#define BASE_SCENE_ENTITY_PROP(className)\
	STRING_PROP(className, name, "SceneEntity", "Common", "Name")\
	STRING_ENUM_PROP(className, layerName, SceneEntity::FillLayersList, "Common", "Layer")\
	BOOL_PROP(className, visible, true, "Common", "Visibile", "State of visibility of an object")\
	TRANSFORM_PROP(className, transform, "Transform")\
	CALLBACK_PROP(className, SceneEntity::OpenSourcePrefab, "Common", "Edit Prefab")
}