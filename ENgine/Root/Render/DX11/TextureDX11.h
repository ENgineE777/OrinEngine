
#pragma once

#include "Root/Render/Texture.h"

struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Oak
{
	class TextureDX11 : public Texture
	{
		friend class DeviceDX11;
		friend class ShaderDX11;

		ID3D11SamplerState* sampler;
		bool sampler_need_recrete;

		virtual void Release();

		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* srview;

		ID3D11RenderTargetView* rt;
		ID3D11DepthStencilView* depth;
	
		static int GetFormat(TextureFormat fmt);

		TextureDX11(int w, int h, TextureFormat f, int l, bool rt, TextureType tp);

		public:

		virtual void SetFilters(TextureFilter magmin, TextureFilter mipmap);
		virtual void SetAdress(TextureAddress adress);
		virtual void SetAdressU(TextureAddress adress);
		virtual void SetAdressV(TextureAddress adress);
		virtual void SetAdressW(TextureAddress adress);
	
		virtual void GenerateMips();

		virtual void Update(int level, int layer, uint8_t* data, int stride);

		protected:

		virtual void Apply(int slot);
	};
}