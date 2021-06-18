#include "Root/Root.h"
#include "SceneEntities/2D/AnimGraph2D.h"

namespace Oak
{
#ifdef OAK_EDITOR
	//extern void ShowSpriteWindow(AssetTexture* texture);
#endif

	CLASSREG(Asset, AssetAnimGraph2D, "AssetAnimGraph2D")

	META_DATA_DESC(AssetAnimGraph2D)
		ASSET_TEXTURE_PROP(AssetAnimGraph2D, texture, "Visual", "Texture")
		FLOAT_PROP(AssetAnimGraph2D, k, 1.0f, "Prp", "kgk", "hk")
		CALLBACK_PROP(AssetAnimGraph2D, AssetAnimGraph2D::StartEditAnimation, "Properties", "Sprite Editor")
	META_DATA_DESC_END()

	void AssetAnimGraph2D::StartEditAnimation(void* owner)
	{
#ifdef OAK_EDITOR
		//ShowSpriteWindow((AssetTexture*)owner);
#endif
	}

	void AssetAnimGraph2D::Reload()
	{
	}

	void AssetAnimGraph2D::LoadData(JsonReader& loader)
	{
		/*if (loader.EnterBlock("spriteSheet"))
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
		}*/
	}

	#ifdef OAK_EDITOR
	void AssetAnimGraph2D::SaveData(JsonWriter& saver)
	{
		/*saver.StartBlock("spriteSheet");

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

		saver.FinishBlock();*/
	}

	const char* AssetAnimGraph2D::GetSceneEntityType()
	{
		return "AnimGraph2D";
	}
	#endif

	void AssetAnimGraph2DRef::Draw(Transform* trans, Color clr, float dt)
	{
		if (!Get()->texture)
		{
			return;
		}

		auto size = Get()->texture.GetSize();
		trans->size = Math::Vector3(size.x, size.y, 0.0f);

		Get()->texture.Draw(trans, clr, dt);
	}

	void AssetAnimGraph2DRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		AnimGraph2D* sprite = reinterpret_cast<AnimGraph2D*>(entity);

		if (sprite)
		{
			sprite->anim = *this;

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}

	Math::Vector2 AssetAnimGraph2DRef::GetSize()
	{
		/*if (sliceIndex == -1 || sliceIndex >= Get()->slices.size())
		{
			return Get()->size;
		}
		
		AssetTexture::Slice& slice = Get()->slices[sliceIndex];
		return slice.size;*/
		return 0.0f;
	}
};