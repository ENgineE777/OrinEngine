
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class LabelWidget : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(LabelWidget)

	#ifndef DOXYGEN_SKIP

		LabelWidget();
		virtual ~LabelWidget() = default;

		void Init() override;
	#endif
	};
}