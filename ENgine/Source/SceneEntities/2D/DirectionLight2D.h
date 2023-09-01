
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class DirectionLight2D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(DirectionLight2D)

		Color color;
		float intesity = 1.0f;
		int lightGroup = 1;

	#ifndef DOXYGEN_SKIP

		DirectionLight2D();
		virtual ~DirectionLight2D() = default;

		void Init() override;
		void Draw(float dt);

		void GetBBox(Math::Vector3& vMin, Math::Vector3& vMax) override;
	#endif
	};
}