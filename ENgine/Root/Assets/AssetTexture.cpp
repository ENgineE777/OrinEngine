#include "Root/Root.h"
#include "SceneEntities/2D/SpriteEntity.h"

namespace Oak
{
	CLASSREG(Asset, AssetTexture, "AssetTexture")

	META_DATA_DESC(AssetTexture)
		ENUM_PROP(AssetTexture, textureFilter, TextureFilter::Linear, "Property", "Texture filter", "Texture filter")
			ENUM_ELEM("Point", TextureFilter::Point)
			ENUM_ELEM("Linear", TextureFilter::Linear)
		ENUM_END
	META_DATA_DESC_END()

	TextureRef AssetTexture::GetTexture()
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

		if (texture.Get())
		{
			texture->SetFilters(textureFilter, textureFilter);
		}
	}

	#ifdef OAK_EDITOR
	const char* AssetTexture::GetSceneEntityType()
	{
		return "Sprite";
	}

	void AssetTexture::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		SpriteEntity* sprite = reinterpret_cast<SpriteEntity*>(entity);

		if (sprite)
		{
			sprite->texture = PointerRef<AssetTexture>(this, _FL_);
		}
	}
	#endif
};