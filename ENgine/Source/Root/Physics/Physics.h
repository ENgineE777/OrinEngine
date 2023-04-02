
#pragma once

#include "PhysScene.h"
#include "Root/Files/FileInMemory.h"
#include "Root/Files/File.h"

using namespace physx;

namespace Orin
{
	/**
	\ingroup gr_code_services_physic
	*/

	/**
	\brief Physics

	This is a manager of PhysScene objects.

	*/

	class Physics
	{
		friend class PhysScene;
		friend class PhysObjectBase;

		PxDefaultErrorCallback   defaultErrorCallback;
		PxDefaultAllocator       defaultAllocatorCallback;
		PxSimulationFilterShader defaultFilterShader = PxDefaultSimulationFilterShader;
		PxMaterial*              defMaterial = nullptr;

		PxFoundation* foundation = nullptr;
		PxPhysics*    physics = nullptr;

		static uint32_t groupCollisionFlags[32];
		static void SetCollisionFlag(uint32_t groups1, uint32_t groups2, bool enable);
		static physx::PxFilterFlags CollisionFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
														  physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
														  physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

		bool debugRender = false;

	#ifdef PLATFORM_WIN
		PxCooking*    cooking = nullptr;

		class StreamWriter : public PxOutputStream
		{
			File file;

		public:

			bool Prepere(const char* name)
			{
				return file.Open(name, File::ModeType::Write);
 			}

			virtual uint32_t write(const void* src, uint32_t count)
			{
				file.Write(src, count);

				return 1;
			}
		};
	#endif

		float physStep = 1.0f / 60.0f;
		float accum_dt = 0.0f;

		eastl::vector<PhysScene*> scenes;

		class StraemReader : public PxInputStream
		{
		public:
			FileInMemory buffer;
			virtual uint32_t read(void* dest, uint32_t count)
			{
				buffer.Read(dest, count);
				return count;
			}
		};

		eastl::vector<PhysObjectBase*> needed_to_delete;

	public:

	#ifndef DOXYGEN_SKIP
		void Init();
		void Update(float dt);
		void Fetch();
		void DeleteObjects();
	#endif

	#ifdef PLATFORM_WIN
		void CookHeightmap(int width, int height, uint8_t* hmap, const char* name);
	#endif

		/**
		\brief Creates a new physical scene

		\return Pointer to a PhysScene
		*/
		PhysScene* CreateScene();

		/**
		\brief This variable stores position on start and restors it when Reset was clled from script

		\
		*/
		void DestroyScene(PhysScene* scene);
	};
}