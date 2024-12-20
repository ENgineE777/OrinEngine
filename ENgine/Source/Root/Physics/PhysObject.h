
#pragma once

#include "PxPhysicsAPI.h"

#include "PxPhysicsAPI.h"
#include "PhysObjectBase.h"

using namespace physx;

namespace Orin
{
	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief PhysObject

	Just a physical box.

	*/

	class CLASS_DECLSPEC PhysObject : public PhysObjectBase
	{
		friend class PhysScene;

	public:

		enum CLASS_DECLSPEC BodyType
		{
			Static = 0 /*!< Hardware button in pressed state */,
			Dynamic /*!< Hardware button in pressed state */,
			DynamicCCD /*!< Hardware button in pressed state */,
			Kinetic /*!< Hardware button in pressed state */,
			Trigger /*!< Hardware button in pressed state */
		};

		/**
		\brief Set active state of a object

		\param[in] set Active state
		*/
		void SetActive(bool set) override;

		/**
		\brief Check if a object is active

		\return Returns active state
		*/
		bool IsActive() override;;

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
		\brief Locking and unlocking rotation

		\param[in] set Should be rotation locked or not
		*/
		void SetFixedRotation(bool set);

		/**
		\brief Get type of a object

		\return Type of a object
		*/
		BodyType GetType();

		/**
		\brief Get group

		\return Physical group
		*/
		int GetGroup();

		/**
		\brief Set belonging to a physical group

		\param[in] group Physical group
		*/
		void SetGroup(int group);

		/**
		\brief Set current transform

		\param[in] mat Matrix with transform
		*/
		void SetTransform(Math::Matrix& mat);

		/**
		\brief Get current transform

		\param[out] mat Output matrix
		*/
		void GetTransform(Math::Matrix& mat);

		/**
		\brief Apply force to point of a object

		\param[in] pos Position of applying of a force
		\param[in] force Strength of a force
		*/
		void AddForceAt(Math::Vector3 pos, Math::Vector3 force);

		/**
		\brief Restrict axis for making object as 2D
		*/
		void RestrictZAxis();

		/**
		\brief PhysObject should released only via this mehod
		*/
		void Release();

	protected:
		Math::Matrix offset;
		BodyType body_type;
		PxHeightField* heightField = nullptr;
		PxRigidActor* actor = nullptr;
		int group = 1;

		void ActualRelease() override;
	};
}
