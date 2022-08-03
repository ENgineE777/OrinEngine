#include "Root/Root.h"
#include "SceneEntities/2D/TileMap.h"

namespace Oak
{
	CLASSREG(Asset, AssetSpriteLayer, "AssetSpriteLayer")

	META_DATA_DESC(AssetSpriteLayer::LayerSprite)
		FLOAT_PROP(AssetSpriteLayer::LayerSprite, pos.x, 0.0f, "Prop", "x", "X coordinate of a camera position")
		FLOAT_PROP(AssetSpriteLayer::LayerSprite, pos.y, 0.0f, "Prop", "y", "Y coordinate of a camera position")
		FLOAT_PROP(AssetSpriteLayer::LayerSprite, size.x, 100.0f, "Prop", "width", "Width of a sprite")
		FLOAT_PROP(AssetSpriteLayer::LayerSprite, size.y, 100.0f, "Prop", "height", "Height of a sprite")
		ASSET_TEXTURE_PROP(AssetSpriteLayer::LayerSprite, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	META_DATA_DESC(AssetSpriteLayer)
		ARRAY_PROP(AssetSpriteLayer, sprites, LayerSprite, "Prop", "sprites")
	META_DATA_DESC_END()

	void AssetSpriteLayer::Init()
	{
#ifdef OAK_EDITOR
		RenderTasks()->AddTask(0, this, (Object::Delegate)& AssetAnimGraph2D::Draw);
#endif
	}
	void AssetSpriteLayer::Reload()
	{
	}

	void AssetSpriteLayer::Draw(float dt)
	{
		/*for (auto sprite : sprites)
		{
			Sprite::UpdateFrame(&sprite.sprite, &sprite.state, dt);
			tmp_trans.size = sprite.size;
			tmp_trans.pos = sprite.pos;
			tmp_trans.BuildMatrices();
			Sprite::Draw(&tmp_trans, COLOR_WHITE, &sprite.sprite, &sprite.state, true, false);
		}*/
	}

	#ifdef OAK_EDITOR
	const char* AssetSpriteLayer::GetSceneEntityType()
	{
		return "SpriteLayer";
	}
	#endif

	void AssetSpriteLayerRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		/*TileMap* tileMap = dynamic_cast<TileMap*>(entity);

		if (tileMap)
		{
			tileMap->tileSet = *this;

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}*/
	}

	void AssetSpriteLayerRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = Oak::root.assets.GetAssetRef<AssetSpriteLayerRef>(path);
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetSpriteLayerRef::SaveData(JsonWriter& saver, const char* name)
	{
		if (Get())
		{
			saver.StartBlock(name);
			saver.Write("Path", Get()->GetPath().c_str());
			saver.FinishBlock();
		}
	}
	#endif
};