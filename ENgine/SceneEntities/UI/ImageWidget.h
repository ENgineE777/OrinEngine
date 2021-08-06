
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class ImageWidget : public SceneEntity
	{
	public:

		AssetTextureRef texture;
		META_DATA_DECL_BASE(ImageWidget)

	#ifndef DOXYGEN_SKIP

		ImageWidget();
		virtual ~ImageWidget() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}