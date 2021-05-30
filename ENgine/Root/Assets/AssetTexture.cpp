#include "Root/Root.h"
#include "SceneEntities/2D/SpriteEntity.h"

namespace Oak
{
#ifdef OAK_EDITOR
	extern void ShowSpriteWindow(AssetTexture* texture);
#endif

	CLASSREG(Asset, AssetTexture, "AssetTexture")

	META_DATA_DESC(AssetTexture)
		ENUM_PROP(AssetTexture, textureFilter, TextureFilter::Linear, "Properties", "Texture filter", "Texture filter")
			ENUM_ELEM("Point", TextureFilter::Point)
			ENUM_ELEM("Linear", TextureFilter::Linear)
		ENUM_END
		ENUM_PROP(AssetTexture, texturMode, TextureAddress::Wrap, "Properties", "Texture filter", "Texture filter")
			ENUM_ELEM("Wrap", TextureAddress::Wrap)
			ENUM_ELEM("Clamp", TextureAddress::Clamp)
			ENUM_ELEM("Mirror", TextureAddress::Mirror)
		ENUM_END
		CALLBACK_PROP(AssetTexture, AssetTexture::StartEditAssetTexture, "Properties", "Sprite Editor")
	META_DATA_DESC_END()

	void AssetTexture::StartEditAssetTexture(void* owner)
	{
#ifdef OAK_EDITOR
		ShowSpriteWindow((AssetTexture*)owner);
#endif
	}

	TextureRef AssetTexture::GetTexture()
	{
		return spriteSheet.texture;
	}

	void AssetTexture::Reload()
	{
		if (spriteSheet.texture.Get())
		{
			root.render.LoadTexture(spriteSheet.texture, path.c_str());
		}
		else
		{
			spriteSheet.texture = root.render.LoadTexture(path.c_str(), _FL_);
		}

		if (spriteSheet.texture.Get())
		{
			spriteSheet.texture->SetFilters(textureFilter, textureFilter);
			spriteSheet.texture->SetAdress(texturMode);
			spriteSheet.size = Math::Vector2((float)spriteSheet.texture->GetWidth(), (float)spriteSheet.texture->GetHeight());
		}
	}

	void AssetTexture::LoadData(JsonReader& loader)
	{
		Sprite::Load(loader, &spriteSheet, "spriteSheet");
	}

	#ifdef OAK_EDITOR
	void AssetTexture::SaveData(JsonWriter& saver)
	{
		Sprite::Save(saver, &spriteSheet, "spriteSheet");
	}

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