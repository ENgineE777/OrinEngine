
#include "PhysEntityBase.h"
#include "Root/Root.h"

namespace Orin
{
	void PhysEntityBase::ApplyProperties()
	{
		color.a = 0.25f;
	}	

	void PhysEntityBase::OnVisiblityChange(bool state)
	{
		if (body.body)
		{
			body.body->SetActive(state);
		}
	}	

	void PhysEntityBase::Release()
	{
		RELEASE(body.body);
		SceneEntity::Release();
	}
}