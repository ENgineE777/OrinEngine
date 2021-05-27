
#include "SpriteEntity.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, SpriteEntity, "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		TRANSFORM_PROP(SpriteEntity, transform, "Transform")
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	Sprite::FrameState SpriteEntity::frameState;

	SpriteEntity::SpriteEntity() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void SpriteEntity::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;

		sprite.frames.push_back(Sprite::Frame());

		RenderTasks(false)->AddTask(0, this, (Object::Delegate)& SpriteEntity::Draw);

		GetScene()->AddToGroup(this, "SpriteAsset");
	}

	void SpriteEntity::ApplyProperties()
	{
		sprite.texture = texture ? texture->GetTexture() : root.render.GetWhiteTexture();
		transform.size = Math::Vector3((float)sprite.texture->GetWidth(), (float)sprite.texture->GetHeight(), 0.0f);
	}

	Transform* SpriteEntity::GetTransform()
	{
		return &transform;
	}

	void SpriteEntity::Draw(float dt)
	{
		transform.BuildMatrices();
		Sprite::UpdateFrame(&sprite, &frameState, dt);

		Sprite::Draw(&transform, COLOR_WHITE, &sprite, &frameState, true, false);
	}
}