
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"

namespace Oak
{
	class CLASS_DECLSPEC Camera2D : public SceneEntity
	{
		Math::Matrix view;

		void UpdateRenderView();

	public:

		META_DATA_DECL_BASE(Camera2D)

		float zoom = 1.0f;
		Math::Vector2 border;

		bool useLimits = false;
		Math::Vector2 leftup;
		Math::Vector2 rightdown;
		SceneEntityRef<SceneEntity> targetRef;

		Math::Vector2 addPosition;
		float addRotation;

#ifndef DOXYGEN_SKIP

		Camera2D();
		virtual ~Camera2D() = default;

		void Init() override;
		void Play() override;
		void Update(float dt);
		void CenterCamera();
#endif
	};
}
