#include "ScriptEntity3D.h"
#include "Root/Root.h"

namespace Oak
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

		if (set)
		{
			Tasks(false)->AddTask(-50, this, (Object::Delegate) & ScriptEntity3D::Update);
		}
		else
		{
			Tasks(false)->DelTask(-50, this);
		}
	}

	void ScriptEntity3D::Play()
	{
		SceneEntity::Play();

		if (IsVisible())
		{
			Tasks(false)->AddTask(-50, this, (Object::Delegate) & ScriptEntity3D::Update);
		}
	}

	void ScriptEntity3D::Update(float dt)
	{
	}
}