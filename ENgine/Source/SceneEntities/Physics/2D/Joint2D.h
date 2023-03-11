
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "PhysEntity2D.h"

namespace Orin
{
	class CLASS_DECLSPEC Joint2D : public SceneEntity
	{
	public:

		float coneLimit = -1.0f;
		SceneEntityRef<PhysEntity2D> targetRef;
		PhysSphericalJoint* joint;

		META_DATA_DECL_BASE(Joint2D)

	#ifndef DOXYGEN_SKIP

		Joint2D() = default;
		virtual ~Joint2D() = default;

		void Init() override;
		void Play() override;

		void CreateJoint();

		void EditorDraw(float dt);

		void Release() override;
	#endif
	};
}