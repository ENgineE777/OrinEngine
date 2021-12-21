
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData.h"
#include "Scene.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include "Root/Files/Files.h"
#include "Support/Transform.h"
#include <typeinfo>

namespace Oak
{
	/**
	\ingroup gr_code_root_scene
	*/

	/**
	\brief SceneEntity

	This is base class of an scene entity.

	*/

	class CLASS_DECLSPEC SceneEntity : public Object
	{
		friend class Scene;

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

	#ifdef OAK_EDITOR
		bool edited = false;
	#endif

	public:

	#ifndef DOXYGEN_SKIP

		eastl::string groupName;

		const char* className = nullptr;
		const char* scriptClassName = nullptr;

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

	#ifdef OAK_EDITOR
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

		virtual bool CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir);

		virtual void OnMouseMove(Math::Vector2 ms);
		virtual	void OnLeftMouseDown();
		virtual void OnLeftMouseUp();
		virtual	void OnRightMouseDown();
		virtual void OnRightMouseUp();
	#endif

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
			\brief Get parent
			\return Pointer to a parent
		*/
		virtual SceneEntity* GetParent();

		/**
			\brief Get list of childs

			\return return list of childs
		*/
		virtual const eastl::vector<SceneEntity*>& GetChilds();

		/**
			\brief Fin a child by UID
			\param[in] uid UID of a child
			\return Return pointer to a child
		*/
		SceneEntity* FindChild(uint32_t uid);

		SceneEntity* FindChild(const char* name);

		template<class T>
		T* FindChild()
		{
			for (auto entity : childs)
			{
				T* casted = dynamic_cast<T*>(entity);

				if (casted)
				{
					return casted;
				}

				T* childCasted = entity->FindChild<T>();

				if (childCasted)
				{
					return childCasted;
				}
			}

			return nullptr;
		}

		SceneEntity* FindEntity(uint32_t uid);

		/**
		\brief Register script class for a scene object
		*/
		virtual void BindClassToScript();

		/**
		\brief Inject properties into script property

		\param[in] type Type of a script property
		\param[in] property Pointer to a script property
		\param[in] prefix Prefix which will be used to names of fileds

		\return True will be returned in case injection was successfully.
		*/
		virtual bool InjectIntoScript(const char* typeName, int name, void* property, const char* prefix);


		struct Holder
		{
			std::size_t typeHash;
			void* ptr = nullptr;
		};

		eastl::map<eastl::string, Holder> callbacks;
		
		template<class T>
		void RegisterCallback(eastl::string name, T callback)
		{
			auto typeHash = typeid(T).hash_code();

			auto iter = callbacks.find(name);

			if (iter == callbacks.end())
			{
				Holder& holder = callbacks[name];

				holder.typeHash = typeHash;
				T* call = new T();
				*call = callback;
				holder.ptr = call;
			}
		}

		template<class T>
		T GetCallback(eastl::string name)
		{
			auto typeHash = typeid(T).hash_code();

			auto iter = callbacks.find(name);

			if (iter != callbacks.end())
			{
				if ((*iter).second.typeHash == typeHash)
				{
					return *((T*)((*iter).second.ptr));
				}
			}

			return T();
		}

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
		virtual void SetEntity(SceneEntity* setEntity) override
		{
			entity = dynamic_cast<T*>(setEntity);
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

		virtual SceneEntity* GetSceneEntity() override { return entity; };
	};

	CLASSFACTORYDEF(SceneEntity)
	CLASSFACTORYDEF_END()

	#define BASE_SCENE_ENTITY_STATE_PROP(className)\
	BOOL_PROP(className, visible, true, "Common", "Visibile", "State of visibility of an object")

	#define BASE_SCENE_ENTITY_PROP(className)\
	STRING_PROP(className, name, "SceneEntity", "Common", "Name")\
	BASE_SCENE_ENTITY_STATE_PROP(className)\
	TRANSFORM_PROP(className, transform, "Transform")
}