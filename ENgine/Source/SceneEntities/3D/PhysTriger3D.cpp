
#include "PhysTriger3D.h"
#include "Root/Root.h"

namespace Orin
{

	ENTITYREG(SceneEntity, PhysTriger3D, "3D/Physics", "PhysTriger3D")

	META_DATA_DESC(PhysTriger3D)
		BASE_SCENE_ENTITY_PROP(PhysTriger3D)
		COLOR_PROP(PhysTriger3D, color, COLOR_YELLOW, "Geometry", "color")
		INT_PROP(PhysTriger3D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(PhysTriger3D, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
	META_DATA_DESC_END()

	void PhysTriger3D::Init()
	{
		transform.transformFlag = MoveXYZ | RotateXYZ | SizeXYZ;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysTriger3D::Draw);

		auto onContactStart = [this](int index, SceneEntity* entity, int contactIndex)
		{
			this->OnContactStart(index, entity, contactIndex);
		};

		RegisterCallback<eastl::function<void(int, SceneEntity*, int)>>("OnContactStart", onContactStart);

		auto onConstactEnd = [this](int index, SceneEntity* entity, int contactIndex)
		{
			this->OnContactEnd(index, entity, contactIndex);
		};

		RegisterCallback<eastl::function<void(int, SceneEntity*, int)>>("OnContactEnd", onConstactEnd);
	}

	void PhysTriger3D::ApplyProperties()
	{
		bodyType = BodyType::Trigger;
		color.a = 0.25f;
	}

	void PhysTriger3D::OnContactStart(int index, SceneEntity* entity, int contactIndex)
	{

	}

	void PhysTriger3D::OnContactEnd(int index, SceneEntity* entity, int  contactIndex)
	{

	}
}