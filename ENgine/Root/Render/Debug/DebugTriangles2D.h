
#pragma once

#include "Root/Render/Render.h"

namespace Oak
{
	class DebugTriangles2D : public Object
	{
		struct Triangle
		{
			Math::Vector2 p[3];
			uint32_t color;
		};

		struct Vertex
		{
			Math::Vector2  pos;
			uint32_t color;
		};

		eastl::vector<Triangle> triangles;
		Program* prg;
		VertexDecl* vdecl;
		DataBuffer* vbuffer;

	public:
	
		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddTriangle(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color);
		void Draw(float dt);
		void Release();
	};
}