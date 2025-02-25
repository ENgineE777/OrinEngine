
#pragma once

#include "Root/Render/VertexDecl.h"

#if PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#include "glad.h"
#endif

#if PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

namespace Orin
{
	class VertexDeclGL : public VertexDecl
	{
	public:

		struct VertexAttrib
		{
			int type;
			int components;
			int offset;
		};

		eastl::vector<VertexAttrib> attribs;

		VertexDeclGL(int count, VertexDecl::ElemDesc* elems);
		void Apply();

		virtual void Release();
	};
}
