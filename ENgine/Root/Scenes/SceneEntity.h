
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

	protected:

		Scene* scene = nullptr;

		eastl::string name;
		eastl::string layerName;
		uint32_t uid = 0;
		bool visible = true;

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
		virtual void SetVisible(bool set);

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
		virtual bool Play();

	#ifdef OAK_EDITOR
		/**
		\brief Set new scene

		\param[in] setScene new scene
		*/
		void SetScene(Scene* setScene);

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
			\brief Check recursevly if an entity is a child

			\param[in] ed Editor mode value

			\return Retirn trun if an entity is a child
		*/
		virtual bool ContainEntity(SceneEntity* entity);

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

	struct SceneEntityRef
	{
		uint32_t uid = 0;
		SceneEntity* entity = nullptr;
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