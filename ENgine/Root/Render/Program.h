
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <eastl/map.h>
#include "Support/ClassFactory.h"
#include "Support/PointerRef.h"

namespace Oak
{
	class Program
	{
		friend class Render;
		friend class PointerRef<Program>;

	protected:
		eastl::string name;
		int refCounter = 0;

		Shader* vshader = nullptr;
		Shader* pshader = nullptr;

		friend class DeviceDX11;
		friend class DeviceGLES;

		static eastl::map<eastl::string, Math::Vector4> vectors;
		static eastl::map<eastl::string, Math::Matrix> matrixes;
		static eastl::map<eastl::string, Texture*> textures;

	public:

		virtual bool Init();

		static void SetVector(const char* param, Math::Vector4* v);
		static void SetMatrix(const char* param, Math::Matrix* mat);
		static void SetTexture(const char* param, Texture* texture);
		virtual const char* GetVsName() = 0;
		virtual const char* GetPsName() = 0;
		virtual void ApplyStates() {};
		void SetVector(ShaderType shader_type, const char* param, Math::Vector4* v, int count);
		void SetMatrix(ShaderType shader_type, const char* param, Math::Matrix* mat, int count);
		void SetTexture(ShaderType shader_type, const char* param, Texture* texture);

		void Release();
	};

	CLASSFACTORYDEF(Program)
	CLASSFACTORYDEF_END()

	typedef PointerRef<Program> ProgramRef;
}