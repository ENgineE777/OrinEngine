
#include "Physics.h"
#include "Root/Root.h"
#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	PhysObject* PhysScene::CreateBox(Math::Vector3 size, Math::Matrix trans, Math::Matrix offset, PhysObject::BodyType type, uint32_t group)
	{
		PhysObject* obj = new PhysObject();

		obj->body_type = type;

		PxReal density = 1.0f;

		Math::Quaternion q(trans);

		PxTransform transform(PxVec3(trans.Pos().x, trans.Pos().y, trans.Pos().z), PxQuat(q.x, q.y, q.z, q.w));

		PxVec3 dimensions(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

		PxBoxGeometry geometry(dimensions);
		PxShape* shape;

		if (type == PhysObject::Static || type == PhysObject::Trigger)
		{
			obj->actor = root.physics.physics->createRigidStatic(transform);
			shape = root.physics.physics->createShape(geometry, *root.physics.defMaterial, true);

			obj->actor->attachShape(*shape);

			obj->offset = offset;

			if (type == PhysObject::Trigger)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}
		}
		else
		{
			Math::Quaternion q_offset(offset);
			PxTransform trans_offset(PxVec3(offset.Pos().x, offset.Pos().y, offset.Pos().z), PxQuat(q_offset.x, q_offset.y, q_offset.z, q_offset.w));

			PxRigidDynamic* actor = PxCreateDynamic(*root.physics.physics, transform, geometry, *root.physics.defMaterial, density, trans_offset);

			actor->getShapes(&shape, 1);

			if (type == PhysObject::Kinetic)
			{
				actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			}
			else
			if (type == PhysObject::DynamicCCD)
			{
				actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			}

			obj->actor = actor;
		}

		scene->addActor(*obj->actor);
		SetShapeGroup(shape, group);

		return obj;
	}

	PhysController* PhysScene::CreateController(PhysControllerDesc& desc, uint32_t group)
	{
		PxCapsuleControllerDesc pxdesc;
		pxdesc.height = desc.height;
		pxdesc.radius = desc.radius;
		pxdesc.position = PxExtendedVec3(desc.pos.x, desc.pos.y, desc.pos.z);
		pxdesc.upDirection = PxVec3(desc.upVector.x, desc.upVector.y, desc.upVector.z);
		pxdesc.density = 1.0f;
		pxdesc.slopeLimit = desc.slopeLimit;
		pxdesc.contactOffset = 0.01f;
		pxdesc.stepOffset = 0.5f;
		pxdesc.scaleCoeff = 0.8f;
		pxdesc.material = root.physics.defMaterial;

		PhysController* controller = new PhysController();
		pxdesc.reportCallback = controller;
		pxdesc.behaviorCallback = controller;

		controller->controller = manager->createController(pxdesc);
		controller->height = desc.height + desc.radius * 2.0f + controller->controller->getContactOffset();

		auto actor = controller->controller->getActor();
		PxShape* shape;
		actor->getShapes(&shape, 1);

		SetShapeGroup(shape, group);

		return controller;
	}

	PhysObject* PhysScene::CreateHeightmap(int width, int height, Math::Vector2  scale, const char* name, uint32_t group)
	{
		PhysObject* hm = nullptr;

		Physics::StraemReader reader;
		if (reader.buffer.Load(name))
		{
			hm = new PhysObject();
			hm->body_type = PhysObject::PhysObject::Static;

			hm->heightField = root.physics.physics->createHeightField(reader);

			if (hm->heightField)
			{
				PxTransform pose = PxTransform(PxVec3(-width * 0.5f * scale.x, 0.0f, -height * 0.5f * scale.x), PxQuat(PxIdentity));

				hm->actor = root.physics.physics->createRigidStatic(pose);

				PxHeightFieldGeometry hfGeom(hm->heightField, PxMeshGeometryFlags(), scale.y, scale.x, scale.x);
				PxShape* shape = root.physics.physics->createShape(hfGeom, *root.physics.defMaterial, true);
				SetShapeGroup(shape, group);
				shape->setFlag(PxShapeFlag::Enum::eVISUALIZATION, false);
				hm->actor->attachShape(*shape);

				scene->addActor(*hm->actor);
			}
		}

		return hm;
	}

	void PhysScene::SetVisualization(bool set)
	{
		scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, set ? 1.0f : 0.0f);
		scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, set ? 1.0f : 0.0f);
	}

	void PhysScene::DrawVisualization()
	{
		const PxRenderBuffer& rb = scene->getRenderBuffer();
		for (PxU32 i = 0; i < rb.getNbLines(); i++)
		{
			const PxDebugLine& line = rb.getLines()[i];
			root.render.DebugLine(Math::Vector3(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
				Math::Vector3(line.pos0.x, line.pos0.y, line.pos0.z), COLOR_GREEN, false);
		}
	}

	bool PhysScene::RayCast(RaycastDesc& desc)
	{
		if (desc.length < 0.005f)
		{
			return false;
		}

		PxRaycastBuffer hit;
		PxQueryFilterData filterData = PxQueryFilterData();
		filterData.data.word0 = desc.group;


		if (scene->raycast(PxVec3(desc.origin.x, desc.origin.y, desc.origin.z), PxVec3(desc.dir.x, desc.dir.y, desc.dir.z), desc.length, hit, PxHitFlags(PxHitFlag::eDEFAULT), filterData))
		{
		
			desc.userdata = (BodyUserData*)(hit.block.actor->userData);
			desc.hitPos = Math::Vector3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
			desc.hitNormal = Math::Vector3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);

			return true;
		}

		return false;
	}

	bool PhysScene::OverlapWithSphere(Math::Vector3 pos, float radius, eastl::vector<BodyUserData*>& bodies)
	{
		PxOverlapHit aTouches[128];

		PxOverlapBuffer hit(aTouches, 128);
		PxSphereGeometry sphere = PxSphereGeometry(radius);
		PxTransform pose = PxTransform(PxVec3(pos.x, pos.y, pos.z));

		PxQueryFilterData filterData = PxQueryFilterData();
		filterData.data.word0 = 1;

		if (scene->overlap(sphere, pose, hit, filterData))
		{
			bodies.resize(hit.getNbTouches());

			for (int i = 0; i < (int)hit.getNbTouches(); i++)
			{
				bodies[i] = (BodyUserData*)(aTouches[i].actor->userData);
			}

			return true;
		}

		return false;
	}

	void PhysScene::Simulate(float dt)
	{
		scene->simulate(dt);
	}

	void PhysScene::FetchResults()
	{
		while (!scene->fetchResults())
		{

		}
	}

	void PhysScene::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		BodyUserData* udataA = static_cast<BodyUserData*>(pairHeader.actors[0]->userData);
		BodyUserData* udataB = static_cast<BodyUserData*>(pairHeader.actors[1]->userData);

		if (udataA && udataB)
		{
			PhysScene::HandleSceneObjectContact("OnContact", udataA->object, udataA->index, udataB->object, udataB->index);
			PhysScene::HandleSceneObjectContact("OnContact", udataB->object, udataB->index, udataA->object, udataA->index);
		}
	}

	void PhysScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			BodyUserData* udataA = static_cast<BodyUserData*>(pairs[i].triggerActor->userData);
			BodyUserData* udataB = static_cast<BodyUserData*>(pairs[i].otherActor->userData);

			//eNOTIFY_TOUCH_FOUND or eNOTIFY_TOUCH_LOST

			if (udataA && udataB)
			{
				if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					PhysScene::HandleSceneObjectContact("OnContactStart", udataA->object, udataA->index, udataB->object, udataB->index);
				}

				if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					PhysScene::HandleSceneObjectContact("OnContactEnd", udataA->object, udataA->index, udataB->object, udataB->index);
				}
			}
		}
	}

	void PhysScene::Release()
	{
		scene->release();
		delete this;
	}

	void PhysScene::HandleSceneObjectContact(const char* callback_name, SceneEntity* object, int index, SceneEntity* contact_object, int contact_index)
	{
		eastl::function<void(int, SceneEntity*, int)> callback;

		callback = object->GetCallback<eastl::function<void(int, SceneEntity*, int)>>(callback_name);

		if (callback)
		{
			callback(index, contact_object, contact_index);
		}
	}
}