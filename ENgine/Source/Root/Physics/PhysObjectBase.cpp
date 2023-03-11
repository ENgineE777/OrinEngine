
#include "Root/Root.h"

namespace Orin
{
	void PhysObjectBase::SetActive(bool set)
	{
		is_active = set;
	}

	bool PhysObjectBase::IsActive()
	{
		return is_active;
	}

	void PhysObjectBase::Release()
	{
		root.physics.needed_to_delete.push_back(this);
	}
}