
#include "SpriteEntity.h"
#include "Root/Root.h"

namespace Oak
{
	ENTITYREG(SceneEntity, SpriteEntity, "2D/Sprites", "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
		INT_PROP(SpriteEntity, drawLevel, 0, "Visual", "draw_level", "Draw priority")
		COLOR_PROP(SpriteEntity, color, COLOR_WHITE, "Visual", "Color")
	META_DATA_DESC_END()

	SpriteEntity::SpriteEntity() : SceneEntity()
	{
	}

	void SpriteEntity::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = SpriteTransformFlags;
	}

	void SpriteEntity::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);

#ifdef OAK_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&SpriteEntity::Draw);
	}

	void SpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			texture.Draw(&transform, color, dt);
		}
	}
}