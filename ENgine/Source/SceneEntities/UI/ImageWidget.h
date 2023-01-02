
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "ContainerWidget.h"

namespace Orin
{
	class CLASS_DECLSPEC ImageWidget : public ContainerWidget
	{
	public:

		Color imageColor = COLOR_WHITE;

		AssetTextureRef texture;
		META_DATA_DECL_BASE(ImageWidget)

	#ifndef DOXYGEN_SKIP

		ImageWidget() = default;;
		virtual ~ImageWidget() = default;

		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}