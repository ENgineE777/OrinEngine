
#include "SpriteEntity.h"
#include "DefferedLight.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, SpriteEntity, "2D/Sprites", "Sprite")

	META_DATA_DESC(SpriteEntity)
		BASE_SCENE_ENTITY_PROP(SpriteEntity)
		ASSET_TEXTURE_PROP(SpriteEntity, texture, "Visual", "Texture")
		ASSET_TEXTURE_PROP(SpriteEntity, normal, "Visual", "Normal")
		ASSET_TEXTURE_PROP(SpriteEntity, material, "Visual", "Material")
		INT_PROP(SpriteEntity, drawLevel, 0, "Visual", "draw_level", "Draw priority")
		BOOL_PROP(SpriteEntity, noZ, false, "Visual", "noZ", "no use Z during render")
		BOOL_PROP(SpriteEntity, lighten, false, "Visual", "lighten", "lighten")
		COLOR_PROP(SpriteEntity, color, COLOR_WHITE, "Visual", "Color")
		COLOR_PROP(SpriteEntity, emmisive, COLOR_WHITE, "Visual", "Emmisive")
		FLOAT_PROP(SpriteEntity, emmisiveIntencity, 1.0f, "Visual", "emmisiveIntencity", "emmisiveIntencity")
		INT_PROP(SpriteEntity, lightGroup, 1, "Visual", "lightGroup", "lightGroup")
		BOOL_PROP(SpriteEntity, useRimLight, false, "Visual", "useRimLight", "useRimLight")
		BOOL_PROP(SpriteEntity, paralaxInEditor, false, "Visual", "paralaxInEditor", "paralaxInEditor")
		FLOAT_PROP(SpriteEntity, paralax.x, 1.0f, "Visual", "paralax X", "X-axis paralax")
		FLOAT_PROP(SpriteEntity, paralax.y, 1.0f, "Visual", "paralax Y", "Y-axis paralax")
	META_DATA_DESC_END()

	SpriteEntity::SpriteEntity() : SceneEntity()
	{
	}

	void SpriteEntity::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = (TransformFlag)(TransformFlag::SpriteTransformFlags ^ TransformFlag::RectSizeXY);
	}

	void SpriteEntity::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);

#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&SpriteEntity::Draw);
		Tasks(true)->AddTask(501, this, (Object::Delegate)&SpriteEntity::DrawOccluder);
	}

	void SpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			Transform trans = transform;

			{
				Math::Matrix view;
				root.render.GetTransform(TransformStage::View, view);
				view.Inverse();

				auto camPos = Sprite::ToPixels(view.Pos());

				auto pos = trans.position;

				if (paralaxInEditor || GetScene()->IsPlaying())
				{
					pos.x = pos.x + (camPos.x - pos.x) * (1.0f - paralax.x);
					pos.y = pos.y + (camPos.y - pos.y) * (1.0f - paralax.y);

					trans.position = pos;
				}
			}

			if (DefferedLight::hackStateEnabled && DefferedLight::gbufferTech)
			{
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "materialMap", material ? material.Get()->texture : nullptr);
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "normalsMap", normal ? normal.Get()->texture : nullptr);

				texture.prg = DefferedLight::gbufferTech;

				Math::Matrix mat = trans.GetGlobal();
				mat.Pos() = 0.0f;

				DefferedLight::gbufferTech->SetMatrix(ShaderType::Pixel, "normalTrans", &mat, 1);

				Math::Vector4 params;
				params.x = (float)lightGroup / DefferedLight::lightGroupDivider;
				params.y = useRimLight ? 1.0f : 0.0f;
				params.z = emmisiveIntencity;

				DefferedLight::gbufferTech->SetVector(ShaderType::Pixel, "params", &params, 1);

				DefferedLight::gbufferTech->SetVector(ShaderType::Pixel, "emmisive", (Math::Vector4*)&emmisive.r, 1);
			}
			else
			{
				if (lighten)
				{
					texture.prg = Sprite::quadLightenPrg;
				}
				else
				{
					texture.prg = noZ ? Sprite::quadPrgNoZ : Sprite::quadPrg;
				}
			}

			texture.Draw(&trans, color, dt);
		}
	}

	void SpriteEntity::DrawOccluder(float dt)
	{
		if (IsVisible() && drawLevel < 8)
		{
			texture.prg = Sprite::quadPrgNoZ;
			texture.Draw(&transform, COLOR_BLACK, 0.0f);
		}
	}
}