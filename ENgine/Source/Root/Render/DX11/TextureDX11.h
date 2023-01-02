
#pragma once

#include "Root/Render/Texture.h"

struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Orin
{
	class TextureDX11 : public Texture
	{
		friend class DeviceDX11;
		friend class ShaderDX11;

		ID3D11SamplerState* sampler;
		bool sampler_need_recrete;

		virtual void Release();

		ID3D11Texture2D* texture = nullptr;
		ID3D11ShaderResourceView* srview = nullptr;

		ID3D11RenderTargetView* rt = nullptr;
		ID3D11DepthStencilView* depth = nullptr;
	
		static int GetFormat(TextureFormat fmt);

		TextureDX11(int w, int h, TextureFormat f, int l, bool rt, TextureType tp);

		void CreateTexture();

		public:

		void Resize(int width, int height) override;

		void SetFilters(TextureFilter magmin, TextureFilter mipmap) override;
		void SetAdress(TextureAddress adress) override;
		void SetAdressU(TextureAddress adress) override;
		void SetAdressV(TextureAddress adress) override;
		void SetAdressW(TextureAddress adress) override;
	
		void GenerateMips() override;

		void Update(int level, int layer, uint8_t* data, int stride) override;

		void* GetNativeResource() override;

		protected:

		void Apply(int slot) override;
	};
}