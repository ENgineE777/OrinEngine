
#pragma once

#include "Root/Render/Texture.h"

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
	class TextureGL : public Texture
	{
	public:

		GLuint texture;
		GLuint gl_type;

		struct FormatDesc
		{
			int internal_format = 0;
			int format = 0;
			int type = 0;
		};

		FormatDesc fdesc;

		static FormatDesc GetFormat(TextureFormat fmt);

		TextureGL(int w, int h, TextureFormat f, int l, bool rt, TextureType tp);

		void Resize(int width, int height) override;

		void SetFilters(TextureFilter magmin, TextureFilter mipmap) override;
		void SetAdressImpl(GLuint coord, TextureAddress adress);
		void SetAdress(TextureAddress adress) override;
		void SetAdressU(TextureAddress adress) override;
		void SetAdressV(TextureAddress adress) override;
		void SetAdressW(TextureAddress adress) override;

		void GenerateMips() override;
		void Apply(int slot) override;

		MappedTexture Lock(int level, int layer, TextureLockFlag lockFlag) override;
		void Unlock() override;

		void* GetNativeResource() override;

		void Update(int level, int layer, uint8_t* data, int stride) override;
		void Release() override;
	};
}
