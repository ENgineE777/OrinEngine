
#include "Root/Root.h"

namespace Orin
{	
	void PhysSphericalJoint::SetActive(bool set)
	{
		PhysObjectBase::SetActive(set);

		if (joint)
		{
			joint->setActors(set ? actorA : nullptr, set ? actorB : nullptr);
		}
	}

	void PhysSphericalJoint::SetConeLimit(float angY, float angZ)
	{
		if (joint)
		{
			joint->setLimitCone(PxJointLimitCone(angY, angZ, 0.01f));
			joint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
	}

	void PhysSphericalJoint::ActualRelease()
	{
		if (joint)
		{
			joint->release();
		}

		delete this;
	}
}