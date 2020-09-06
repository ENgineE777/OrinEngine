
#pragma once

#include "Support/Support.h"

namespace Oak
{
	enum class ElementType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		UInt,
		UInt2,
		UInt3,
		UInt4,
		Ubyte4
	};

	enum class ElementSemantic
	{
		Position,
		Texcoord,
		Color
	};

	class VertexDecl
	{
	public:

		struct ElemDesc
		{
			ElementType type;
			ElementSemantic semantic;
			int index;
		};

		virtual void Release() = 0;
	};
}
