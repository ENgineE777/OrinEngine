
#pragma once

#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	/**
	\page scene_object_3D_GenericMarker Generic Marker

	Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
	be finded via call of Scene::GetGroup

	This class ::GenericMarker is a representation on C++ side.

	Parameters
	----------

	Name              | Description
	------------------| -------------
	scene_group       | Name of group which will be used in registration via call Scene::AddToGroup

	*/


	/**
	\ingroup gr_code_scene_objects_3D
	*/

	/**
	\brief Representation of a generic marker in 3D space

	Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
	be finded via call of Scene::GetGroup

	*/

	class CLASS_DECLSPEC GenericMarker : public SceneEntity
	{
	public:

		struct CLASS_DECLSPEC Instance
		{
			META_DATA_DECL_BASE(Instance)

			Transform transform;
			Color color;
			float radius = 1.0f;

			Math::Vector3 GetPosition();
		};

		bool is2D = false;
		bool fullShade = true;
		bool isPath = false;
		eastl::vector<int> mapping;
		eastl::vector<Instance> instances;

		/**
		\brief Name of group which will be used in registration via class Scene::AddToGroup
		*/
		eastl::string sceneGroup;

	#ifndef DOXYGEN_SKIP
		int selInst = -1;
		META_DATA_DECL(GenericMarker)

		void Init() override;
		void ApplyProperties() override;

		void Draw(float dt);

	#ifdef OAK_EDITOR
		//bool CheckSelection(Vector2 ms, Vector3 start, Vector3 dir) override;
		void SetEditMode(bool ed) override;
		void SetGizmo();
	#endif

	#endif
	};
}
