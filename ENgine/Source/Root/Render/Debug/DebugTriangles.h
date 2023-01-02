
#pragma once

#include "Root/Render/Render.h"

namespace Orin
{
	class DebugTriangles : public Object
	{
		struct Triangle
		{
			Math::Vector3 p[3];
			uint32_t color;
		};

		struct Vertex
		{
			Math::Vector3 pos;
			Math::Vector3 normal;
			uint32_t color;
		};

		eastl::vector<Triangle> triangles;
		RenderTechniqueRef prg;
		VertexDeclRef vdecl;
		DataBufferRef vbuffer;

	public:
	
		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color);
		void Draw(float dt);
		void Release();
	};
}