
#include "SpriteEntity.h"
#include "AnimGraph2D.h"
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
		BOOL_PROP(SpriteEntity, autoTileH, false, "Visual", "autoTileH", "autoTileH")
		BOOL_PROP(SpriteEntity, autoTileV, false, "Visual", "autoTileV", "autoTileV")
		BOOL_PROP(SpriteEntity, autoCalcTileZone, true, "Visual", "autoCalcTileZone", "autoCalcTileZone")
		VECTOR2_PROP(SpriteEntity, zoneSize, 100.0f, "Visual", "zoneSize")
		BOOL_PROP(SpriteEntity, usePortlas, false, "Visual", "usePortlas", "Draw priority")
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
		
		if (autoCalcTileZone)
		{
			auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());

			zoneCenter = { pos.x, pos.y };
			zoneSize = size;
		}
		else
		{
			zoneCenter = 0.0f;
		}
	}

	void SpriteEntity::Draw(float dt)
	{
		if (IsVisible())
		{
			Transform trans = transform;
			
			if (usePortlas && !portlaMask)
			{
				portlaMask.SetEntity(GetScene()->FindEntity<PortalMask>());
			}

			if (DefferedLight::hackStateEnabled && DefferedLight::gbufferTech)
			{				
				RenderTechniqueRef tech = (usePortlas && portlaMask) ? portlaMask->quadMaskedDefferedPrg : DefferedLight::gbufferTech;

				tech->SetTexture(ShaderType::Pixel, "materialMap", material ? material.Get()->texture : nullptr);
				tech->SetTexture(ShaderType::Pixel, "normalsMap", normal ? normal.Get()->texture : nullptr);

				Math::Matrix mat = trans.GetGlobal();
				mat.Pos() = 0.0f;

				tech->SetMatrix(ShaderType::Pixel, "normalTrans", &mat, 1);

				Math::Vector4 params;
				params.x = (float)lightGroup / DefferedLight::lightGroupDivider;
				params.y = useRimLight ? 1.0f : 0.0f;
				params.z = emmisiveIntencity;

				tech->SetVector(ShaderType::Pixel, "params", &params, 1);

				tech->SetVector(ShaderType::Pixel, "emmisive", (Math::Vector4*)&emmisive.r, 1);

				texture.prg = tech;
			}
			else
			{
				if (lighten)
				{
					texture.prg = (usePortlas && portlaMask) ? portlaMask->quadMaskedLightenPrg : Sprite::quadLightenPrg;
				}
				else
				{
					texture.prg = noZ ? Sprite::quadPrgNoZ : Sprite::quadPrg;
				}
			}			

			auto camPos = Sprite::GetCamPos();

			if (autoTileH || autoTileV)	
			{
				auto pos = trans.parent ? Sprite::ToPixels(trans.parent->GetGlobal().Pos()) : 0.0f;

				if (paralaxInEditor || GetScene()->IsPlaying())
				{
					pos.x += (camPos.x - pos.x) * (1.0f - paralax.x);
					pos.y += (camPos.y - pos.y) * (1.0f - paralax.y);
				}

				int x_offset = (int)((camPos.x - pos.x) / zoneSize.x);
				int y_offset = (int)((camPos.y - pos.y) / zoneSize.y);

				int fromX = 0;
				int toX = 0;

				if (autoTileH)
				{
					float scale = root.render.GetDevice()->GetHeight() / Sprite::GetPixelsHeight();
					toX = (int)(((root.render.GetDevice()->GetWidth() / scale) / zoneSize.x + 1) * 0.5f);
					fromX = -toX;
				}

				int fromY = 0;
				int toY = 0;

				if (autoTileV)
				{
					toY = (int)((Sprite::GetPixelsHeight() / zoneSize.y + 1) * 0.5f);
					fromY = -toY;
				}
				
				Transform localTrans = transform;
				localTrans.parent = nullptr;

				pos += transform.position;

				for (int y = fromY - 1; y <= toY + 1; y++)
				{
					for (int x = fromX - 1; x <= toX + 1; x++)
					{
						localTrans.position = Math::Vector3(pos.x + (x_offset + x) * zoneSize.x - zoneCenter.x,
															pos.y + (y_offset + y) * zoneSize.y - zoneCenter.y, pos.z);

						texture.Draw(&localTrans, color, dt);
					}
				}
			}
			else
			{
				auto pos = Sprite::ToPixels(trans.GetGlobal().Pos());

				if (paralaxInEditor || GetScene()->IsPlaying())
				{
					auto localPos = trans.position;

					localPos.x += (camPos.x - pos.x) * (1.0f - paralax.x);
					localPos.y += (camPos.y - pos.y) * (1.0f - paralax.y);

					trans.position = localPos;
				}

				texture.Draw(&trans, color, AnimGraph2D::pause ? 0.0f : dt);
			}
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