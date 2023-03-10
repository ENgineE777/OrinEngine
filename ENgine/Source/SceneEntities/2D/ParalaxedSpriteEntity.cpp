
#include "ParalaxedSpriteEntity.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, ParalaxedSpriteEntity, "2D/Sprites", "ParalaxedSprite")

	META_DATA_DESC(ParalaxedSpriteEntity)
		BASE_SCENE_ENTITY_PROP(ParalaxedSpriteEntity)
		BOOL_PROP(ParalaxedSpriteEntity, paralaxInEditor, false, "Visual", "paralaxInEditor", "paralaxInEditor")
		FLOAT_PROP(ParalaxedSpriteEntity, paralax.x, 1.0f, "Visual", "paralax X", "X-axis paralax")
		FLOAT_PROP(ParalaxedSpriteEntity, paralax.y, 1.0f, "Visual", "paralax Y", "Y-axis paralax")
		ASSET_TEXTURE_PROP(ParalaxedSpriteEntity, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	ParalaxedSpriteEntity::ParalaxedSpriteEntity() : SceneEntity()
	{
	}

	void ParalaxedSpriteEntity::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = SpriteTransformFlags;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&ParalaxedSpriteEntity::Draw);
	}

	void ParalaxedSpriteEntity::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);
	}

	void ParalaxedSpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			Math::Matrix view;
			root.render.GetTransform(TransformStage::View, view);
			view.Inverse();

			auto camPos = Sprite::ToPixels(view.Pos());

			Transform trans = transform;

			auto pos = trans.position;

			if (paralaxInEditor || GetScene()->IsPlaying())
			{
				pos.x = pos.x + (camPos.x - pos.x) * (1.0f - paralax.x);
				pos.y = pos.y + (camPos.y - pos.y) * (1.0f - paralax.y);

				trans.position = pos;
			}

			texture.Draw(&trans, COLOR_WHITE, dt);
		}
	}
}