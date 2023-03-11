
#pragma once

#ifdef PLATFORM_IOS

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#endif

#include "PxPhysicsAPI.h"

#ifdef PLATFORM_IOS

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG

#endif

#endif

#include "Support/Support.h"

namespace Orin
{
	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief PhysJoint

	Joint

	*/

	using namespace physx;

	class CLASS_DECLSPEC PhysSphericalJoint : public PhysObjectBase
	{
		friend class Physics;
		friend class PhysScene;

	public:

		void SetActive(bool set) override;
		void SetConeLimit(float angY, float angZ);

	protected:
		PxSphericalJoint* joint = nullptr;
		PxRigidActor* actorA = nullptr;
		PxRigidActor* actorB = nullptr;

		void ActualRelease() override;
	};
}