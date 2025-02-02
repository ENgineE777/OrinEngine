
#include "DeviceDX11.h"
#include "DataBufferDX11.h"
#include "ShaderDX11.h"
#include "VertexDeclDX11.h"
#include "TextureDX11.h"
#include "Root/Root.h"

namespace Orin
{
	DeviceDX11* DeviceDX11::instance = nullptr;

	DeviceDX11::DeviceDX11()
	{
		instance = this;

		blend_desc = NEW D3D11_BLEND_DESC();
		ZeroMemory(blend_desc, sizeof(D3D11_BLEND_DESC));
		blend_state = nullptr;
		blend_changed = true;

		ds_desc = NEW D3D11_DEPTH_STENCIL_DESC();
		ZeroMemory(ds_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		ds_state = nullptr;
		ds_changed = true;

		raster_desc = NEW D3D11_RASTERIZER_DESC();
		ZeroMemory(raster_desc, sizeof(D3D11_RASTERIZER_DESC));
		raster_state = nullptr;
		raster_changed = true;

		blend_desc->RenderTarget[0].BlendEnable = false;

		blend_desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc->RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		ds_desc->DepthEnable = true;
		ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		ds_desc->DepthFunc = D3D11_COMPARISON_LESS;

		ds_desc->StencilEnable = false;
		ds_desc->StencilReadMask = 0xFF;
		ds_desc->StencilWriteMask = 0xFF;

		ds_desc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		ds_desc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		ds_desc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds_desc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		ds_desc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		ds_desc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		ds_desc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds_desc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		raster_desc->AntialiasedLineEnable = false;
		raster_desc->CullMode = D3D11_CULL_BACK;
		raster_desc->DepthBias = 0;
		raster_desc->DepthBiasClamp = 0.0f;
		raster_desc->DepthClipEnable = false;

		raster_desc->FillMode = D3D11_FILL_SOLID;
		raster_desc->FrontCounterClockwise = false;
		raster_desc->MultisampleEnable = false;
		raster_desc->ScissorEnable = false;
		raster_desc->SlopeScaledDepthBias = 0.0f;

		for (int i = 0; i < 6; i++)
		{
			cur_rt[i] = nullptr;
		}

		cur_depth = nullptr;
		need_set_rt = true;

		vp_was_setted = false;
	}

	bool DeviceDX11::Init(void* external_device)
	{
		if (external_device)
		{
			pd3dDevice = (ID3D11Device*)external_device;
			pd3dDevice->GetImmediateContext(&immediateContext);
		}
		else
		{
			UINT createDeviceFlags = 0;
	#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

			D3D_DRIVER_TYPE driverTypes[] =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
			};
			UINT numDriverTypes = ARRAYSIZE(driverTypes);

			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
			};
			UINT numFeatureLevels = ARRAYSIZE(featureLevels);

			D3D_DRIVER_TYPE   driverType;
			D3D_FEATURE_LEVEL featureLevel;
			HRESULT hr;

			for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
			{
				driverType = driverTypes[driverTypeIndex];
				hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
					D3D11_SDK_VERSION, &pd3dDevice, &featureLevel, &immediateContext);

				if (SUCCEEDED(hr))
				{
					break;
				}
			}

			if (pd3dDevice == nullptr)
			{
				OAK_ALERT("Can't create any DX11Device");
				return false;
			}

			hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));

			if (hr < 0)
			{
				OAK_ALERT("CreateDXGIFactory1 failed with error");
				return false;
			}
		}

		return true;
	}

	bool DeviceDX11::SetBackBuffer(int id, int wgt, int hgt, void* data)
	{
		HWND handle = *((HWND*)data);
		HRESULT hr;

		WindowBackBufferHolder* backbuffer_holder = nullptr;

		for (auto& holder : backbuffer_holders)
		{
			if (holder.id == id)
			{
				backbuffer_holder = &holder;
			}
		}

		if (!backbuffer_holder)
		{
			backbuffer_holders.emplace_back();
			backbuffer_holder = &backbuffer_holders.back();
			backbuffer_holder->handle = handle;
			backbuffer_holder->id = id;
		}

		if (backbuffer_holder->scr_w != wgt ||
			backbuffer_holder->scr_h != hgt)
		{
			root.Log("Render", "Set videomode : %i x %i", wgt, hgt);

			backbuffer_holder->scr_w = wgt;
			backbuffer_holder->scr_h = hgt;
			backbuffer_holder->cur_aspect = (float)hgt / (float)wgt;

			RELEASE(backbuffer_holder->depthStencilView)
			RELEASE(backbuffer_holder->depthStencil)
			RELEASE(backbuffer_holder->renderTargetShaderView)
			RELEASE(backbuffer_holder->renderTargetView)
			RELEASE(backbuffer_holder->renderTarget)

			if (factory)
			{
				RELEASE(backbuffer_holder->swapChain)

				DXGI_SWAP_CHAIN_DESC sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.BufferCount = 2;
				sd.BufferDesc.Width = wgt;
				sd.BufferDesc.Height = hgt;
				sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				sd.BufferDesc.RefreshRate.Numerator = 60;
				sd.BufferDesc.RefreshRate.Denominator = 1;
				sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				sd.OutputWindow = backbuffer_holder->handle;
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
				sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
				sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				sd.Windowed = TRUE;

				hr = factory->CreateSwapChain(pd3dDevice, &sd, &backbuffer_holder->swapChain);

				if (hr < 0)
				{
					OAK_ALERT("factory->CreateSwapChain failed with error");
					return false;
				}

				ID3D11Texture2D* pBackBuffer = nullptr;
				hr = backbuffer_holder->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

				if (hr < 0)
				{
					OAK_ALERT("backbuffer_holder->swapChain->GetBuffer failed with error");
					return false;
				}

				hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer_holder->renderTargetView);

				if (hr < 0)
				{
					OAK_ALERT("pd3dDevice->CreateRenderTargetView failed with error");
					return false;
				}

				pBackBuffer->Release();

				swapChain = backbuffer_holder->swapChain;
			}
			else
			{
				D3D11_TEXTURE2D_DESC descRT;
				ZeroMemory(&descRT, sizeof(descRT));
				descRT.Width = wgt;
				descRT.Height = hgt;
				descRT.MipLevels = 1;
				descRT.ArraySize = 1;
				descRT.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				descRT.SampleDesc.Count = 1;
				descRT.SampleDesc.Quality = 0;
				descRT.Usage = D3D11_USAGE_DEFAULT;
				descRT.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				descRT.CPUAccessFlags = 0;
				descRT.MiscFlags = 0;

				hr = pd3dDevice->CreateTexture2D(&descRT, nullptr, &backbuffer_holder->renderTarget);
				hr = pd3dDevice->CreateRenderTargetView(backbuffer_holder->renderTarget, nullptr, &backbuffer_holder->renderTargetView);

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				DeviceDX11::instance->pd3dDevice->CreateShaderResourceView(backbuffer_holder->renderTarget, &srvDesc, &backbuffer_holder->renderTargetShaderView);
			}

			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = wgt;
			descDepth.Height = hgt;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;
			hr = pd3dDevice->CreateTexture2D(&descDepth, nullptr, &backbuffer_holder->depthStencil);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			hr = pd3dDevice->CreateDepthStencilView(backbuffer_holder->depthStencil, &descDSV, &backbuffer_holder->depthStencilView);
		}

		scr_w = backbuffer_holder->scr_w;
		scr_h = backbuffer_holder->scr_h;
		cur_aspect = backbuffer_holder->cur_aspect;

		renderTargetView = backbuffer_holder->renderTargetView;
		depthStencilView = backbuffer_holder->depthStencilView;
		renderTargetShaderView = backbuffer_holder->renderTargetShaderView;

		RestoreRenderTarget();

		return true;
	}

	void* DeviceDX11::GetBackBuffer()
	{
		return renderTargetShaderView;
	}

	void DeviceDX11::SetFullscreenState(bool state)
	{
		if (swapChain)
		{
			HRESULT hr = swapChain->SetFullscreenState(true, nullptr);
			root.Log("sdfs", "SetFullscreenState %i", hr);
		}
	}

	int DeviceDX11::GetWidth()
	{
		return scr_w;
	}

	int DeviceDX11::GetHeight()
	{
		return scr_h;
	}

	float DeviceDX11::GetAspect()
	{
		return cur_aspect;
	}

	void DeviceDX11::Clear(bool renderTarget, Color color, bool zbuffer, float zValue)
	{
		if (renderTarget)
		{
			for (int i = 0; i < 6; i++)
			{
				if (cur_rt[i])
				{
					immediateContext->ClearRenderTargetView(cur_rt[i], (float*)&color.r);
				}
			}
		}

		if (zbuffer && cur_depth)
		{
			immediateContext->ClearDepthStencilView(cur_depth, D3D11_CLEAR_DEPTH, zValue, 0);
		}
	}

	void DeviceDX11::Present()
	{
		if (swapChain)
		{
			swapChain->Present(1, 0);
			need_set_rt = true;
		}
		else
		{
			need_set_rt = true;
			need_apply_vdecl = true;
			need_apply_prog = true;
			blend_changed = true;
			ds_changed = true;
			raster_changed = true;
		}
	}

	void DeviceDX11::PrepareRenderTechnique(RenderTechnique* program)
	{

	}

	void DeviceDX11::SetRenderTechnique(RenderTechnique* program)
	{
		if (currentRenderTechnique != program)
		{
			currentRenderTechnique = program;
			need_apply_prog = true;
			need_apply_vdecl = true;
		}
	}

	VertexDeclRef DeviceDX11::CreateVertexDecl(int count, VertexDecl::ElemDesc* elems, const char* file, int line)
	{
		return VertexDeclRef(new(file, line) VertexDeclDX11(count, elems), file, line);
	}

	void DeviceDX11::SetVertexDecl(VertexDecl* vdecl)
	{
		if (cur_vdecl != vdecl)
		{
			need_apply_vdecl = true;
			cur_vdecl = (VertexDeclDX11*)vdecl;
		}
	}

	DataBufferRef DeviceDX11::CreateBuffer(int count, int stride, const char* file, int line)
	{
		return DataBufferRef(new(file, line) DataBufferDX11(count, stride), file, line);
	}

	void DeviceDX11::SetVertexBuffer(int slot, DataBuffer* buffer)
	{
		ID3D11Buffer* vb = nullptr;
		unsigned int stride = 0;

		if (buffer)
		{
			vb = ((DataBufferDX11*)buffer)->buffer;
			stride = buffer->GetStride();
		}

		unsigned int offset = 0;
		immediateContext->IASetVertexBuffers(slot, 1, &vb, &stride, &offset);
	}

	void DeviceDX11::SetIndexBuffer(DataBuffer* buffer)
	{
		ID3D11Buffer* ib = nullptr;
		DXGI_FORMAT fmt = DXGI_FORMAT_R16_UINT;

		if (buffer)
		{
			ib = ((DataBufferDX11*)buffer)->buffer;
			if (buffer->GetStride() == 4)
			{
				fmt = DXGI_FORMAT_R32_UINT;
			}
		}

		immediateContext->IASetIndexBuffer(ib, fmt, 0);
	}

	Shader* DeviceDX11::CreateShader(ShaderType type, const char* name)
	{
		return NEW ShaderDX11(type, name);
	}

	Texture* DeviceDX11::CreateTextureInner(int w, int h, TextureFormat f, int l, bool rt, TextureType tp, const char* file, int line)
	{
		return new(file, line) TextureDX11(w, h, f, l, rt, tp);
	}

	TextureRef DeviceDX11::CreateTexture(int w, int h, TextureFormat f, int l, bool rt, TextureType tp, const char* file, int line)
	{
		return TextureRef(CreateTextureInner(w, h, f, l, rt, tp, file, line), file, line);
	}

	int DeviceDX11::GetPrimitiveType(PrimitiveTopology type)
	{
		int tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		switch (type)
		{
			case PrimitiveTopology::LineStrip:
			{
				tp = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			}
			break;
			case PrimitiveTopology::LinesList:
			{
				tp = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			}
			break;
			case PrimitiveTopology::TriangleStrip:
			{
				tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}
			break;
			case PrimitiveTopology::TrianglesList:
			{
				tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			}
			break;
		}

		return tp;
	}

	int DeviceDX11::CalcPrimCount(PrimitiveTopology type, int primCount)
	{
		int tp = primCount;

		switch (type)
		{
			case PrimitiveTopology::LineStrip:
			{
				tp = primCount + 1;
			}
			break;
			case PrimitiveTopology::LinesList:
			{
				tp = primCount * 2;
			}
			break;
			case PrimitiveTopology::TriangleStrip:
			{
				tp = primCount + 2;
			}
			break;
			case PrimitiveTopology::TrianglesList:
			{
				tp = primCount * 3;
			}
			break;
		}

		return tp;
	}

	void DeviceDX11::Draw(PrimitiveTopology prim, int startVertex, int primCount)
	{
		UpdateStates();

		immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(prim));
		immediateContext->Draw(CalcPrimCount(prim, primCount), startVertex);
	}

	void DeviceDX11::DrawIndexed(PrimitiveTopology prim, int startVertex, int startIndex, int primCount)
	{
		UpdateStates();

		immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(prim));
		immediateContext->DrawIndexed(CalcPrimCount(prim, primCount), startIndex, startVertex);
	}


	void DeviceDX11::SetAlphaBlend(bool enable)
	{
		blend_desc->RenderTarget[0].BlendEnable = enable;

		blend_changed = true;
	}

	void DeviceDX11::SetBlendFunc(BlendArg src, BlendArg dest)
	{
		blend_desc->RenderTarget[0].SrcBlend = (D3D11_BLEND)((int)src + 1);
		blend_desc->RenderTarget[0].DestBlend = (D3D11_BLEND)((int)dest + 1);

		blend_changed = true;
	}

	void DeviceDX11::SetBlendOperation(BlendOp op)
	{
		blend_desc->RenderTarget[0].BlendOp = (D3D11_BLEND_OP)((int)op + 1);

		blend_changed = true;
	}

	void DeviceDX11::SetDepthTest(bool enable)
	{
		ds_desc->DepthEnable = enable;
		ds_changed = true;
	}

	void DeviceDX11::SetDepthWriting(bool enable)
	{
		if (enable)
		{
			ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		}

		ds_changed = true;
	}

	void DeviceDX11::SetDepthFunc(CompareFunc func)
	{
		ds_desc->DepthFunc = (D3D11_COMPARISON_FUNC)((int)func + 1);
		ds_changed = true;
	}

	void DeviceDX11::SetCulling(CullMode mode)
	{
		raster_desc->CullMode = (D3D11_CULL_MODE)((int)mode + 1);
		raster_changed = true;
	}

	void DeviceDX11::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
	{
		float curDepthBias = 0.0f;
		float curBiasSlope = 0.0f;

		if (enable)
		{
			curDepthBias = depthOffset;
			curBiasSlope = slopeZBias;
		}

		raster_desc->DepthBias = (int)(curDepthBias / (1.0f / pow(2, 23)));
		raster_desc->SlopeScaledDepthBias = curBiasSlope;

		raster_changed = true;
	}

	void DeviceDX11::UpdateStates()
	{
		if (need_set_rt)
		{
			ID3D11ShaderResourceView* emties[] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
			DeviceDX11::instance->immediateContext->PSSetShaderResources(0, 6, emties);

			immediateContext->OMSetRenderTargets(6, cur_rt, cur_depth);
			need_set_rt = false;

			D3D11_VIEWPORT vp;

			if (cur_rt[0])
			{
				vp.Width = (FLOAT)cur_rt_w;
				vp.Height = (FLOAT)cur_rt_h;
			}
			else
			{
				vp.Width = (FLOAT)cur_depth_w;
				vp.Height = (FLOAT)cur_depth_h;
			}

			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			immediateContext->RSSetViewports(1, &vp);
		}

		if (currentRenderTechnique)
		{
			if (need_apply_prog)
			{
				if (currentRenderTechnique->vshader) currentRenderTechnique->vshader->Apply();
				if (currentRenderTechnique->pshader) currentRenderTechnique->pshader->Apply();

				SetAlphaBlend(false);
				SetDepthTest(true);
				SetDepthWriting(true);
				SetBlendFunc(BlendArg::ArgSrcAlpha, BlendArg::ArgInvSrcAlpha);
				SetCulling(CullMode::CullCCW);

				currentRenderTechnique->ApplyStates();
				need_apply_prog = false;
			}

			if (currentRenderTechnique->vshader)
			{
				currentRenderTechnique->vshader->UpdateConstants();

				if (need_apply_vdecl)
				{
					if (cur_vdecl)
					{
						cur_vdecl->Apply((ShaderDX11*)currentRenderTechnique->vshader);
					}

					need_apply_vdecl = false;
				}
			}

			if (currentRenderTechnique->pshader) currentRenderTechnique->pshader->UpdateConstants();
		}

		if (blend_changed)
		{
			RELEASE(blend_state)

			pd3dDevice->CreateBlendState(blend_desc, &blend_state);
			immediateContext->OMSetBlendState(blend_state, 0, 0xffffffff);

			blend_changed = false;
		}

		if (ds_changed)
		{
			RELEASE(ds_state)

			instance->pd3dDevice->CreateDepthStencilState(ds_desc, &ds_state);
			immediateContext->OMSetDepthStencilState(ds_state, 255);

			ds_changed = false;
		}

		if (raster_changed)
		{
			RELEASE(raster_state)

			pd3dDevice->CreateRasterizerState(raster_desc, &raster_state);
			immediateContext->RSSetState(raster_state);

			raster_changed = false;
		}
	}

	void DeviceDX11::SetScissors(bool enable)
	{
		raster_desc->ScissorEnable = enable;
		raster_changed = true;
	}

	void DeviceDX11::SetScissorRect(Rect rect)
	{
		RECT DX11Rect;

		DX11Rect.left = rect.left;
		DX11Rect.top = rect.top;
		DX11Rect.right = rect.right;
		DX11Rect.bottom = rect.bottom;

		immediateContext->RSSetScissorRects(1, &DX11Rect);
	}

	void DeviceDX11::SetViewport(const Viewport& viewport)
	{
		D3D11_VIEWPORT vp;
		vp.Width = (float)viewport.width;
		vp.Height = (float)viewport.height;
		vp.MinDepth = viewport.minZ;
		vp.MaxDepth = viewport.maxZ;
		vp.TopLeftX = (float)viewport.x;
		vp.TopLeftY = (float)viewport.y;

		immediateContext->RSSetViewports(1, &vp);

		vp_was_setted = true;
	}

	void DeviceDX11::GetViewport(Viewport& viewport)
	{
		if (vp_was_setted)
		{
			D3D11_VIEWPORT d3dviewport;
			unsigned int num = 1;
			immediateContext->RSGetViewports(&num, &d3dviewport);

			viewport.x = (int)d3dviewport.TopLeftX;
			viewport.y = (int)d3dviewport.TopLeftY;
			viewport.width = (int)d3dviewport.Width;
			viewport.height = (int)d3dviewport.Height;
			viewport.minZ = d3dviewport.MinDepth;
			viewport.maxZ = d3dviewport.MaxDepth;
		}
		else
		{
			viewport.x = 0;
			viewport.y = 0; 

			if (cur_rt[0])
			{
				viewport.width = (short)cur_rt_w;
				viewport.height = (short)cur_rt_h;
			}
			else
			{
				viewport.width = (short)cur_depth_w;
				viewport.height = (short)cur_depth_h;
			}

			viewport.minZ = 0.0f;
			viewport.maxZ = 1.0f;
		}
	}

	void DeviceDX11::SetRenderTarget(int slot, Texture* rt)
	{
		vp_was_setted = false;

		if (rt)
		{
			TextureDX11* rt_dx11 = (TextureDX11*)rt;

			if (!rt_dx11->rt)
			{
				pd3dDevice->CreateRenderTargetView(rt_dx11->texture, nullptr, &rt_dx11->rt);
			}

			if (slot == 0)
			{
				cur_rt_w = rt_dx11->GetWidth();
				cur_rt_h = rt_dx11->GetHeight();
			}

			cur_rt[slot] = rt_dx11->rt;
		}
		else
		{
			if (slot == 0)
			{
				cur_rt_w = -1;
				cur_rt_h = -1;
			}

			cur_rt[slot] = nullptr;
		}

		need_set_rt = true;
	}

	void DeviceDX11::SetDepth(Texture* depth)
	{
		vp_was_setted = false;

		if (depth)
		{
			TextureDX11* depth_dx11 = (TextureDX11*)depth;

			cur_depth_w = depth_dx11->GetWidth();
			cur_depth_h = depth_dx11->GetHeight();

			cur_depth = depth_dx11->depth;
		}
		else
		{
			cur_depth_w = -1;
			cur_depth_h = -1;
			cur_depth = nullptr;
		}

		need_set_rt = true;
	}

	void DeviceDX11::RestoreRenderTarget()
	{
		vp_was_setted = false;
		cur_rt_w = scr_w;
		cur_rt_h = scr_h;
	
		for (int i = 0; i < 6; i++)
		{
			cur_rt[i] = nullptr;
		}

		cur_rt[0] = renderTargetView;
		cur_depth = depthStencilView;
		need_set_rt = true;
	}

	void DeviceDX11::Release()
	{
		delete blend_desc;
		delete ds_desc;
		delete raster_desc;

		delete this;
	}
}