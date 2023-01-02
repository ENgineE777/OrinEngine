
#include "PointLight2D.h"
#include "Root/Root.h"

namespace Orin
{
	const char* PointLight2DProgram::GetVsName()
	{
		return "sprite_vs.shd";
	};

	const char* PointLight2DProgram::GetPsName()
	{
		return "sprite_light_ps.shd";
	};

	void PointLight2DProgram::ApplyStates()
	{
		root.render.GetDevice()->SetDepthTest(false);
		root.render.GetDevice()->SetDepthWriting(false);
		//root.render.GetDevice()->SetBlendFunc(BlendArg::ArgOne, BlendArg::ArgOne);
		root.render.GetDevice()->SetAlphaBlend(true);
		root.render.GetDevice()->SetCulling(CullMode::CullNone);
	}

	ENTITYREG(SceneEntity, PointLight2D, "2D/Lights", "PointLight2D")

	META_DATA_DESC(PointLight2D)
		BASE_SCENE_ENTITY_PROP(PointLight2D)
		COLOR_PROP(PointLight2D, color, COLOR_WHITE, "Visual", "Color")
	META_DATA_DESC_END()

	PointLight2D::PointLight2D() : SceneEntity()
	{
		transform.size.x = 100.0f;
	}

	void PointLight2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectSizeX;

		spriteLight = root.render.GetRenderTechnique<PointLight2DProgram>(_FL_);

		Tasks(true)->AddTask(555, this, (Object::Delegate) & PointLight2D::Draw);
	}

	void PointLight2D::Draw(float dt)
	{
		if (IsVisible())
		{
			transform.size.y = transform.size.x;

			Sprite::Draw(nullptr, color, transform.GetGlobal(), Math::Vector2(-transform.size.x, transform.size.y) * 0.5f, transform.size.x, 0.0f, 1.0f, spriteLight);
		}
	}
}