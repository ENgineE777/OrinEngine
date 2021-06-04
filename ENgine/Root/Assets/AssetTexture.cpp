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
			texture->SetAdress(texturMode);
			size = Math::Vector2((float)texture->GetWidth(), (float)texture->GetHeight());
		}
	}

	void AssetTexture::LoadData(JsonReader& loader)
	{
		if (loader.EnterBlock("spriteSheet"))
		{
			Math::Vector2 size = 0.0f;

			loader.Read("sizeX", size.x);
			loader.Read("sizeY", size.y);

			int count = 1;
			loader.Read("count", count);
			slices.resize(count);

			for (int i = 0; i < count; i++)
			{
				Slice& slice = slices[i];

				loader.EnterBlock("Sheet");

				loader.Read("name", slice.name);
				loader.Read("isNineSliced", slice.isNineSliced);
				loader.Read("pos", slice.pos);
				loader.Read("size", slice.size);
				loader.Read("upLeftOffset", slice.upLeftOffset);
				loader.Read("downRightOffset", slice.downRightOffset);
				loader.Read("offset", slice.offset);

				loader.LeaveBlock();
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetTexture::SaveData(JsonWriter& saver)
	{
		saver.StartBlock("spriteSheet");

		saver.Write("sizeX", size.x);
		saver.Write("sizeY", size.y);

		int count = (int)slices.size();
		saver.Write("count", count);

		saver.StartArray("Sheet");

		for (int i = 0; i < count; i++)
		{
			Slice& slice = slices[i];

			saver.StartBlock(nullptr);

			saver.Write("name", slice.name);
			saver.Write("isNineSliced", slice.isNineSliced);
			saver.Write("pos", slice.pos);
			saver.Write("size", slice.size);
			saver.Write("upLeftOffset", slice.upLeftOffset);
			saver.Write("downRightOffset", slice.downRightOffset);
			saver.Write("offset", slice.offset);

			saver.FinishBlock();
		}

		saver.FinishArray();

		saver.FinishBlock();
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
			sprite->texture = AssetTextureRef(this, _FL_);

			char str[256];
			StringUtils::Copy(str, 256, name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}
	#endif

	void AssetTextureRef::Draw(Transform* trans, Color clr)
	{
		Math::Matrix local_trans = trans->global;
		Math::Vector3 pos3d = Math::Vector3(trans->offset.x, 1.0f - trans->offset.y, trans->offset.z) * trans->size * Math::Vector3(-1.0f, -1.0f, -1.0f);
		Math::Vector2 pos = Math::Vector2(pos3d.x, pos3d.y);
		Math::Vector2 size = Math::Vector2(trans->size.x, trans->size.y);

		if (sliceIndex == -1 || sliceIndex >= Get()->slices.size())
		{
			Sprite::Draw(Get()->texture, clr, local_trans, pos, size, 0.0f, 1.0f);
		}
		else
		{
			AssetTexture::Slice& slice = Get()->slices[sliceIndex];
			Sprite::Draw(Get()->texture, clr, local_trans, pos, size, Math::Vector2(slice.pos.x, Get()->size.y - slice.pos.y) / Get()->size, slice.size / Get()->size);
		}
	}
};