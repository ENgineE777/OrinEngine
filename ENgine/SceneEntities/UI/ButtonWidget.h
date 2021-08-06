
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class ButtonWidget : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(ButtonWidget)

	#ifndef DOXYGEN_SKIP

		ButtonWidget();
		virtual ~ButtonWidget() = default;

		void Init() override;
	#endif
	};
}