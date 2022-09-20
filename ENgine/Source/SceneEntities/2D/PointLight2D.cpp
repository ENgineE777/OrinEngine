
#include "PointLight2D.h"
#include "Root/Root.h"

namespace Oak
{
	class PointLight2DProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_light_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			//root.render.GetDevice()->SetBlendFunc(BlendArg::ArgOne, BlendArg::ArgOne);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	CLASSREGEX(Program, PointLight2DProgram, PointLight2DProgram, "PointLight2DProgram")
	CLASSREGEX_END(Program, PointLight2DProgram)

	CLASSREG(SceneEntity, PointLight2D, "PointLight2D")

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

		spriteLight = root.render.GetProgram("PointLight2DProgram", _FL_);

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