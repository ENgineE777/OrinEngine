
#include "Joint2D.h"
#include "Root/Root.h"

#include "PxPhysicsAPI.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Joint2D, "2D/Physics", "Joint2D")

	META_DATA_DESC(Joint2D)
		BASE_SCENE_ENTITY_PROP(Joint2D)
		FLOAT_PROP(Joint2D, coneLimit, -1.0f, "Properties", "cone limit", "coneLimit")
		SCENEOBJECT_PROP(Joint2D, targetRef, "Properties", "Target")
	META_DATA_DESC_END()

	void Joint2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&Joint2D::EditorDraw);
	}

	void Joint2D::Play()
	{
		GetScene()->AddPostPlay(0, this, (Object::DelegateSimple)&Joint2D::CreateJoint);
	}

	void Joint2D::CreateJoint()
	{
		if (parent && targetRef)
		{
			auto* parentEntity = dynamic_cast<PhysEntity2D*>(parent);

			if (parentEntity)
			{
				auto* bodyA = parentEntity->body.body;
				auto* bodyB = targetRef->body.body;

				if (bodyA && bodyB)
				{
					auto mat = transform.GetGlobal();

					auto matA = parentEntity->GetTransform().GetGlobal();
					
					Math::Matrix inverse = matA;
					inverse.Inverse();

					matA = mat * inverse;

					auto matB = targetRef->GetTransform().GetGlobal();

					inverse = matB;
					inverse.Inverse();

					matB = mat * inverse;

					joint = root.GetPhysScene()->CreateSphericalJoint(bodyA, matA, bodyB, matB);

					if (coneLimit > 0.01f)
					{
						joint->SetConeLimit(coneLimit * Math::Radian, coneLimit * Math::Radian);
					}
				}
			}			
		}
	}

	void Joint2D::EditorDraw(float dt)
	{
		if (!GetScene()->IsPlaying() && !prefabInstance)
		{
			auto mat = GetTransform().GetGlobal();
			auto center = Sprite::ToPixels(mat.Pos());

			float angle = atan2f(mat.Vx().y, mat.Vx().x);

			Math::Vector3 dir(cosf(angle + coneLimit * Math::Radian), sinf(angle + coneLimit * Math::Radian), 0.0f);

			auto p1 = center + dir * 15.0f;

			dir = Math::Vector3(cosf(angle - coneLimit * Math::Radian), sinf(angle - coneLimit * Math::Radian), 0.0f);
			auto p2 = center + dir * 15.0f;

			Sprite::DebugLine(center, p1, COLOR_CYAN);
			Sprite::DebugLine(center, p2, COLOR_CYAN);
			Sprite::DebugLine(p1, p2, COLOR_CYAN);
		}
	}

	void Joint2D::Release()
	{
		SceneEntity::Release();

		RELEASE(joint)
	}

}