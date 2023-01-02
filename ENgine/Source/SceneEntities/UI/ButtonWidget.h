
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "ContainerWidget.h"

namespace Orin
{
	class CLASS_DECLSPEC ButtonWidget : public ContainerWidget
	{
	public:

		META_DATA_DECL_BASE(ButtonWidget)

	#ifndef DOXYGEN_SKIP

		ButtonWidget() = default;
		virtual ~ButtonWidget() = default;
	#endif
	};
}