
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "Root/Assets/AssetTexture.h"
#include "Root/Fonts/FontRef.h"
#include "ContainerWidget.h"

namespace Orin
{
	class CLASS_DECLSPEC LabelWidget : public ContainerWidget
	{
	public:

		META_DATA_DECL_BASE(LabelWidget)

		FontRef font;

		int fontHeight = 20;
		eastl::string fontName;

		float linesGapMul = 1.0f;
		eastl::string text;

		int screenHeight = -1;
		eastl::vector<FontRes::LineBreak> line_breaks;

	#ifndef DOXYGEN_SKIP

		LabelWidget() = default;;
		virtual ~LabelWidget() = default;

		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}