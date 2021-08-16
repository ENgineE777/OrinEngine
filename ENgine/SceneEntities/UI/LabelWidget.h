
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "Root/Assets/AssetTexture.h"
#include "Root/Fonts/FontRef.h"
#include "ContainerWidget.h"

namespace Oak
{
	class LabelWidget : public ContainerWidget
	{
	public:

		META_DATA_DECL_BASE(LabelWidget)

		FontRef font;

		int fontHeight = 20;
		eastl::string fontName;

		eastl::string text;

	#ifndef DOXYGEN_SKIP

		LabelWidget() = default;;
		virtual ~LabelWidget() = default;

		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}