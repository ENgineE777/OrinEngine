
#include "Camera2D.h"
#include "Root/Root.h"
#include "Support/Sprite.h"

namespace Oak
{
	CLASSREG(SceneEntity, Camera2D, "Camera2D")

	META_DATA_DESC(Camera2D)
		BASE_SCENE_ENTITY_PROP(Camera2D)
		TRANSFORM3D_PROP(Camera2D, transform, "Transform")
		FLOAT_PROP(Camera2D, zoom, 1.0f, "Property", "zoom", "Zoom of a camera")
	META_DATA_DESC_END()

	Camera2D::Camera2D() : SceneEntity()
	{
	}

	void Camera2D::Init()
	{
		Tasks(false)->AddTask(0, this, (Object::Delegate)&Camera2D::Update);
	}

	void Camera2D::Update(float dt)
	{
		transform.BuildMatrices();

		if (GetScene()->Playing())
		{
			if (GetState() == SceneEntity::State::Active)
			{
				auto pos = transform.global.Pos();

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
			float dist = 7.0f;
			float width = tanf(45.0f * Math::Radian * 0.5f) * dist;

			Math::Vector3 edges[] = { Math::Vector3(0.0f), Math::Vector3(-width , width, dist), Math::Vector3(width, width, dist), Math::Vector3(width,-width, dist), Math::Vector3(-width,-width, dist)};

			for (int i = 0; i < 5; i++)
			{
				edges[i] = edges[i] * transform.global;
			}

			for (int i = 1; i < 5; i++)
			{
				root.render.DebugLine(edges[0], COLOR_GREEN, edges[i], COLOR_GREEN);
				root.render.DebugLine(edges[i], COLOR_GREEN, edges[i == 4 ? 1 : i + 1], COLOR_GREEN);
			}
		}
	}

	Transform* Camera2D::GetTransform()
	{
		return &transform;
	}
}