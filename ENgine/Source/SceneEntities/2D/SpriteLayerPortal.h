
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class SpriteLayerPortal : public SceneEntity
	{
	public:		

		META_DATA_DECL_BASE(SpriteLayerPortal)

	#ifndef DOXYGEN_SKIP

		SpriteLayerPortal();
		virtual ~SpriteLayerPortal() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
		void EditorDraw(float dt);
	#endif
	};
}