#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class AssetTexture : public Asset
	{
		friend class PointerRef<AssetTexture>;

		TextureFilter textureFilter = TextureFilter::Linear;
		TextureRef texture;

	public:

		META_DATA_DECL_BASE(AssetTexture)

		TextureRef GetTexture();
		void Reload() override;
	};

	typedef PointerRef<AssetTexture> AssetTextureRef;
}