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
		if (texture.Get())
		{
			root.render.LoadTexture(texture, path.c_str());
		}
		else
		{
			texture = root.render.LoadTexture(path.c_str(), _FL_);
		}
	}

	void AssetTexture::Release()
	{

	}
};