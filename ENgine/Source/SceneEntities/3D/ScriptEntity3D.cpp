#include "ScriptEntity3D.h"
#include "Root/Root.h"

namespace Orin
{
	void ScriptEntity3D::Init()
	{
		transform.transformFlag = MoveXYZ;
	}

	void ScriptEntity3D::OnVisiblityChange(bool set)
	{
		if (!GetScene()->IsPlaying())
		{
			return;
		}

		if (set  && !taskRegisterd)
		{
			Tasks(false)->AddTask(-50, this, (Object::Delegate) & ScriptEntity3D::Update);
			taskRegisterd = true;
		}
		else
		if (!set && taskRegisterd)
		{
			Tasks(false)->DelTask(-50, this);
			taskRegisterd = false;
		}
	}

	void ScriptEntity3D::Play()
	{
		SceneEntity::Play();

		if (IsVisible() && !taskRegisterd)
		{
			Tasks(false)->AddTask(-50, this, (Object::Delegate) & ScriptEntity3D::Update);
			taskRegisterd = true;
		}
	}

	void ScriptEntity3D::Update(float dt)
	{
	}
}