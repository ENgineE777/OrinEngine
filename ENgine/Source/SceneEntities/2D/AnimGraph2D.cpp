
#include "AnimGraph2D.h"
#include "Root/Root.h"
#include "DefferedLight.h"

namespace Orin
{
	ENTITYREG(SceneEntity, AnimGraph2D, "2D/Sprites","AnimGraph2D")

	META_DATA_DESC(AnimGraph2D)
		BASE_SCENE_ENTITY_PROP(AnimGraph2D)
		ASSET_ANIM_GRAPH_2D_PROP(AnimGraph2D, anim, "Visual", "Anim")
		INT_PROP(AnimGraph2D, drawLevel, 0, "Visual", "draw_level", "Draw priority")
		BOOL_PROP(AnimGraph2D, noZ, false, "Visual", "noZ", "no use Z during render")
		COLOR_PROP(AnimGraph2D, color, COLOR_WHITE, "Visual", "Color")
		COLOR_PROP(AnimGraph2D, emmisive, COLOR_WHITE, "Visual", "Emmisive")
		FLOAT_PROP(AnimGraph2D, emmisiveIntencity, 1.0f, "Visual", "emmisiveIntencity", "emmisiveIntencity")
		INT_PROP(AnimGraph2D, lightGroup, 1, "Visual", "lightGroup", "lightGroup")
		BOOL_PROP(AnimGraph2D, useRimLight, false, "Visual", "useRimLight", "useRimLight")
	META_DATA_DESC_END()

	bool AnimGraph2D::pause = false;

	void AnimGraph2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = (TransformFlag)(TransformFlag::SpriteTransformFlags ^ TransformFlag::RectSizeXY);

		Tasks(true)->AddTask(0, this, (Object::Delegate)&AnimGraph2D::Draw);
	}

	void AnimGraph2D::ApplyProperties()
	{
#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&AnimGraph2D::Draw);
		Tasks(true)->AddTask(501, this, (Object::Delegate)&AnimGraph2D::DrawOccluder);
		Tasks(true)->AddTask(550, this, (Object::Delegate)&AnimGraph2D::DrawMask);
	}

	void AnimGraph2D::Draw(float dt)
	{
		if (IsVisible() && anim.Get())
		{
			if (DefferedLight::hackStateEnabled && DefferedLight::gbufferTech)
			{
				anim.prg = DefferedLight::gbufferTech;

				Math::Matrix mat = transform.GetGlobal();
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
				anim.prg = noZ ? Sprite::quadPrgNoZ : Sprite::quadPrg;
			}

			anim.Draw(&transform, color, pause ? 0.0f : dt);
		}
	}

	void AnimGraph2D::DrawOccluder(float dt)
	{
		if (IsVisible() && anim.Get() && drawLevel < 8)
		{
			anim.prg = Sprite::quadPrgNoZ;
			anim.Draw(&transform, COLOR_BLACK, 0.0f);
		}
	}

	void AnimGraph2D::DrawMask(float dt)
	{
		if (IsVisible() && anim.Get() && drawLevel < 8)
		{
			anim.prg = Sprite::quadPrg;
			anim.Draw(&transform, COLOR_BLACK, 0.0f);
		}
	}
}