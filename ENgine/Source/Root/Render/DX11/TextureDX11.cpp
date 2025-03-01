
#include "TextureDX11.h"
#include "DeviceDX11.h"
#include "Root/Root.h"

#include "d3d11.h"

namespace Orin
{
	int TextureDX11::GetFormat(TextureFormat fmt)
	{
		switch (fmt)
		{
			case TextureFormat::FMT_A8R8G8B8: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::FMT_A8R8: return DXGI_FORMAT_R8G8_UNORM;
			case TextureFormat::FMT_A8: return DXGI_FORMAT_R8_UNORM;
			case TextureFormat::FMT_R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
			case TextureFormat::FMT_R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
			case TextureFormat::FMT_R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;
			case TextureFormat::FMT_D16: return DXGI_FORMAT_R16_TYPELESS;
			case TextureFormat::FMT_D24: return DXGI_FORMAT_R24G8_TYPELESS;
		}

		return 0;
	}

	TextureDX11::TextureDX11(int w, int h, TextureFormat f, int l, bool is_rt, TextureType tp) : Texture(w, h, f, l, tp)
	{
		if (lods == 0)
		{
			lods = GetLevels(width, height, 1);
		}

		CreateTexture();
	}

	void TextureDX11::CreateTexture()
	{
		DXGI_FORMAT fmt = (DXGI_FORMAT)GetFormat(format);

		D3D11_TEXTURE2D_DESC desc;

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = lods;
		desc.ArraySize = 1;
		desc.Format = fmt;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.Usage = D3D11_USAGE_DEFAULT;

		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN;
		
		if (fmt == DXGI_FORMAT_R16_TYPELESS)
		{
			depthFormat = DXGI_FORMAT_R16_UNORM;
		}
		else
		if (fmt == DXGI_FORMAT_R24G8_TYPELESS)
		{
			depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

		if (depthFormat != DXGI_FORMAT_UNKNOWN)
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.Usage = D3D11_USAGE_DEFAULT;

			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
		}

		DeviceDX11::instance->pd3dDevice->CreateTexture2D(&desc, nullptr, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		srvDesc.Format = fmt;

		if (depthFormat != DXGI_FORMAT_UNKNOWN)
		{
			srvDesc.Format = DXGI_FORMAT_R16_UNORM;
		}

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = lods;

		sampler = nullptr;
		sampler_need_recrete = true;

		DeviceDX11::instance->pd3dDevice->CreateShaderResourceView(texture, &srvDesc, &srview);

		if (depthFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = depthFormat;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			DeviceDX11::instance->pd3dDevice->CreateDepthStencilView(texture, &descDSV, &depth);
		}
	}

	void TextureDX11::Resize(int setWidth, int setHeight)
	{
		RELEASE(rt);
		RELEASE(depth);

		RELEASE(srview)
		RELEASE(texture)

		width = setWidth;
		height = setHeight;

		CreateTexture();
	}

	void TextureDX11::SetFilters(TextureFilter magmin, TextureFilter mipmap)
	{
		Texture::SetFilters(magmin, mipmap);

		sampler_need_recrete = true;
	}

	void TextureDX11::SetAdress(TextureAddress adress)
	{
		Texture::SetAdress(adress);

		sampler_need_recrete = true;
	}

	void TextureDX11::SetAdressU(TextureAddress adress)
	{
		Texture::SetAdressU(adress);

		sampler_need_recrete = true;
	}


	void TextureDX11::SetAdressV(TextureAddress adress)
	{
		Texture::SetAdressV(adress);

		sampler_need_recrete = true;
	}


	void TextureDX11::SetAdressW(TextureAddress adress)
	{
		Texture::SetAdressW(adress);

		sampler_need_recrete = true;
	}

	void TextureDX11::GenerateMips()
	{
		DeviceDX11::instance->immediateContext->GenerateMips(srview);
	}

	void TextureDX11::Apply(int slot)
	{
		if (sampler_need_recrete)
		{
			RELEASE(sampler)

			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory( &sampDesc, sizeof(sampDesc) );

			D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

			if (magminf == TextureFilter::Point && mipmapf == TextureFilter::Point)
			{
				filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			}
			else
			if (magminf == TextureFilter::Point && mipmapf == TextureFilter::Linear)
			{
				filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			}
			else
			if (magminf == TextureFilter::Linear && mipmapf == TextureFilter::Point)
			{
				filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}
			else
			if (magminf == TextureFilter::Linear && mipmapf == TextureFilter::Linear)
			{
				filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}

			sampDesc.Filter = filter;
			sampDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)((int)adressU + 1);
			sampDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)((int)adressV + 1);
			sampDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)((int)adressW + 1);
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			memcpy(sampDesc.BorderColor, borderColor, sizeof(float) * 4);

			DeviceDX11::instance->pd3dDevice->CreateSamplerState( &sampDesc, &sampler );

			sampler_need_recrete = false;
		}

		DeviceDX11::instance->immediateContext->PSSetSamplers( slot, 1, &sampler );
	}

	void TextureDX11::Update(int level, int layer, uint8_t* data, int stride)
	{
		UINT index = D3D11CalcSubresource(level, layer, 1);
		DeviceDX11::instance->immediateContext->UpdateSubresource(texture, index, nullptr, data, stride, 0);
	}

	MappedTexture TextureDX11::Lock(int level, int layer, TextureLockFlag setLockFlag)
	{
		if (textureStaging == nullptr)
		{
			DXGI_FORMAT fmt = (DXGI_FORMAT)GetFormat(format);

			D3D11_TEXTURE2D_DESC desc;

			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = lods;
			desc.ArraySize = 1;
			desc.Format = fmt;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			desc.Usage = D3D11_USAGE_STAGING;

			desc.BindFlags = 0;

			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;

			/*DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN;

			if (fmt == DXGI_FORMAT_R16_TYPELESS)
			{
				depthFormat = DXGI_FORMAT_R16_UNORM;
			}
			else
				if (fmt == DXGI_FORMAT_R24G8_TYPELESS)
				{
					depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
				}

			if (depthFormat != DXGI_FORMAT_UNKNOWN)
			{
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
				desc.Usage = D3D11_USAGE_DEFAULT;

				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
			}*/

			DeviceDX11::instance->pd3dDevice->CreateTexture2D(&desc, nullptr, &textureStaging);
		}

		lockFlag = setLockFlag;

		lockedSubResource = D3D11CalcSubresource(level, layer, 1);

		if (lockFlag != TextureLockFlag::Write)
		{
			DeviceDX11::instance->immediateContext->CopySubresourceRegion(textureStaging, lockedSubResource, 0, 0, 0, texture, lockedSubResource, nullptr);
		}

		MappedTexture mapped;

		D3D11_MAPPED_SUBRESOURCE res;
		DeviceDX11::instance->immediateContext->Map(textureStaging, lockedSubResource, D3D11_MAP_READ_WRITE, 0, &res);

		mapped.data = (uint8_t*)res.pData;
		mapped.rowStride = res.RowPitch;
		mapped.depthStride = res.DepthPitch;

		return mapped;
	}

	void TextureDX11::Unlock()
	{
		DeviceDX11::instance->immediateContext->Unmap(textureStaging, 0);

		if (lockFlag != TextureLockFlag::Read)
		{
			DeviceDX11::instance->immediateContext->CopySubresourceRegion(texture, lockedSubResource, 0, 0, 0, textureStaging, lockedSubResource, nullptr);
		}
	}

	void* TextureDX11::GetNativeResource()
	{
		return srview;
	}

	void TextureDX11::Release()
	{
		if (root.render.textures.count(name) > 0)
		{
			root.render.textures.erase(name);
		}

		RELEASE(sampler)

		RELEASE(rt);
		RELEASE(depth);

		RELEASE(srview)
		RELEASE(texture)
		RELEASE(textureStaging)

		delete this;
	}
}