
#pragma once

#ifdef PLATFORM_IOS

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#endif

/**
\ingroup gr_code_services_physic
*/

#include "PxPhysicsAPI.h"

#ifdef PLATFORM_IOS

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG

#endif

#endif

#include "PhysObjectBase.h"
#include "Support/Support.h"

using namespace physx;

namespace Orin
{
	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief PhysControllerDesc

	Description of a character controller

	*/

	struct CLASS_DECLSPEC PhysControllerDesc
	{
		/** \brief Height of a controller. Overall height euqals radius * 2 + height */
		float height = 1.0f;

		/** \brief Radius of a controller. Overall height euqals radius * 2 + height */
		float radius = 0.25f;

		/** \brief Up vector of a controller */
		Math::Vector3 upVector = Math::Vector3(0.0f, 1.0f, 0.0f);

		/** \brief Postion of a controller */
		Math::Vector3 pos;

		/** \brief The maximum slope which the character can walk up. */
		float slopeLimit = cosf(Math::Radian * 20.0f);

		/** \brief  Defines the maximum height of an obstacle which the character can climb. */
		float stepOffset = 0.5f;
	};

	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief PhysController

	Kinematic character controller which is capsule.

	*/

	class CLASS_DECLSPEC PhysController : public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxQueryFilterCallback, public PxControllerFilterCallback, public PhysObjectBase
	{
		friend class PhysScene;
		PxController* controller = nullptr;
		float height = 1.0f;
		bool active = true;
		Math::Vector3 deactivePos;
		uint32_t collideGroup;
		PhysScene* scene = nullptr;

	public:

		enum CLASS_DECLSPEC CollideType
		{
			CollideSides = (1 << 0) /*!< Sides is collided */,
			CollideUp = (1 << 1) /*!< Upper point is collided */,
			CollideDown = (1 << 2) /*!< Lower point is collided */
		};

	#ifndef DOXYGEN_SKIP
		PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
		PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;

		// Implements PxUserControllerHitReport
		void onShapeHit(const PxControllerShapeHit& hit) override;
		void onControllerHit(const PxControllersHit& hit)  override {};
		void onObstacleHit(const PxControllerObstacleHit& hit) override {};

		// Implements PxControllerBehaviorCallback
		PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor)  override;
		PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
		PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;

		//PxControllerFilterCallback
		bool filter(const PxController& a, const PxController& b) override;
	#endif

		/**
		\brief Set active state of a object

		\param[in] set Active state
		*/
		void SetActive(bool set);

		/**
		\brief Check if a object is active

		\return Returns active state
		*/
		bool IsActive();

		/**
		\brief Set pointer to a user data

		\param[in] data Pointer to a user data
		*/
		void SetUserData(void* data);

		/**
		\brief Return pointer to a user data

		\return Pointer to a user data
		*/
		void* GetUserData();

		/**
		\brief Get overall heigh of a controller

		\return overall heigh of a controller
		*/
		float GetHeight();

		/**
		\brief Check if controller is colliding with something

		\param[in] type Type of collision

		\return True if there collision. Otherwise false will be returned.
		*/
		bool IsColliding(CollideType type);

		/**
		\brief Moves the character using a "collide-and-slide" algorithm.

		\param[in] dir Displacement vector
		\param[in] group With which collision group should be collided
		\param[in] group Which collision group should be ignored
		*/
		void Move(Math::Vector3 dir, uint32_t group);

		/**
		\brief Set up direction

		\param[in] up direction
		*/
		void SetUpDirection(Math::Vector3 up);

		/**
		\brief Get up direction

		\return up direction
		*/
		Math::Vector3 GetUpDirection() const;

		/**
		\brief Set belonging to a physical group

		\param[in] group Physical group
		*/
		void SetGroup(int group);

		/**
		\brief Set foot position of a controller

		\param[in] pos New foot position of a controller
		*/
		void SetFootPosition(Math::Vector3 pos);

		/**
		\brief Get foot position of a controller

		\return Current foot position of a controller
		*/
		Math::Vector3 GetFootPosition();

		/**
		\brief Set position of a controller

		\param[in] pos New position of a controller
		*/
		void SetPosition(Math::Vector3 pos);

		/**
		\brief Get position of a controller

		\return pos Current position of a controller
		*/
		Math::Vector3 GetPosition();

		/**
		\brief Get position of a controller's actor

		\return pos Current position of a controller's actor
		*/
		Math::Vector3 GetActorPosition();

		/**
		\brief PhysController should released only via this mehod
		*/
		void Release() override;

	protected:

		void ActualRelease() override;
	};
}