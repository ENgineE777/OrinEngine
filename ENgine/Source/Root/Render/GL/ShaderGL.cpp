
#include "ShaderGL.h"
#include "DeviceGL.h"
#include "VertexDeclGL.h"
#include "TextureGL.h"
#include "DataBufferGL.h"
#include "Root/Root.h"
#include "Root/Files/FileInMemory.h"

namespace Orin
{
	ShaderGL::ShaderGL(ShaderType tp, const char* name) : Shader(tp)
	{
		int gl_type = GL_VERTEX_SHADER;
		if (tp == ShaderType::Pixel)
		{
			gl_type = GL_FRAGMENT_SHADER;
		}

		shader = glCreateShader(gl_type);

		FileInMemory buffer;
		char path[1024];
		StringUtils::Printf(path, 1024, "Shaders/GLES/%s", name);

		if (buffer.Load(path))
		{
			const char* buff = (const char*)buffer.GetData();
			glShaderSource(shader, 1, (const char**)&buff, NULL);

			glCompileShader(shader);

			GLint logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

			if (logLength > 0)
			{
				GLchar* log = (GLchar*)malloc(logLength);
				glGetShaderInfoLog(shader, logLength, &logLength, log);

				root.Log("Render", "Shader %s with type %i was not compiled: %s", name, tp, log);

				free(log);
			}

			int status = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status == 0)
			{
				glDeleteShader(shader);
			}
		}
	}

	ShaderGL::Param& ShaderGL::GetParam(const char* name)
	{
		if (params.count(name) == 0)
		{
			params[name].param = glGetUniformLocation(program, name);
		}

		return params[name];
	}

	bool ShaderGL::SetVector(const char* param, Math::Vector4* v, int count)
	{
		Param& p = GetParam(param);

		if (p.param != -1)
		{
			glUniform4fv(p.param, count, (float*)v);
			return true;
		}

		return false;
	}

	bool ShaderGL::SetMatrix(const char* param, Math::Matrix* m, int count)
	{
		Param& p = GetParam(param);

		if (p.param != -1)
		{
			glUniformMatrix4fv(p.param, count, false, (float*)m);
			return true;
		}

		return false;
	}

	bool ShaderGL::SetTexture(const char* param, Texture* tex)
	{
		Param& p = GetParam(param);

		if (p.index == -1)
		{
			p.index = tex_index;
			tex_index++;
		}

		if (p.param != -1)
		{
			if (tex)
			{
				glActiveTexture(GL_TEXTURE0 + p.index);
				glBindTexture(GL_TEXTURE_2D, ((TextureGL*)tex)->texture);
			}

			glUniform1i(p.param, p.index);

			return true;
		}

		return false;
	}

	void ShaderGL::Apply()
	{
	}

	void ShaderGL::UpdateConstants()
	{
	}

	void ShaderGL::Release()
	{

	}
}