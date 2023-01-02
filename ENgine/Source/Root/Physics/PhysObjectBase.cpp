
#include "Root/Root.h"

namespace Orin
{
	void PhysObjectBase::Release()
	{
		root.physics.needed_to_delete.push_back(this);
	}
}