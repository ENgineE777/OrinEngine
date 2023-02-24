#include "PhysController.h"
#include "PhysObject.h"
#include "PhysScene.h"
#include "Root/Root.h"

namespace Orin
{
	PX_INLINE void addForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
	{
		/*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
		{
		Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
		"PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
		return;
		}*/

		const PxTransform globalPose = body.getGlobalPose();
		const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

		const PxVec3 torque = (pos - centerOfMass).cross(force);
		body.addForce(force, mode, wakeup);
		body.addTorque(torque, mode, wakeup);
	}

	static void addForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true)
	{
		//transform pos to world space
		const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

		addForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
	}

	void PhysController::onShapeHit(const PxControllerShapeHit& hit)
	{
		if (hit.shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
		{
			PhysScene::BodyUserData* udataA = static_cast<PhysScene::BodyUserData*>(hit.shape->getActor()->userData);
			PhysScene::BodyUserData* udataB = static_cast<PhysScene::BodyUserData*>(controller->getUserData());

			if (udataA && udataB)
			{
				PhysScene::HandleSceneObjectContact("OnContact", udataA->object, udataA->index, udataB->object, udataB->index);
				PhysScene::HandleSceneObjectContact("OnContact", udataB->object, udataB->index, udataA->object, udataA->index);
			}
		}

		return;

		PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
		if (actor)
		{
			if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
				return;

			if (0)
			{
				const PxVec3 p = actor->getGlobalPose().p + hit.dir * 10.0f;

				PxShape* shape;
				actor->getShapes(&shape, 1);
				PxRaycastHit newHit;
				PxU32 n = PxShapeExt::raycast(*shape, *shape->getActor(), p, -hit.dir, 20.0f, PxHitFlag::ePOSITION, 1, &newHit);
				if (n)
				{
					// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
					// useless stress on the solver. It would be possible to enable/disable vertical pushes on
					// particular objects, if the gameplay requires it.
					const PxVec3 upVector = hit.controller->getUpDirection();
					const PxF32 dp = hit.dir.dot(upVector);
					//		shdfnd::printFormatted("%f\n", fabsf(dp));
					if (fabsf(dp)<1e-3f)
						//		if(hit.dir.y==0.0f)
					{
						const PxTransform globalPose = actor->getGlobalPose();
						const PxVec3 localPos = globalPose.transformInv(newHit.position);
						PxRigidBodyExt::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
					}
				}
			}

			// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
			// useless stress on the solver. It would be possible to enable/disable vertical pushes on
			// particular objects, if the gameplay requires it.
			const PxVec3 upVector = hit.controller->getUpDirection();
			const PxF32 dp = hit.dir.dot(upVector);
			//		shdfnd::printFormatted("%f\n", fabsf(dp));
			if (fabsf(dp)<1e-3f)
				//		if(hit.dir.y==0.0f)
			{
				const PxTransform globalPose = actor->getGlobalPose();
				const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
				PxRigidBodyExt::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
			}
		}
	}

	PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
	{
		PhysScene::BodyUserData* udataA = static_cast<PhysScene::BodyUserData*>(controller->getUserData());
		PhysScene::BodyUserData* udataB = static_cast<PhysScene::BodyUserData*>(actor.userData);

		if (udataA && udataB)
		{
			PhysScene::HandleSceneObjectContact("OnContact", udataA->object, udataA->index, udataB->object, udataB->index);
			PhysScene::HandleSceneObjectContact("OnContact", udataB->object, udataB->index, udataA->object, udataA->index);
		}

		if (udataB)
		{
			if (udataB->body->GetType() == PhysObject::Kinetic)
			{
				return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
			}
		}

		return PxControllerBehaviorFlags(0);
	}

	PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxController& controller)
	{
		return PxControllerBehaviorFlags(0);
	}

	PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxObstacle& obstacle)
	{
		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
	}

	bool PhysController::filter(const PxController& a, const PxController& b)
	{
		PhysScene::BodyUserData* udataA = static_cast<PhysScene::BodyUserData*>(GetUserData());
		PhysScene::BodyUserData* udataB = static_cast<PhysScene::BodyUserData*>(GetUserData() == a.getUserData() ? b.getUserData() : a.getUserData());

		auto actor = b.getActor();
		PxShape* shape;
		actor->getShapes(&shape, 1);

		if (!(collideGroup & shape->getQueryFilterData().word0))
		{
			return false;
		}

		return true;
	}

	PxQueryHitType::Enum PhysController::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
	{
		PhysScene::BodyUserData* udataA = static_cast<PhysScene::BodyUserData*>(GetUserData());
		PhysScene::BodyUserData* udataB = static_cast<PhysScene::BodyUserData*>(actor->userData);

		if (!(collideGroup & shape->getQueryFilterData().word0))
		{
			return PxQueryHitType::eNONE;
		}

		if (udataA && udataB)
		{
			if ((udataA->body && !udataA->body->IsActive()) ||
				(udataB->body && !udataB->body->IsActive()))
			{
				return PxQueryHitType::eNONE;
			}
			else
			{
				if (udataA->body && udataA->body->GetType() == PhysObject::Trigger)
				{
					PhysScene::HandleSceneObjectContact("OnContact", udataB->object, udataB->index, udataA->object, udataA->index);
					return PxQueryHitType::eTOUCH;
				}
				else
				if (udataB->body && udataB->body->GetType() == PhysObject::Trigger)
				{
					PhysScene::HandleSceneObjectContact("OnContact", udataA->object, udataA->index, udataB->object, udataB->index);
					return PxQueryHitType::eTOUCH;
				}
			}
		}

		return PxQueryHitType::eTOUCH;
	}

	PxQueryHitType::Enum PhysController::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
	{
		return PxQueryHitType::eTOUCH;
	}

	void PhysController::SetActive(bool set)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetActive for controller during phys update");
			return;
		}

		if (active == set)
		{
			return;
		}

		active = set;

		if (!active)
		{
			PxExtendedVec3 cpos = controller->getFootPosition();
			deactivePos = Math::Vector3((float)cpos.x, (float)cpos.y, (float)cpos.z);
		}

		controller->setFootPosition(active ? PxExtendedVec3(deactivePos.x, deactivePos.y, deactivePos.z) : PxExtendedVec3(-100000.0f, -100000.0f, -100000.0f));

		auto actor = controller->getActor();
		PxShape* shape;
		actor->getShapes(&shape, 1);

		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, set);
	}

	bool PhysController::IsActive()
	{
		return active;
	}

	void PhysController::SetUserData(void* data)
	{
		controller->getActor()->userData = data;
		controller->setUserData(data);
	}

	void* PhysController::GetUserData()
	{
		return controller->getUserData();
	}

	float PhysController::GetHeight()
	{
		return height;
	}

	bool PhysController::IsColliding(CollideType type)
	{
		if (!active)
		{
			return false;
		}

		PxControllerState cctState;
		controller->getState(cctState);

		if ((cctState.collisionFlags & type) == 0)
		{
			int k = 0;
			k++;
		}

		return (cctState.collisionFlags & type) != 0;

	}

	void PhysController::Move(Math::Vector3 dir, uint32_t group)
	{
		if (!active)
		{
			deactivePos += dir;
			return;
		}

		if (dir.Length2() < 0.0001f)
		{
			return;
		}

		if (collideGroup != group)
		{
			collideGroup = group;
			controller->invalidateCache();
		}
 
		PxControllerFilters filters = PxControllerFilters();
		filters.mFilterCallback = this;
		filters.mCCTFilterCallback = this;

		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call Move for controller during phys update");
		}
		else
		{
			scene->inPhysUpdate = true;
			const PxU32 flags = controller->move(PxVec3(dir.x, dir.y, dir.z), 0.0001f, 1.0f/60.0f, filters, NULL);
			scene->inPhysUpdate = false;
		}
	}

	void PhysController::SetUpDirection(Math::Vector3 up)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetUpDirection for controller during phys update");
		}
		else
		{
			controller->setUpDirection(PxVec3(up.x, up.y, up.z));
		}
	}

	Math::Vector3 PhysController::GetUpDirection() const
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call GetUpDirection for controller during phys update");
			return Math::Vector3{};
		}

		const physx::PxVec3 up = controller->getUpDirection();
		return Math::Vector3(up.x, up.y, up.z);
	}

	void PhysController::SetGroup(int group)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetGroup for controller during phys update");
			return;
		}

		auto actor = controller->getActor();
		PxShape* shape;
		actor->getShapes(&shape, 1);

		PhysScene::SetShapeGroup(shape, group, group);
	}

	void PhysController::SetFootPosition(Math::Vector3 pos)
	{
		if (!active)
		{
			deactivePos = pos;
			return;
		}

		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetPosition for controller during phys update");
		}
		else
		{
			controller->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
		}
	}

	Math::Vector3 PhysController::GetFootPosition()
	{
		if (!active)
		{
			return deactivePos;
		}

		PxExtendedVec3 cpos = controller->getFootPosition();
		return Math::Vector3((float)cpos.x, (float)cpos.y, (float)cpos.z);
	}

	void PhysController::SetPosition(Math::Vector3 pos)
	{
		if (!active)
		{
			deactivePos = pos;
			return;
		}

		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetPosition for controller during phys update");
		}
		else
		{
			controller->setPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
		}
	}

	Math::Vector3 PhysController::GetPosition()
	{
		if (!active)
		{
			return deactivePos;
		}

		PxExtendedVec3 cpos = controller->getPosition();
		return Math::Vector3((float)cpos.x, (float)cpos.y, (float)cpos.z);
	}

	Math::Vector3 PhysController::GetActorPosition()
	{
		if (!active)
		{
			return deactivePos;
		}

		PxTransform tm = controller->getActor()->getGlobalPose();
		return Math::Vector3((float)tm.p.x, (float)tm.p.y, (float)tm.p.z);
	}

	void PhysController::Release()
	{
		SetUserData(nullptr);
		PhysObjectBase::Release();
	}

	void PhysController::ActualRelease()
	{
		controller->release();
		delete this;
	}
}