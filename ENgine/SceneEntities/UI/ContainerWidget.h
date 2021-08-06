
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class ContainerWidget : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(ContainerWidget)

	#ifndef DOXYGEN_SKIP

		ContainerWidget();
		virtual ~ContainerWidget() = default;

		void Init() override;
	#endif
	};
}