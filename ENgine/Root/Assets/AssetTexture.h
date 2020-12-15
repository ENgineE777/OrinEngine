#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class AssetTexture : public Asset
	{
		friend class PointerRef<AssetTexture>;

		TextureRef texture;

	public:

		TextureRef GetTexture();
		void Reload() override;
	};

	typedef PointerRef<AssetTexture> AssetTextureRef;
}