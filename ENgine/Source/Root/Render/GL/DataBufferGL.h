
#pragma once

#include "Root/Render/DataBuffer.h"
#include <vector>

#if PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#include "glad.h"
#endif

#if PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif


#ifdef PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

namespace Orin
{
	class DataBufferGL : public DataBuffer
	{
	public:

		GLuint buffer;

		DataBufferGL(int sz, int strd);

		virtual void* Lock();
		virtual void Unlock();
		virtual void Release();
	};
}
