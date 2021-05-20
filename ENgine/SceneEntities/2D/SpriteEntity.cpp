
#include "SpriteEntity.h"
#include "Root/Root.h"

#include "Support/MetaData.h"


namespace Oak
{
	CLASSREG(SceneEntity, SpriteEntity, "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		FLOAT_PROP(SpriteEntity, transform.position.x, 0.0f, "Transform", "x", "X coordinate of a sprite")
		FLOAT_PROP(SpriteEntity, transform.position.y, 0.0f, "Transform", "y", "Y coordinate of a sprite")
		FLOAT_PROP(SpriteEntity, transform.position.z, 0.0f, "Transform", "depth", "Depth from a camera position")
		FLOAT_PROP(SpriteEntity, transform.rotation.z, 0.0f, "Transform", "rotation", "Rotation of a sprite")
		FLOAT_PROP(SpriteEntity, transform.size.x, 100.0f, "Transform", "width", "Width of a sprite")
		FLOAT_PROP(SpriteEntity, transform.size.y, 100.0f, "Transform", "height", "Height of a sprite")
		FLOAT_PROP(SpriteEntity, transform.offset.x, 0.5f, "Transform", "offset X", "X coordinate of anchorn in absolute units")
		FLOAT_PROP(SpriteEntity, transform.offset.y, 0.5f, "Transform", "offset Y", "Y coordinate of anchorn in absolute units")
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	Sprite::FrameState SpriteEntity::frameState;

	SpriteEntity::SpriteEntity() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void SpriteEntity::Init()
	{
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