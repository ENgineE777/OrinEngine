
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <map>
#include "Support/ClassFactory.h"

namespace Oak
{
	class Program
	{
	protected:
		Shader* vshader = nullptr;
		Shader* pshader = nullptr;

		friend class DeviceDX11;
		friend class DeviceGLES;

		static std::map<std::string, Math::Vector4> vectors;
		static std::map<std::string, Math::Matrix> matrixes;
		static std::map<std::string, Texture*> textures;

	public:

		virtual bool Init();

		static void SetVector(const char* param, Math::Vector4* v);
		static void SetMatrix(const char* param, Math::Matrix* mat);
		static void SetTexture(const char* param, Texture* texture);
		virtual const char* GetVsName() = 0;
		virtual const char* GetPsName() = 0;
		virtual void ApplyStates() {};
		void SetVector(Shader::Type shader_type, const char* param, Math::Vector4* v, int count);
		void SetMatrix(Shader::Type shader_type, const char* param, Math::Matrix* mat, int count);
		void SetTexture(Shader::Type shader_type, const char* param, Texture* texture);
	};

	CLASSFACTORYDEF(Program)
	CLASSFACTORYDEF_END()
}