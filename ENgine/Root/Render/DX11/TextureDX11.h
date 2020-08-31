
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
		ID3D11SamplerState* sampler;
		bool sampler_need_recrete;

	public:
		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* srview;

		ID3D11RenderTargetView* rt;
		ID3D11DepthStencilView* depth;
	
		static int GetFormat(Format fmt);

		TextureDX11(int w, int h, Format f, int l, bool rt, Type tp);

		virtual void SetFilters(FilterType magmin, FilterType mipmap);
		virtual void SetAdress(TextureAddress adress);
		virtual void SetAdressU(TextureAddress adress);
		virtual void SetAdressV(TextureAddress adress);
		virtual void SetAdressW(TextureAddress adress);
	
		virtual void GenerateMips();
		virtual void Apply(int slot);

		virtual void Update(int level, int layer, uint8_t* data, int stride);
		virtual void Release();
	};
}