#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class AssetTexture : public Asset
	{
		TextureRef texture;

	public:

		const TextureRef& GetTexture();
		virtual void Reload();
		virtual void Release();
	};
}