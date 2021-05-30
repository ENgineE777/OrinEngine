#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class AssetTexture : public Asset
	{
		friend class PointerRef<AssetTexture>;

		TextureFilter textureFilter = TextureFilter::Linear;
		TextureRef texture;

	public:

		Sprite::Data spriteData;

		META_DATA_DECL_BASE(AssetTexture)

		TextureRef GetTexture();
		void Reload() override;

		#ifdef OAK_EDITOR
		virtual const char* GetSceneEntityType();
		virtual void SetupCreatedSceneEntity(SceneEntity* entity);
		#endif
	};

	typedef PointerRef<AssetTexture> AssetTextureRef;
}