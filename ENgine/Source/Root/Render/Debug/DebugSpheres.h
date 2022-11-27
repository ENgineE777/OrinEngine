
#pragma once

#include "Root/Render/Render.h"

namespace Oak
{
	class DebugSpheres : public Object
	{
		enum
		{
			RigsCount = 16,
			SidesCount = 16,
			PrimCount = RigsCount * SidesCount * 2
		};

		struct Sphere
		{
			Math::Vector3 pos;
			Color   color;
			float   radius;
		};

		struct Vertex
		{
			Math::Vector3  pos;
			Math::Vector3  normal;
			uint32_t color;
		};

		eastl::vector<Sphere> spheres;

		RenderTechniqueRef prg;
		VertexDeclRef vdecl;
		DataBufferRef vbuffer;
		DataBufferRef ibuffer;

	public:
	
		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddSphere(Math::Vector3 pos, Color color, float radius);
		void Draw(float dt);
		void Release();
	};
}