
#include "Root/Root.h"

namespace Oak
{
	eastl::map<eastl::string, Math::Vector4> RenderTechnique::vectors;
	eastl::map<eastl::string, Math::Matrix> RenderTechnique::matrixes;
	eastl::map<eastl::string, Texture*> RenderTechnique::textures;

	void RenderTechnique::SetVector(const char* param, Math::Vector4* v)
	{
		Math::Vector4& vec = vectors[param];
		memcpy(&vec.x, &v->x, 4 * 4);
	}

	void RenderTechnique::SetMatrix(const char* param, Math::Matrix* mat)
	{
		Math::Matrix& mt = matrixes[param];
		memcpy(&mt.m[0][0], &mat->m[0][0], 16 * 4);
	}

	void RenderTechnique::SetTexture(const char* param, Texture* texture)
	{
		textures[param] = texture;
	}

	bool RenderTechnique::Init()
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

	void RenderTechnique::SetVector(ShaderType shader_type, const char* param, Math::Vector4* v, int count)
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

	void RenderTechnique::SetMatrix(ShaderType shader_type, const char* param, Math::Matrix* mat, int count)
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

	void RenderTechnique::SetTexture(ShaderType shader_type, const char* param, Texture* texture)
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

	void RenderTechnique::ReloadShaders()
	{
		RELEASE(vshader)
		RELEASE(pshader)

		if (GetVsName())
		{
			vshader = root.render.GetDevice()->CreateShader(ShaderType::Vertex, GetVsName());
		}

		if (GetPsName())
		{
			pshader = root.render.GetDevice()->CreateShader(ShaderType::Pixel, GetPsName());
		}
	}

	void RenderTechnique::Release()
	{
		RELEASE(vshader)
		RELEASE(pshader)

		root.render.renderTechniques.erase(hash);

		delete this;
	}
}

