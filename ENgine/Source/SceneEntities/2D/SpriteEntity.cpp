
#include "SpriteEntity.h"
#include "DefferedLight.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, SpriteEntity, "2D/Sprites", "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
		ASSET_TEXTURE_PROP(SpriteEntity, normal, "Visual", "normal")
		ASSET_TEXTURE_PROP(SpriteEntity, emission, "Visual", "emission")
		INT_PROP(SpriteEntity, drawLevel, 0, "Visual", "draw_level", "Draw priority")
		BOOL_PROP(SpriteEntity, noZ, false, "Visual", "noZ", "no use Z during render")
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

#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&SpriteEntity::Draw);
	}

	void SpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			if (DefferedLight::hackStateEnabled && DefferedLight::gbufferTech)
			{
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "materialMap", emission ? emission.Get()->texture : nullptr);
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "normalsMap", normal ? normal.Get()->texture : nullptr);

				texture.prg = DefferedLight::gbufferTech;
			}
			else
			{
				texture.prg = noZ ? Sprite::quadPrgNoZ : Sprite::quadPrg;
			}

			texture.Draw(&transform, color, dt);
		}
	}
}