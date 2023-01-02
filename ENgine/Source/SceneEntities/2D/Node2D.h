
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class CLASS_DECLSPEC Node2D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(Node2D)

	#ifndef DOXYGEN_SKIP

		Node2D() = default;
		virtual ~Node2D() = default;

		void Init() override;
	#endif
	};
}