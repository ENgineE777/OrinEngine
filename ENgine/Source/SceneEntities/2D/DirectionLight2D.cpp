
#include "DirectionLight2D.h"
#include "Root/Root.h"
#include "Editor/EditorDrawer.h"

namespace Orin
{
	ENTITYREG(SceneEntity, DirectionLight2D, "2D/Lights", "DirectionLight2D")

	META_DATA_DESC(DirectionLight2D)
		BASE_SCENE_ENTITY_PROP(DirectionLight2D)
		COLOR_PROP(DirectionLight2D, color, COLOR_WHITE, "Visual", "Color")
		FLOAT_PROP(DirectionLight2D, intesity, 1.0f, "Visual", "intesity", "intesity")
		INT_PROP(DirectionLight2D, lightGroup, 1, "Visual", "lightGroup", "lightGroup")
	META_DATA_DESC_END()

	DirectionLight2D::DirectionLight2D() : SceneEntity()
	{
		transform.size.x = 100.0f;
	}

	void DirectionLight2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectSizeX | TransformFlag::RotateZ;

		Tasks(true)->AddTask(199, this, (Object::Delegate) &DirectionLight2D::Draw);

		GetScene()->AddToGroup(this, "DirectionLight2D");
	}

	void DirectionLight2D::Draw(float dt)
	{
		if (GetScene()->IsPlaying())
		{
			return;
		}

		transform.size.x = 100.0f;
		transform.size.y = transform.size.x;

		if (IsVisible())
		{
			auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());
			float scale = 640.0f / Sprite::GetPixelsHeight();

			auto size = Math::Vector2((float)editorDrawer.directionalTex->GetWidth(), (float)editorDrawer.directionalTex->GetHeight()) / scale;
			
			editorDrawer.DrawSprite(editorDrawer.directionalTex, Math::Vector2(pos.x, pos.y), size, -Math::Vector2(size.x, -size.y) * 0.5f, Math::Radian * transform.rotation.z, COLOR_WHITE);
		}
	}

	void DirectionLight2D::GetBBox(Math::Vector3& vMin, Math::Vector3& vMax)
	{
		auto mat = transform.GetGlobal();

		float scale = 640.0f / Sprite::GetPixelsHeight();

		auto size = Sprite::ToUnits(Math::Vector2((float)editorDrawer.directionalTex->GetWidth(), (float)editorDrawer.directionalTex->GetHeight()) / scale);

		vMin = transform.GetGlobal().Pos() - size * 0.5f;
		vMax = transform.GetGlobal().Pos() + size * 0.5f;
	}
}