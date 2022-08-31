
#include "Root/Root.h"

namespace Oak
{
	eastl::map<eastl::string, Math::Vector4> Program::vectors;
	eastl::map<eastl::string, Math::Matrix> Program::matrixes;
	eastl::map<eastl::string, Texture*> Program::textures;

	void Program::SetVector(const char* param, Math::Vector4* v)
	{
		Math::Vector4& vec = vectors[param];
		memcpy(&vec.x, &v->x, 4 * 4);
	}

	void Program::SetMatrix(const char* param, Math::Matrix* mat)
	{
		Math::Matrix& mt = matrixes[param];
		memcpy(&mt.m[0][0], &mat->m[0][0], 16 * 4);
	}

	void Program::SetTexture(const char* param, Texture* texture)
	{
		textures[param] = texture;
	}

	bool Program::Init()
	{
		if (GetVsName())
		{
			vshader = root.render.GetDevice()->CreateShader(ShaderType::Vertex, GetVsName());
		}

		if (GetPsName())
		{
			pshader = root.render.GetDevice()->CreateShader(ShaderType::Pixel, GetPsName());
		}

		return true;
	}

	void Program::SetVector(ShaderType shader_type, const char* param, Math::Vector4* v, int count)
	{
		if (shader_type == ShaderType::Vertex)
		{
			if (vshader) vshader->SetVector(param, v, count);
		}

		if (shader_type == ShaderType::Pixel)
		{
			if (pshader) pshader->SetVector(param, v, count);
		}
	}

	void Program::SetMatrix(ShaderType shader_type, const char* param, Math::Matrix* mat, int count)
	{
		if (shader_type == ShaderType::Vertex)
		{
			if (vshader) vshader->SetMatrix(param, mat, count);
		}

		if (shader_type == ShaderType::Pixel)
		{
			if (pshader) pshader->SetMatrix(param, mat, count);
		}
	}

	void Program::SetTexture(ShaderType shader_type, const char* param, Texture* texture)
	{
		if (shader_type == ShaderType::Vertex)
		{
			if (vshader) vshader->SetTexture(param, texture);
		}

		if (shader_type == ShaderType::Pixel)
		{
			if (pshader) pshader->SetTexture(param, texture);
		}
	}

	void Program::Release()
	{
		RELEASE(vshader)
		RELEASE(pshader)

		root.render.programs.erase(name);

		delete this;
	}
}

