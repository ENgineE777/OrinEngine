
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData.h"
#include "Scene.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include "Root/Files/Files.h"
#include "Support/Transform.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_scene
	*/

	/**
	\brief SceneEntity

	This is base class of an scene entity.

	*/

	class SceneEntity : public Object
	{
		friend class Scene;

	private:
		Scene* scene = nullptr;

	public:
		enum class State
		{
			Invisible,
			Inactive,
			Active
		};

	protected:

		eastl::string name;
		eastl::string layerName;
		uint32_t uid = 0;
		State state = State::Active;
	#ifdef OAK_EDITOR
		bool edited = false;
		TaskExecutor::SingleTaskPool* taskPool = nullptr;
		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
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
		\brief Set state of a scene object

		\param[in] state New state of a scene object
		*/
		virtual void SetState(State state);

		/**
		\brief Get current state of a scene object

		\return Current state of a scene object
		*/
		virtual State GetState();

		/**
		\brief Get transformation of scene entity

		\return Transformation of scene entity
		*/
		virtual Transform* GetTransform();

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
		\brief Save properties into JSON

		\param[in] writer JSON helper class for writing JSON
		*/
		virtual void Save(JsonWriter& writer);

		/**
		\brief Get task pool

		\param[in] editor Should be used scene task pool or created new one which will be used in the editor
		*/
		virtual TaskExecutor::SingleTaskPool* Tasks(bool editor);

		/**
		\brief Get render task pool

		\param[in] editor Should be used scene render task pool or created new one which will be used in the editor
		*/
		virtual TaskExecutor::SingleTaskPool* RenderTasks(bool editor);

		/**
		\brief This method called when scene is starting to play

		\return Result of switching a scene object in palyng state.
		*/
		virtual bool Play();

	#ifdef OAK_EDITOR
		/**
		\brief Set new scene

		\param[in] setScene new scene
		*/
		void SetScene(Scene* setScene);

		/**
		\brief Enable entity's task pools

		\param[in] enable 
		*/
		virtual void EnableTasks(bool enable);

		/**
		\brief Check if entity has own task pools

		\return if entity has own task pools
		*/
		virtual bool HasOwnTasks();

		/**
		\brief Copy data from source entity

		\param[in] sorce Source entity
		*/
		virtual void Copy(SceneEntity* sorce);

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
		\brief This method called when scene resoources are exporting
		*/
		virtual void Export();
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
		\brief Get child scene object by UID

		\param[in] uid UID of a child scene object

		\return Pointer to a child scene object
		*/
		virtual SceneEntity* GetChild(uint32_t uid);

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

		/**
		\brief SceneObject should released only via this method
		*/
		virtual void Release();
	};

	struct SceneObjectRef
	{
		uint32_t uid = 0;
		bool isAsset = false;
		SceneEntity* entity = nullptr;
	};

	CLASSFACTORYDEF(SceneEntity)
	CLASSFACTORYDEF_END()

	#define BASE_SCENE_ENTITY_STATE_PROP(className)\
	ENUM_PROP(className, state, 2, "Common", "State", "State of a object")\
		ENUM_ELEM("Invisible", 0)\
		ENUM_ELEM("Inactive", 1)\
		ENUM_ELEM("Active", 2)\
	ENUM_END

	#define BASE_SCENE_ENTITY_PROP(className)\
	STRING_PROP(className, name, "", "Common", "Name")\
	BASE_SCENE_ENTITY_STATE_PROP(className)
}