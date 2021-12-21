
#include "Camera2D.h"
#include "Root/Root.h"
#include "Support/Sprite.h"

namespace Oak
{
	CLASSREG(SceneEntity, Camera2D, "Camera2D")

	META_DATA_DESC(Camera2D)
		BASE_SCENE_ENTITY_PROP(Camera2D)
		FLOAT_PROP(Camera2D, zoom, 1.0f, "Properties", "zoom", "Zoom of a camera")
		SCENEOBJECT_PROP(Camera2D, targetRef, "Properties", "Target")
		FLOAT_PROP(Camera2D, border.x, 100.0f, "Properties", "Horizontal border", "Horizontal border of a camera")
		FLOAT_PROP(Camera2D, border.y, 100.0f, "Properties", "Vertical border", "Vertical border of a camera")
		BOOL_PROP(Camera2D, useLimits, false, "Limits", "Use Limits", "Use limits for a camera")
		FLOAT_PROP(Camera2D, leftup.x, -1000.0f, "Limits", "Left limit", "Left limit of a camera")
		FLOAT_PROP(Camera2D, rightdown.x, 1000.0f, "Limits", "Right limit", "Right limit of a camera")
		FLOAT_PROP(Camera2D, leftup.y, 1000.0f, "Limits", "Up limit", "Up limit of a camera")
		FLOAT_PROP(Camera2D, rightdown.y, -1000.0f, "Limits", "Down limit", "Down limit of a camera")
	META_DATA_DESC_END()

	Camera2D::Camera2D() : SceneEntity()
	{
	}

	void Camera2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.size.x = Sprite::pixelsHeight * 16.0f / 9.0f;
		transform.size.y = Sprite::pixelsHeight;
		transform.transformFlag = TransformFlag::MoveX | TransformFlag::MoveY | RectMoveXY;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&Camera2D::Update);
	}

	void Camera2D::UpdateRenderView()
	{
		if (IsVisible())
		{
			auto pos = transform.global.Pos() * Sprite::pixelsPerUnitInvert;

			float dist = (Sprite::pixelsHeight * 0.5f * Sprite::pixelsPerUnitInvert) / (tanf(22.5f * Math::Radian) * zoom);
			view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), Math::Vector3(0, 1, 0));

			root.render.SetTransform(TransformStage::View, view);

			Math::Matrix proj;
			proj.BuildProjection(45.0f * Math::Radian, (float)root.render.GetDevice()->GetHeight() / (float)root.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
			root.render.SetTransform(TransformStage::Projection, proj);
		}
	}

	void Camera2D::Update(float dt)
	{
		if (GetScene()->IsPlaying())
		{
			if (targetRef.entity)
			{
				transform.size.x = Sprite::pixelsHeight / root.render.GetDevice()->GetAspect();
				transform.size.y = Sprite::pixelsHeight;

				Math::Vector3 pos = transform.position;

				if (-transform.size.x * 0.5f + border.x > targetRef.entity->GetTransform().global.Pos().x - pos.x)
				{
					pos.x = targetRef.entity->GetTransform().global.Pos().x + transform.size.x * 0.5f - border.x;
				}

				if ( transform.size.x * 0.5f - border.x < targetRef.entity->GetTransform().global.Pos().x - pos.x)
				{
					pos.x = targetRef.entity->GetTransform().global.Pos().x - transform.size.x * 0.5f + border.x;
				}

				if (-transform.size.y * 0.5f + border.y > targetRef.entity->GetTransform().global.Pos().y - pos.y)
				{
					pos.y = targetRef.entity->GetTransform().global.Pos().y + transform.size.y * 0.5f - border.y;
				}

				if (transform.size.y * 0.5f - border.y < targetRef.entity->GetTransform().global.Pos().y - pos.y)
				{
					pos.y = targetRef.entity->GetTransform().global.Pos().y - transform.size.y * 0.5f + border.y;
				}

				if (useLimits)
				{
					pos.x = Math::Clamp(pos.x, leftup.x + transform.size.x * 0.5f, rightdown.x - transform.size.x * 0.5f);
					pos.y = Math::Clamp(pos.y, rightdown.y + transform.size.y * 0.5f, leftup.y - transform.size.y * 0.5f);
				}

				transform.position = pos;
			}

			UpdateRenderView();
		}
		else
		{
			Math::Vector3 p1, p2;

			for (int i = 0; i < 8; i++)
			{
				Math::Vector2 offset = (i >= 4 ? border * 2.0f : 0.0f);

				if (i == 0 || i == 4)
				{
					p1 = Math::Vector3(0, 0, 0);
					p2 = Math::Vector3(transform.size.x - offset.x, 0, 0);
				}
				else
				if (i == 1 || i == 5)
				{
					p1 = Math::Vector3(transform.size.x - offset.x, 0, 0);
					p2 = Math::Vector3(transform.size.x - offset.x, transform.size.y - offset.y, 0);
				}
				else
				if (i == 2 || i == 6)
				{
					p1 = Math::Vector3(transform.size.x - offset.x, transform.size.y - offset.y, 0);
					p2 = Math::Vector3(0, transform.size.y - offset.y, 0);
				}
				else
				if (i == 3 || i == 7)
				{
					p1 = Math::Vector3(0, transform.size.y - offset.y, 0);
					p2 = Math::Vector3(0, 0, 0);
				}

				p1 -= Math::Vector3(transform.offset.x * (transform.size.x - offset.x), transform.offset.y * (transform.size.y - offset.y), 0);
				p1 = p1 * transform.global;
				p1 *= *transform.unitsInvScale;
				p2 -= Math::Vector3(transform.offset.x * (transform.size.x - offset.x), transform.offset.y * (transform.size.y - offset.y), 0);
				p2 = p2 * transform.global;
				p2 *= *transform.unitsInvScale;

				Math::Vector2 tmp = Math::Vector2(p1.x, p1.y);
				p1 = Math::Vector3(tmp.x, tmp.y, p1.z);

				tmp = Math::Vector2(p2.x, p2.y);
				p2 = Math::Vector3(tmp.x, tmp.y, p1.z);

				root.render.DebugLine(p1, COLOR_YELLOW, p2, COLOR_YELLOW, false);
			}
		}
	}
}