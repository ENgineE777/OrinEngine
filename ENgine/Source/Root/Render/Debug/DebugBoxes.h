
#pragma once

#include "Root/Render/Render.h"

namespace Orin
{
	class DebugBoxes : public Object
	{
		struct Box
		{
			Math::Matrix trans;
			Color  color;
		};

		struct Vertex
		{
			Math::Vector3  pos;
			Math::Vector3  normal;
			uint32_t color;
		};

		eastl::vector<Box> boxes;

		RenderTechniqueRef prg;
		DataBufferRef vbuffer;
		VertexDeclRef vdecl;
		DataBufferRef ibuffer;

	public:
	
		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddBox(Math::Matrix pos, Color color, Math::Vector3 scale);
		void Draw(float dt);
		void Release();
	};
}
