#include "Root/Root.h"
#include "SceneEntities/2D/SpritesLayer.h"

#ifdef ORIN_EDITOR
#include "Editor/EditorDrawer.h"
#include "imgui.h"
#endif

namespace Orin
{
	CLASSREG(Asset, AssetSpritesLayer, "AssetSpritesLayer")

	META_DATA_DESC(AssetSpritesLayer::LayerSprite)
		FLOAT_PROP(AssetSpritesLayer::LayerSprite, pos.x, 0.0f, "Prop", "x", "X coordinate of a camera position")
		FLOAT_PROP(AssetSpritesLayer::LayerSprite, pos.y, 0.0f, "Prop", "y", "Y coordinate of a camera position")
		FLOAT_PROP(AssetSpritesLayer::LayerSprite, scale, 1.0f, "Prop", "scale", "Scale of a sprite")
		ASSET_TEXTURE_PROP(AssetSpritesLayer::LayerSprite, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	META_DATA_DESC(AssetSpritesLayer)
		ARRAY_PROP(AssetSpritesLayer, sprites, LayerSprite, "Prop", "sprites")
	META_DATA_DESC_END()

	void AssetSpritesLayer::Init()
	{
#ifdef ORIN_EDITOR
		RenderTasks()->AddTask(0, this, (Object::Delegate)&AssetSpritesLayer::Draw);
#endif
	}

	void AssetSpritesLayer::CalcInnerValues()
	{
		size = 100.0f;
		center = 0.0f;

		if (sprites.size() > 0)
		{
			Math::Vector2 minPos = sprites[0].pos - sprites[0].texture.GetSize() * sprites[0].scale * 0.5f;
			Math::Vector2 maxPos = sprites[0].pos + sprites[0].texture.GetSize() * sprites[0].scale * 0.5f;

			for (auto& sprite : sprites)
			{
				Math::Vector2 tmpMinPos = sprite.pos - sprite.texture.GetSize() * sprite.scale * 0.5f;
				Math::Vector2 tmpMaxPos = sprite.pos + sprite.texture.GetSize() * sprite.scale * 0.5f;

				minPos.x = fmin(minPos.x, tmpMinPos.x);
				minPos.y = fmin(minPos.y, tmpMinPos.y);

				maxPos.x = fmax(maxPos.x, tmpMaxPos.x);
				maxPos.y = fmax(maxPos.y, tmpMaxPos.y);
			}

			size = maxPos - minPos;
			center = (maxPos + minPos) * 0.5f;
		}
	}

	void AssetSpritesLayer::Reload()
	{
	}

	void AssetSpritesLayer::Draw(float dt)
	{
		AssetNavigable2D::Draw(dt);

		for (auto& sprite : sprites)
		{
			Transform transform;
			transform.position = sprite.pos;
			transform.size = sprite.texture.GetSize() * sprite.scale;

			sprite.texture.Draw(&transform, COLOR_WHITE, dt);
		}
	}

	void AssetSpritesLayer::LoadMetaData(JsonReader& loader)
	{
		AssetNavigable2D::LoadMetaData(loader);

		CalcInnerValues();
	}

	#ifdef ORIN_EDITOR
	const char* AssetSpritesLayer::GetSceneEntityType()
	{
		return "SpritesLayer";
	}

	void AssetSpritesLayer::ImGuiMetaProperties()
	{

	}

	void AssetSpritesLayer::ImGuiProperties()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->ImGuiWidgets();

		if (GetMetaData()->IsValueWasChanged())
		{
			Reload();
			Save();
			CalcInnerValues();
		}
	}
	#endif

	Math::Vector2 AssetSpritesLayerRef::GetSize()
	{
		auto* asset = Get();

		if (asset)
		{
			return asset->size;
		}

		return 100.0f;
	}

	void AssetSpritesLayerRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		SpritesLayer* layer = dynamic_cast<SpritesLayer*>(entity);

		if (layer)
		{
			layer->spritesAsset = *this;

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}

	void AssetSpritesLayerRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = root.assets.GetAssetRef<AssetSpritesLayerRef>(path);
			}

			loader.LeaveBlock();
		}
	}

	void AssetSpritesLayerRef::Draw(Math::Vector3 pos, Color clr, float dt)
	{
		auto* asset = Get();

		if (asset)
		{
			Transform trans;
			trans.objectType = ObjectType::Object2D;

			for (auto sprite : asset->sprites)
			{
				auto pos2d = asset->center - sprite.pos;

				trans.position = Math::Vector3(pos.x + pos2d.x, pos.y + pos2d.y, pos.z);

				trans.size = sprite.texture.GetSize();
				trans.scale = sprite.scale;

				sprite.texture.Draw(&trans, COLOR_WHITE, dt);
			}
		}
	}

	#ifdef ORIN_EDITOR
	void AssetSpritesLayerRef::SaveData(JsonWriter& saver, const char* name)
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