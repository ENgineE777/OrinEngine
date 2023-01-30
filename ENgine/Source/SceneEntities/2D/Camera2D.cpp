
#include "Camera2D.h"
#include "Root/Root.h"
#include "Support/Sprite.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Camera2D, "2D", "Camera2D")

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
		transform.objectType = ObjectType::Object2D;
		transform.size.x = Sprite::GetPixelsHeight() * 16.0f / 9.0f;
		transform.size.y = Sprite::GetPixelsHeight();
		transform.transformFlag = TransformFlag::MoveX | TransformFlag::MoveY | RectMoveXY;

		Tasks(true)->AddTask(-10, this, (Object::Delegate)&Camera2D::Update);
	}

	void Camera2D::Play()
	{
		SceneEntity::Play();

		addPosition = {};
		addRotation = {};
	}

	void Camera2D::UpdateRenderView()
	{
		if (IsVisible())
		{
			const Math::Vector3 addPos = Sprite::ToUnits(addPosition);
			const Math::Vector3 pos    = transform.GetGlobal().Pos() + Math::Vector3{addPos.x, addPos.y, 0.f};

			const float angle = Math::HalfPI + addRotation * Math::Radian;
			const Math::Vector3 upVector{cosf(angle), sinf(angle), 0.f};

			float dist = Sprite::ToUnits(Sprite::GetPixelsHeight() * 0.5f) / (tanf(22.5f * Math::Radian) * zoom);
			view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), upVector);

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
			if (targetRef)
			{
				auto halfScreenSize = Sprite::GetHalfScreenSize();

				Math::Vector3 pos = transform.position;
				Math::Vector3 targetPos = Sprite::ToPixels(targetRef->GetTransform().GetGlobal().Pos());

				if (-halfScreenSize.x + border.x > targetPos.x - pos.x)
				{
					pos.x = targetPos.x + halfScreenSize.x - border.x;
				}

				if (halfScreenSize.x - border.x < targetPos.x - pos.x)
				{
					pos.x = targetPos.x - halfScreenSize.x + border.x;
				}

				if (-halfScreenSize.y + border.y > targetPos.y - pos.y)
				{
					pos.y = targetPos.y + halfScreenSize.y - border.y;
				}

				if (halfScreenSize.y - border.y < targetPos.y - pos.y)
				{
					pos.y = targetPos.y - halfScreenSize.y + border.y;
				}

				if (useLimits)
				{
					pos.x = Math::Clamp(pos.x, leftup.x + halfScreenSize.x, rightdown.x - halfScreenSize.x);
					pos.y = Math::Clamp(pos.y, rightdown.y + halfScreenSize.y, leftup.y - transform.size.y);
				}

				transform.position = pos;

				Sprite::_camPos = Math::Vector2(pos.x, pos.y);
			}

			Sprite::_zoom = zoom;

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
				p1 += transform.position;
				p1 = Sprite::ToUnits(p1);

				p2 -= Math::Vector3(transform.offset.x * (transform.size.x - offset.x), transform.offset.y * (transform.size.y - offset.y), 0);
				p2 += transform.position;
				p2 = Sprite::ToUnits(p2);

				Math::Vector2 tmp = Math::Vector2(p1.x, p1.y);
				p1 = Math::Vector3(tmp.x, tmp.y, p1.z);

				tmp = Math::Vector2(p2.x, p2.y);
				p2 = Math::Vector3(tmp.x, tmp.y, p1.z);

				root.render.DebugLine(p1, COLOR_YELLOW, p2, COLOR_YELLOW, false);
			}
		}
	}

	void Camera2D::CenterCamera()
	{
		if (targetRef)
		{
			transform.position = Sprite::ToPixels(targetRef->GetTransform().GetGlobal().Pos());
		}
	}
}