
#include "Camera2D.h"
#include "Root/Root.h"
#include "Support/Sprite.h"

namespace Oak
{
	CLASSREG(SceneEntity, Camera2D, "Camera2D")

	META_DATA_DESC(Camera2D)
		BASE_SCENE_ENTITY_PROP(Camera2D)
		TRANSFORM2D_PROP(Camera2D, transform, "Transform")
		FLOAT_PROP(Camera2D, zoom, 1.0f, "Properties", "zoom", "Zoom of a camera")
		SCENEOBJECT_PROP(Camera2D, targetRef, "Properties", "Target")
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

		Tasks(false)->AddTask(0, this, (Object::Delegate)&Camera2D::Update);
	}

	void Camera2D::Update(float dt)
	{
		transform.BuildMatrices();

		if (GetScene()->Playing())
		{
			if (targetRef.entity)
			{
				transform.position.x = targetRef.entity->GetTransform()->global.Pos().x;
				transform.position.y = targetRef.entity->GetTransform()->global.Pos().y;

				transform.BuildMatrices();
			}

			if (GetState() == SceneEntity::State::Active)
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
		else
		{
			Math::Vector3 p1, p2;

			for (int i = 0; i < 4; i++)
			{
				if (i == 0)
				{
					p1 = Math::Vector3(0, 0, 0);
					p2 = Math::Vector3(transform.size.x, 0, 0);
				}
				else
				if (i == 1)
				{
					p1 = Math::Vector3(transform.size.x, 0, 0);
					p2 = Math::Vector3(transform.size.x, transform.size.y, 0);
				}
				else
				if (i == 2)
				{
					p1 = Math::Vector3(transform.size.x, transform.size.y, 0);
					p2 = Math::Vector3(0, transform.size.y, 0);
				}
				else
				if (i == 3)
				{
					p1 = Math::Vector3(0, transform.size.y, 0);
					p2 = Math::Vector3(0, 0, 0);
				}

				p1 -= Math::Vector3(transform.offset.x * transform.size.x, transform.offset.y * transform.size.y, 0);
				p1 = p1 * transform.global;
				p1 *= *transform.unitsInvScale;
				p2 -= Math::Vector3(transform.offset.x * transform.size.x, transform.offset.y * transform.size.y, 0);
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

	Transform* Camera2D::GetTransform()
	{
		return &transform;
	}
}