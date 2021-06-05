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

			if (slice.isNineSliced)
			{
				float x[] = { 0, slice.upLeftOffset.x, size.x - slice.downRightOffset.x, size.x };
				float y[] = { 0, slice.upLeftOffset.y, size.y - slice.downRightOffset.y, size.y };

				float u[] = { 0, slice.upLeftOffset.x, slice.size.x - slice.downRightOffset.x, slice.size.x };
				float v[] = { 0, slice.upLeftOffset.y, slice.size.y - slice.downRightOffset.y, slice.size.y };

				int index = 0;
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
					{
						Math::Vector2 slicePos = pos + Math::Vector2(x[j], y[i]);
						Math::Vector2 sliceSize = Math::Vector2(x[j + 1] - x[j], y[i + 1] - y[i]);

						Math::Vector2 sliceUVPos = slice.pos + Math::Vector2(u[j], -v[i]);
						Math::Vector2 sliceUVSize = Math::Vector2(u[j + 1] - u[j], v[i + 1] - v[i]);

						Sprite::Draw(Get()->texture, clr, local_trans, slicePos, sliceSize, sliceUVPos / Get()->size, sliceUVSize / Get()->size);
						index++;
					}
			}
			else
			{
				Sprite::Draw(Get()->texture, clr, local_trans, pos, size, Math::Vector2(slice.pos.x, slice.pos.y) / Get()->size, slice.size / Get()->size);
			}
		}
	}

	void AssetTextureRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		SpriteEntity* sprite = reinterpret_cast<SpriteEntity*>(entity);

		if (sprite)
		{
			sprite->texture = *this;

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			if (sliceIndex != -1 && sliceIndex < Get()->slices.size())
			{
				StringUtils::Cat(str, 256, Get()->slices[sliceIndex].name.c_str());
			}

			entity->SetName(str);
		}
	}

	Math::Vector2 AssetTextureRef::GetSize()
	{
		if (sliceIndex == -1 || sliceIndex >= Get()->slices.size())
		{
			return Get()->size;
		}
		
		AssetTexture::Slice& slice = Get()->slices[sliceIndex];
		return slice.size;
	}
};