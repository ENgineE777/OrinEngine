#include "Physics.h"
#include "Root/Render/Render.h"
#include "Root/Root.h"

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

	void Physics::Init()
	{
		PxTolerancesScale tolerancesScale;

		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale, true);

	#ifdef PLATFORM_WIN
		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(tolerancesScale));
	#endif

		defMaterial = physics->createMaterial(0.5f, 0.5f, 0.0005f);
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

	physx::PxFilterFlags CollisionFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
											   physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
											   physx::PxPairFlags& retPairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		retPairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;

		PxU32 group0 = filterData0.word1 != filterData0.word0 ? filterData0.word1 : filterData0.word0;
		PxU32 group1 = filterData1.word1 != filterData1.word0 ? filterData1.word1 : filterData1.word0;

		if (filterData0.word1 != filterData0.word0 && filterData1.word1 != filterData1.word0)
		{
			group0 = filterData0.word0;
			group1 = filterData1.word1;
		}

		if (!(group0 & group1))
		{
			return PxFilterFlag::eKILL;
		}

		return PxFilterFlag::eDEFAULT;
	}

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