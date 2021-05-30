#include "Root/Root.h"
#include "SceneEntities/2D/SpriteEntity.h"

namespace Oak
{
#ifdef OAK_EDITOR
	extern void ShowSpriteWindow(Sprite::Data* data);
#endif

	void StartEditAssetTexture(void* owner)
	{
#ifdef OAK_EDITOR
		AssetTexture* asset = (AssetTexture*)owner;
		ShowSpriteWindow(&asset->spriteData);
#endif
	}

	CLASSREG(Asset, AssetTexture, "AssetTexture")

	META_DATA_DESC(AssetTexture)
		ENUM_PROP(AssetTexture, textureFilter, TextureFilter::Linear, "Properties", "Texture filter", "Texture filter")
			ENUM_ELEM("Point", TextureFilter::Point)
			ENUM_ELEM("Linear", TextureFilter::Linear)
		ENUM_END
		CALLBACK_PROP(AssetTexture, StartEditAssetTexture, "Properties", "Sprite Editor")
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
			spriteData.texName = name;
			spriteData.texture = texture;
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

			char str[256];
			StringUtils::Copy(str, 256, name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}
	#endif
};