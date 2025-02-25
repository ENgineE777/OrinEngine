
#pragma once

#include "Root/Render/Shader.h"
#include <vector>
#include <map>
#include <stdint.h>

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
	class ShaderGL : public Shader
	{
		Texture* textures[16];

		struct Param
		{
			GLuint param = -1;
			int    index = -1;
		};

		int tex_index = 0;

		eastl::map<eastl::string, Param> params;

		Param& GetParam(const char* name);

	public:

		GLuint shader = -1;
		GLuint program;

		ShaderGL(ShaderType tp, const char* name);

		virtual bool SetVector(const char* param, Math::Vector4* v, int count);
		virtual bool SetMatrix(const char* param, Math::Matrix* m, int count);
		virtual bool SetTexture(const char* param, Texture* tex);

		virtual void Apply();
		virtual void UpdateConstants();

		virtual void Release();
	};
}