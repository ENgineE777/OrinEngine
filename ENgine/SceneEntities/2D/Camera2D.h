
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"

namespace Oak
{
	class Camera2D : public SceneEntity
	{
		Math::Matrix view;

	public:

		META_DATA_DECL_BASE(Camera3D)

		float zoom = 1.0f;
		Math::Vector2 border;

		bool useLimits = false;
		Math::Vector2 leftup;
		Math::Vector2 rightdown;
		SceneEntityRef<SceneEntity> targetRef;

#ifndef DOXYGEN_SKIP

		Camera2D();
		virtual ~Camera2D() = default;

		void Init() override;
		void Update(float dt);

#endif
	};
}
