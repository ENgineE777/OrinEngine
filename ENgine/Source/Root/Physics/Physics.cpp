#include "Physics.h"
#include "Root/Render/Render.h"
#include "Root/Root.h"

#include <cmath>

namespace Orin
{
	#ifdef PLATFORM_ANDROID
	extern "C"
	{
		int android_getCpuCount()
		{
			return 1;
		}
	}
	#endif

	uint32_t Physics::groupCollisionFlags[32];

	physx::PxFilterFlags Physics::CollisionFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
														physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
														physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		// let triggers through, and do any other prefiltering you need.
		//if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		//{
			//pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			//return PxFilterFlag::eDEFAULT;
		//}

		// generate contacts for all that were not filtered above
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		int index1 = (int)log2(filterData0.word0);
		int index2 = (int)log2(filterData1.word0);

		uint32_t ShapeGroup0 = index1 & 31;
		uint32_t ShapeGroup1 = index2 & 31;

		if ((groupCollisionFlags[ShapeGroup0] & (1 << ShapeGroup1)) == 0)
		{
			return PxFilterFlag::eKILL;
		}

		return PxFilterFlag::eDEFAULT;
	}

	void Physics::SetCollisionFlag(PxU32 groups1, PxU32 groups2, bool enable)
	{
		groups1 = (int)log2(groups1) & 31;
		groups2 = (int)log2(groups2) & 31;

		if (enable)
		{
			//be symmetric:
			groupCollisionFlags[groups1] |= (1 << groups2);
			groupCollisionFlags[groups2] |= (1 << groups1);
		}
		else
		{
			groupCollisionFlags[groups1] &= ~(1 << groups2);
			groupCollisionFlags[groups2] &= ~(1 << groups1);
		}
	}

	void Physics::Init()
	{
		PxTolerancesScale tolerancesScale;

		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale, true);

	#ifdef PLATFORM_WIN
		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(tolerancesScale));
	#endif

		defMaterial = physics->createMaterial(0.5f, 0.5f, 0.0005f);

		for (unsigned i = 0; i < 32; i++)
		{
			groupCollisionFlags[i] = 0xffffffff;
		}
	}

	#ifdef PLATFORM_WIN
	void Physics::CookHeightmap(int width, int height, uint8_t* hmap, const char* name)
	{
		PxHeightFieldSample* samples = new PxHeightFieldSample[width * height];

		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				samples[x + y * width].height = static_cast<PxI16>(hmap[((x)*width + y)]);

				samples[x + y * width].clearTessFlag();
				samples[x + y * width].materialIndex0 = 0;
				samples[x + y * width].materialIndex1 = 0;
			}

		PxHeightFieldDesc heightFieldDesc;

		heightFieldDesc.nbColumns = width;
		heightFieldDesc.nbRows = height;
		heightFieldDesc.samples.data = samples;
		heightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);

		StreamWriter writer;
		if (writer.Prepere(name))
		{
			cooking->cookHeightField(heightFieldDesc, writer);
		}
	}
	#endif

	PhysScene* Physics::CreateScene()
	{
		PhysScene* scene = new PhysScene();

		PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
		sceneDesc.simulationEventCallback = scene;
		sceneDesc.filterShader = CollisionFilterShader;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;		

		if (!sceneDesc.cpuDispatcher)
		{
			sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		}

		sceneDesc.isValid();

		scene->scene = physics->createScene(sceneDesc);
		scene->manager = PxCreateControllerManager(*scene->scene);

		scenes.push_back(scene);

		return scene;
	}

	void Physics::DestroyScene(PhysScene* scene)
	{
		for (int i=0; i<scenes.size(); i++)
		{
			if (scenes[i] == scene)
			{
				scenes.erase(scenes.begin() + i);
				scene->Release();
				break;
			}
		}
	}

	void Physics::Update(float dt)
	{
		accum_dt += dt;

		if (accum_dt > 0.5f)
		{
			accum_dt = 0.5f;
		}

		if (accum_dt > physStep)
		{
			for (auto scene : scenes)
			{
				if (!scene->needFetch)
				{
				}

				scene->Simulate(physStep);
				scene->needFetch = true;
			}

			accum_dt -= physStep;
		}
	}

	void Physics::Fetch()
	{
		for (int i = 0; i < scenes.size(); i++)
		{
			PhysScene* scene = scenes[i];

			if (!scene->needFetch)
			{
				continue;
			}

			scene->FetchResults();
			scene->needFetch = false;
		}

		if (root.controls.DebugHotKeyPressed("KEY_LCONTROL", "KEY_N"))
		{
			debugRender = !debugRender;
		}

		for (auto scene : scenes)
		{
			scene->SetVisualization(debugRender);

			if (!debugRender)
			{
				continue;
			}

			scene->DrawVisualization();
		}
	}

	void Physics::DeleteObjects()
	{
		for (auto object : needed_to_delete)
		{
			object->ActualRelease();
		}

		needed_to_delete.clear();
	}
}