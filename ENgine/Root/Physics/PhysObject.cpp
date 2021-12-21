#include "PhysObject.h"
#include "PhysScene.h"
#include "Root/Root.h"

namespace Oak
{
	PhysObject::BodyType PhysObject::GetType()
	{
		return body_type;
	}

	void PhysObject::SetActive(bool set)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetActive for phys object during phys update");
			return;
		}

		if (set == is_active)
		{
			return;
		}

		is_active = set;

		actor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !set);

		PxShape* shape;
		actor->getShapes(&shape, 1);

		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, set);
	}

	bool PhysObject::IsActive()
	{
		return is_active;
	}

	void PhysObject::SetUserData(void* data)
	{
		actor->userData = data;
	}

	void* PhysObject::GetUserData()
	{
		return actor->userData;
	}

	void PhysObject::SetFixedRotation(bool set)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetFixedRotation for phys object during phys update");
			return;
		}

		((PxRigidDynamic*)actor)->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
	}

	void PhysObject::SetGroup(int group)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetGroup for phys object during phys update");
			return;
		}

		PxShape* shape;
		actor->getShapes(&shape, 1);

		PhysScene::SetShapeGroup(shape, group);
	}

	void PhysObject::SetTransform(Math::Matrix& mat)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call SetTransform for phys object during phys update");
			return;
		}

		Math::Matrix mat_offset = (body_type == Static) ? offset * mat : mat;

		PxMat33 m;
		m.column0 = PxVec3(mat_offset.Vx().x, mat_offset.Vx().y, mat_offset.Vx().z);
		m.column1 = PxVec3(mat_offset.Vy().x, mat_offset.Vy().y, mat_offset.Vy().z);
		m.column2 = PxVec3(mat_offset.Vz().x, mat_offset.Vz().y, mat_offset.Vz().z);

		PxTransform pT;
		pT.q = PxQuat(m);
		pT.p = PxVec3(mat_offset.Pos().x, mat_offset.Pos().y, mat_offset.Pos().z);

		if (body_type == Kinetic)
		{
			PxSceneWriteLock scopedLock(*actor->getScene());
			((PxRigidDynamic*)actor)->setKinematicTarget(pT);
		}
		else
		{
			actor->setGlobalPose(pT, true);
		}
	}

	void PhysObject::GetTransform(Math::Matrix& mat)
	{
		PxTransform pT = actor->getGlobalPose();
		Math::Quaternion quat(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
		quat.GetMatrix(mat);
		mat.Pos() = Math::Vector3(pT.p.x, pT.p.y, pT.p.z);
	}

	void PhysObject::AddForceAt(Math::Vector3 pos, Math::Vector3 force)
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call AddForceAt for phys object during phys update");
			return;
		}

		if (body_type != Dynamic && body_type != DynamicCCD)
		{
			return;
		}

		PxRigidBodyExt::addForceAtPos(*((PxRigidBody*)actor), PxVec3(force.x, force.y, force.z), PxVec3(pos.x, pos.y, pos.z), PxForceMode::eIMPULSE, true);
	}

	void PhysObject::RestrictZAxis()
	{
		if (scene->inPhysUpdate)
		{
			root.Log("Physics", "Can't call RestrictZAxis for phys object during phys update");
			return;
		}

		auto* rigid_actor = (PxRigidDynamic*)(actor);

		if (rigid_actor)
		{
			rigid_actor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
		}
	}

	void PhysObject::Release()
	{
		actor->userData = nullptr;
		PhysObjectBase::Release();
	}

	void PhysObject::ActualRelease()
	{
		PxShape* shape;
		actor->getShapes(&shape, 1);
		actor->detachShape(*shape);

		actor->release();
		delete this;
	}
}