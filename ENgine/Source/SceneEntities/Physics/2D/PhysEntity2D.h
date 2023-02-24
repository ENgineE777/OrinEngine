
#pragma once

#include "SceneEntities/Physics/PhysEntityBase.h"

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

	class CLASS_DECLSPEC PhysEntity2D : public PhysEntityBase
	{
	public:

	#ifndef DOXYGEN_SKIP

		META_DATA_DECL(PhysEntity2D)

		virtual ~PhysEntity2D() = default;

		void Init() override;
		void Play() override;
		void Draw(float dt);

	#endif
	};
}
