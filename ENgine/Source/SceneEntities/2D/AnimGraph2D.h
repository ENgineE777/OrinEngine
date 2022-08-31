
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "root/Assets/AssetAnimGraph2D.h"

namespace Oak
{
	class CLASS_DECLSPEC AnimGraph2D : public SceneEntity
	{
	public:

		Color color;
		AssetAnimGraph2DRef anim;

		META_DATA_DECL_BASE(AnimGraph2D)

	#ifndef DOXYGEN_SKIP

		AnimGraph2D();
		virtual ~AnimGraph2D() = default;

		void Init() override;
		void Draw(float dt);
	#endif
	};
}