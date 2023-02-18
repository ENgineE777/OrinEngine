
#include "PointLight2D.h"
#include "Root/Root.h"
#include "Editor/EditorDrawer.h"

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
		FLOAT_PROP(PointLight2D, falloff, -1.0f, "Visual", "falloff", "falloff")
		FLOAT_PROP(PointLight2D, intesity, 1.0f, "Visual", "intesity", "intesity")
		FLOAT_PROP(PointLight2D, lineWidth, 0.0f, "Visual", "Line Width", "Line Width")
		FLOAT_PROP(PointLight2D, viewAngle, 181.0f, "Visual", "View Angle", "View Angle")
		BOOL_PROP(PointLight2D, castShadow, false, "Visual", "Cast Shadow", "View Angle")
	META_DATA_DESC_END()

	PointLight2D::PointLight2D() : SceneEntity()
	{
		transform.size.x = 100.0f;
	}

	void PointLight2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectSizeX | TransformFlag::RotateZ;

		spriteLight = root.render.GetRenderTechnique<PointLight2DProgram>(_FL_);

		Tasks(true)->AddTask(199, this, (Object::Delegate) & PointLight2D::Draw);

		GetScene()->AddToGroup(this, "PointLight2D");
	}

	void PointLight2D::Draw(float dt)
	{
		if (GetScene()->IsPlaying())
		{
			return;
		}

		transform.size.y = transform.size.x;

		if (IsVisible())
		{
			auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());
			float scale = 640.0f / Sprite::GetPixelsHeight();

			auto size = Math::Vector2((float)editorDrawer.lightBulbTex->GetWidth(), (float)editorDrawer.lightBulbTex->GetHeight()) / scale;

			editorDrawer.DrawSprite(editorDrawer.lightBulbTex, Math::Vector2(pos.x, pos.y) - Math::Vector2(size.x, -size.y) * 0.5f, size, 0.0f, 0.0f, COLOR_WHITE);
		}
	}
}