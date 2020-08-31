
#pragma once

#include "Root/Render/Render.h"

namespace Oak
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

		std::vector<Box> boxes;

		Program* prg;
		DataBuffer* vbuffer;
		VertexDecl* vdecl;
		DataBuffer* ibuffer;

	public:
	
		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddBox(Math::Matrix pos, Color color, Math::Vector3 scale);
		void Draw(float dt);
		void Release();
	};
}
