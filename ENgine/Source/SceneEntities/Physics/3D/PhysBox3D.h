
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Root/Physics/PhysScene.h"

namespace Orin
{
	/**
	\page scene_object_3D_PhysBox Physic Box

	Box can be static or dynamic. Dimentions are adjustable. Debug render used for
	rendering in a scene. So this object is more for draft construction of levels.

	This class ::PhysBox is a representation on C++ side.

	Parameters
	----------

	Name              | Description
	------------------| -------------
	color             | Display color of a box
	SizeX             | Size along X axis of a box
	SizeY             | Size along Y axis of a box
	SizeZ             | Size along Z axis of a box
	Is Static         | Set if object should be satic or dynamic

	*/


	/**
	\ingroup gr_code_scene_objects_3D
	*/

	/**
	\brief Representation of physical box in 3D space

	Box can be static or dynamic. Dimentions are adjustable. Debug render used for
	rendering in a scene. So this object is more for draft construction of levels.

	*/

	class CLASS_DECLSPEC PhysBox3D : public SceneEntity
	{
	protected:

		uint32_t physGroup;

		/**
		\brief Color of a box.
		*/

		Color color;

		/**
			\brief Visualize box during play
		*/
		bool visibleDuringPlay;

		/**
			\brief Affecting on parent
		*/
		bool affectOnParent = false;

	public:

	#ifndef DOXYGEN_SKIP

		enum class BodyType
		{
			Static = 0,
			Dynamic = 1,
			DynamicCCD = 2,
			Kinematic = 3,
			Trigger = 4
		};

		/**
			\brief Group of a body
		*/

		/**
			\brief Controls if box static or dynamic.
		*/

		BodyType bodyType;

		PhysScene::BodyUserData body;

		META_DATA_DECL(PhysBox3D)

		virtual ~PhysBox3D() = default;

		void Init() override;
		virtual void Draw(float dt);

		void ApplyProperties() override;

		void OnVisiblityChange(bool set) override;
		void SetPhysGroup(int group);

		void Play() override;
		void Release() override;

	#endif
	};
}
