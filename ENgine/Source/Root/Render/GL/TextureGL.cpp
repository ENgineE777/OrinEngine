
#include "TextureGL.h"
#include "Root/Root.h"

namespace Orin
{
	TextureGL::FormatDesc TextureGL::GetFormat(TextureFormat fmt)
	{
		FormatDesc desc;

		switch (fmt)
		{
		case TextureFormat::FMT_A8R8G8B8:
			desc.internal_format = GL_RGBA;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::FMT_A8R8:
			desc.internal_format = GL_LUMINANCE_ALPHA;
			desc.format = GL_LUMINANCE_ALPHA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::FMT_A8:
			desc.internal_format = GL_LUMINANCE;
			desc.format = GL_LUMINANCE;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::FMT_R16_FLOAT:
			desc.internal_format = GL_RED;
			desc.format = GL_RED;
			desc.type = GL_FLOAT;
			break;
		case TextureFormat::FMT_D16:
			desc.internal_format = GL_DEPTH_COMPONENT;
			desc.format = GL_DEPTH_COMPONENT;
			desc.type = GL_FLOAT;
			break;
		}

		return desc;
	}

	TextureGL::TextureGL(int w, int h, TextureFormat f, int l, bool is_rt, TextureType tp) : Texture(w, h, f, l, tp)
	{
		GLuint types[] = { GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_3D };
		gl_type = types[(int)tp];

		fdesc = GetFormat(format);

		glGenTextures(1, &texture);
		glBindTexture(gl_type, texture);

		if (lods == 0)
		{
			lods = GetLevels(width, height, 1);
		}

		w = width;
		h = height;

		for (int i = 0; i < lods; i++)
		{
			glTexImage2D(gl_type, i, fdesc.internal_format, w, h, 0, fdesc.format, fdesc.type, nullptr);

			NextMip(w, h, 1);
		}

		glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void TextureGL::Resize(int width, int height)
	{

	}

	void TextureGL::SetFilters(TextureFilter magmin, TextureFilter mipmap)
	{
		Texture::SetFilters(magmin, mipmap);

		glBindTexture(gl_type, texture);

		if (magmin == TextureFilter::Point)
		{
			glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (lods == 1)
		{

		}
		else
		{
			if (magminf == TextureFilter::Point && mipmapf == TextureFilter::Point)
			{
				glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			}
			else
			if (magminf == TextureFilter::Point && mipmapf == TextureFilter::Linear)
			{
				glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			}
			else
			if (magminf == TextureFilter::Linear && mipmapf == TextureFilter::Point)
			{
				glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
			else
			if (magminf == TextureFilter::Linear && mipmapf == TextureFilter::Linear)
			{
				glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}
		}
	}

	void TextureGL::SetAdress(TextureAddress adress)
	{
		Texture::SetAdress(adress);
	}

	void TextureGL::SetAdressImpl(GLuint coord, TextureAddress adress)
	{
		glBindTexture(gl_type, texture);

		if (adress == TextureAddress::Clamp)
		{
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		if (adress == TextureAddress::Mirror)
		{
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else
		{
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}

	void TextureGL::SetAdressU(TextureAddress adress)
	{
		Texture::SetAdressU(adress);
		SetAdressImpl(GL_TEXTURE_WRAP_S, adress);
	}


	void TextureGL::SetAdressV(TextureAddress adress)
	{
		Texture::SetAdressV(adress);
		SetAdressImpl(GL_TEXTURE_WRAP_T, adress);
	}

	void TextureGL::SetAdressW(TextureAddress adress)
	{
		Texture::SetAdressW(adress);
		SetAdressImpl(GL_TEXTURE_WRAP_R, adress);
	}

	void TextureGL::GenerateMips()
	{
		glBindTexture(gl_type, texture);
		glGenerateMipmap(gl_type);
	}

	void TextureGL::Apply(int slot)
	{
	}

	MappedTexture TextureGL::Lock(int level, int layer, TextureLockFlag lockFlag)
	{
		MappedTexture Mapped;
		return Mapped;
	}

	void TextureGL::Unlock()
	{

	}

	void* TextureGL::GetNativeResource()
	{
		return &texture;
	}

	void TextureGL::Update(int level, int layer, uint8_t* data, int stride)
	{
		glBindTexture(gl_type, texture);
		glTexSubImage2D(gl_type, level, 0, 0, width, height, fdesc.format, fdesc.type, data);
	}

	void TextureGL::Release()
	{
		if (root.render.textures.count(name) > 0)
		{
			root.render.textures.erase(name);
		}

		delete this;
	}
}