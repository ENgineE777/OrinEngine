
#include "SpriteEntity.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, SpriteEntity, "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
		COLOR_PROP(SpriteEntity, color, COLOR_WHITE, "Visual", "Color")
	META_DATA_DESC_END()

	SpriteEntity::SpriteEntity() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void SpriteEntity::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&SpriteEntity::Draw);
	}

	void SpriteEntity::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);
	}

	void SpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			transform.BuildMatrices();

			texture.Draw(&transform, color, dt);
		}
	}
}