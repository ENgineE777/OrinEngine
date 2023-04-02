
#pragma once

#include "PhysObject.h"
#include "PhysController.h"
#include "PhysSphericalJoint.h"

using namespace physx;

namespace Orin
{
	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief PhysScene

	This is representation of a physical scene. Objects to a scene are adding via proper
	methods of PhysScene.

	*/

	class CLASS_DECLSPEC PhysScene : public PxSimulationEventCallback
	{
		friend class Physics;
		friend class PhysObject;
		friend class PhysController;

		PxScene* scene = nullptr;
		bool needFetch = false;
		PxControllerManager* manager = nullptr;
		bool inPhysUpdate = false;

		static void SetShapeGroup(PxShape* shape, uint32_t group);
		PhysObject* CreatePhysObject(const PxGeometry& geometry, Math::Matrix trans, Math::Matrix offset, PhysObject::BodyType type, uint32_t group);

	public:

		/**
		\brief BodyUserData

		This struct used as a wrapper around SceneObject and pointer sets to this wraper via SetUserData.

		*/

		struct CLASS_DECLSPEC BodyUserData
		{
			/** \brief Poninter to a scene object */
			class SceneEntity* object = nullptr;

			/** \brief Index of a instance */
			int index = -1;

			/** \brief Pointer to a physical object */
			PhysObject* body = nullptr;

			/** \brief Ponter to a character controller */
			class PhysController* controller = nullptr;
		};

		/**
		\brief RaycastDesc

		This struct used in RayCast call for passing ray data and getting result of a ray cast.

		*/

		struct CLASS_DECLSPEC RaycastDesc
		{
			/** \brief Origin of a ray */
			Math::Vector3 origin;

			/** \brief Direction of a ray. Should be normalized */
			Math::Vector3 dir;

			/** \brief Length of a ray */
			float length;

			/** \brief Hit postion of a ray */
			Math::Vector3 hitPos;

			/** \brief Hit normal of a ray */
			Math::Vector3 hitNormal;

			/** \brief Hit length of a ray */
			float hitLength;

			/** \brief Group which should be tested */
			uint32_t group;

			/** \brief Pointer to a BodyUserData */
			BodyUserData* userdata = nullptr;
		};

		void SetCollisionFlag(uint32_t groups1, uint32_t groups2, bool enable);

		/**
		\brief Create a physical box in a scene

		\param[in] size Dimestion of a box
		\param[in] trans Transformation of a box
		\param[in] offset Offset of a orgin in local space
		\param[in] type Type of a body
		\param[in] group Belonging to a physical group

		\return Pointer to physical object
		*/
		PhysObject* CreateBox(Math::Vector3 size, Math::Matrix trans, Math::Matrix offset, PhysObject::BodyType type, uint32_t group);

		/**
		\brief Create a physical sphere in a scene

		\param[in] radius Radius of a sphere
		\param[in] trans Transformation of a sphere
		\param[in] offset Offset of a orgin in local space
		\param[in] type Type of a body
		\param[in] group Belonging to a physical group

		\return Pointer to physical object
		*/
		PhysObject* CreateSphere(float radius, Math::Matrix trans, Math::Matrix offset, PhysObject::BodyType type, uint32_t group);

		/**
		\brief Create a physical capsule in a scene

		\param[in] radius Radius of a capsule
		\param[in] height Height of a capsule
		\param[in] trans Transformation of a capsule
		\param[in] offset Offset of a orgin in local space
		\param[in] type Type of a body
		\param[in] group Belonging to a physical group

		\return Pointer to physical object
		*/
		PhysObject* CreateCapsule(float radius, float height, Math::Matrix trans, Math::Matrix offset, PhysObject::BodyType type, uint32_t group);

		/**
		\brief Create a character controller in a scene

		\param[in] desc Description of charater controller
		\param[in] group Belonging to a physical group

		\return Pointer to a character controller
		*/
		PhysController* CreateController(PhysControllerDesc& desc, uint32_t group);

		/**
		\brief Create a static heighfiled in a scene

		\param[in] desc Description of height map 
		\param[in] name Full path to a cooked heigh map
		\param[in] group Belonging to a physical group

		\return Pointer to a height map
		*/
		PhysObject* CreateHeightmap(int width, int height, Math::Vector2  scale, const char* name, uint32_t group);

		/**
		\brief Make ray cast

		\param[in] desc Description of a ray

		\return True will be return in case ray hitted something. Otherwise false will be returned 
		*/
		bool RayCast(RaycastDesc& desc);

		/**
		\brief Make sphere sweep cast

		\param[in] desc Description of a sphere sweep

		\return True will be return in case sphere hitted something. Otherwise false will be returned 
		*/
		bool SphereCast(RaycastDesc& desc, float radius);

		/**
		\brief Make ray cast against given box.

		\param[in] desc Description of a ray

		\return True will be return in case ray hitted given box. Otherwise false will be returned 
		*/
		static bool RayCastBox(RaycastDesc& desc, Math::Matrix boxTrans, Math::Vector3 boxHalfSize);

		PhysSphericalJoint* CreateSphericalJoint(PhysObject* bodyA, Math::Matrix transA, PhysObject* bodyB, Math::Matrix transB);

		bool OverlapWithSphere(Math::Vector3 pos, float radius, uint32_t physGroup, eastl::vector<BodyUserData*>& bodies);
		bool OverlapWithBox(Math::Matrix trans, Math::Vector3 size, uint32_t physGroup, eastl::vector<BodyUserData*>& bodies);

	#ifndef DOXYGEN_SKIP

		void SetVisualization(bool set);
		void DrawVisualization();

		void Simulate(float dt);
		void FetchResults();

		void Release();

		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {};
		void onWake(PxActor** actors, PxU32 count) {};
		void onSleep(PxActor** actors, PxU32 count) {};
		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
		void onTrigger(PxTriggerPair* pairs, PxU32 count);
		void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};

		static void HandleSceneObjectContact(const char* callback_name, class SceneEntity* object, int index, class SceneEntity* contact_object, int contact_index);
	#endif
	};
}