#include "Root/Root.h"

namespace Oak
{
	CLASSREG(Asset, AssetTexture, "AssetTexture")

	const TextureRef& AssetTexture::GetTexture()
	{
		return texture;
	}

	void AssetTexture::Reload()
	{
		texture = root.render.LoadTexture(path.c_str(), _FL_);
	}

	void AssetTexture::Release()
	{

	}
};