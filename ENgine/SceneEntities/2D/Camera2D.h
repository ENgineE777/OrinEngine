
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"

namespace Oak
{
	class Camera2D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(Camera3D)

		float zoom = 1.0f;

		Transform transform;
		Math::Matrix view;

#ifndef DOXYGEN_SKIP

		Camera2D();
		virtual ~Camera2D() = default;

		Transform* GetTransform() override;

		void Init() override;
		void Update(float dt);

#endif
	};
}
