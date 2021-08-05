#include "ScriptEntity.h"
#include "Root/Root.h"

namespace Oak
{
	void ScriptEntity::Init()
	{
	}

	void ScriptEntity::SetVisible(bool visible)
	{
		SceneEntity::SetVisible(visible);

		if (visible)
		{
			Tasks(false)->AddTask(100, this, (Object::Delegate) & ScriptEntity::Update);
		}
		else
		{
			Tasks(false)->DelTask(100, this);
		}
	}

	void ScriptEntity::Play()
	{
		SceneEntity::Play();

		if (IsVisible())
		{
			Tasks(false)->AddTask(100, this, (Object::Delegate) & ScriptEntity::Update);
		}
	}

	void ScriptEntity::Update(float dt)
	{
	}
}