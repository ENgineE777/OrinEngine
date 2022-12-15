
#pragma once

#include "Root/Render/Shader.h"
#include "Root/Files/FileInMemory.h"
#include <eastl/vector.h>
#include <eastl/map.h>
#include <stdint.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

namespace Oak
{
	class ShaderDX11 : public Shader
	{
		friend class VertexDeclDX11;

		ID3D11VertexShader* vshader = nullptr;
		ID3D11PixelShader*  pshader = nullptr;

		struct ConstantBuffer
		{
			uint8_t* rawdata = nullptr;
			int      size = 0;
			int      slot = 0;
			bool     dirty = false;
			ID3D11Buffer* buffer = nullptr;
		};

		struct ShaderParamInfo
		{
			int slot = -1;
			int offset = 0;
			int size = 0;
		};

		eastl::vector<ConstantBuffer> buffers;
		eastl::map<eastl::string, ShaderParamInfo> constantsInfo;
		eastl::map<eastl::string, ShaderParamInfo> samplersInfo;

		FileInMemory buffer;
		class TextureDX11* textures[16];
		virtual void Release();

	public:

		ShaderDX11(ShaderType tp, const char* name);

		virtual bool SetVector(const char* param, Math::Vector4* v, int count);
		virtual bool SetMatrix(const char* param, Math::Matrix* m, int count);
		virtual bool SetTexture(const char* param, Texture* tex);

		virtual void Apply();
		virtual void UpdateConstants();
	};
}