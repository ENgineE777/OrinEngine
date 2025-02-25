
#pragma once

#include "Root/Render/Device.h"

#if PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#include "glad.h"
#endif

#if PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif


#ifdef PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

namespace Orin
{
	class DeviceGL : public Device
	{
		friend class Render;
		friend class DataBufferGL;
		friend class ShaderGL;
		friend class TextureGL;
		friend class VertexDeclGL;

		class VertexDeclGL* cur_vdecl = nullptr;
		class DataBufferGL* cur_buff = nullptr;
		GLuint ibuff_type = 0;
		class DataBufferGL* cur_ibuff = nullptr;

		bool need_apply_vdecl = false;

		bool vp_was_setted = false;
		Viewport cur_viewport;
		int  cur_depth_w;
		int  cur_depth_h;
		GLuint frame_buffer = 0;
		bool need_set_rt = false;
		bool use_def_backbuffer = true;
		class TextureGL* cur_rt[6];
		class TextureGL* cur_depth = nullptr;

		DeviceGL();
		virtual bool Init(void* data);
		virtual void PrepareRenderTechnique(RenderTechnique* program);
		virtual void Release();

		virtual Shader* CreateShader(ShaderType type, const char* name);
		int GetBlendArg(BlendArg arg);
		int GetBlendOp(BlendOp op);
		int GetCompareFunc(CompareFunc func);

		void UpdateStates();

	public:

		static DeviceGL* instance;

		virtual void  SetVideoMode(int wgt, int hgt, void* data);
		virtual int   GetWidth();
		virtual int   GetHeight();
		virtual float GetAspect();

		virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue);
		virtual void Present();

		virtual void SetRenderTechnique(RenderTechnique* program);

		virtual VertexDecl* CreateVertexDecl(int count, VertexDecl::ElemDesc* elems);
		virtual void SetVertexDecl(VertexDecl* vdecl);

		virtual DataBuffer* CreateBuffer(int count, int stride);
		virtual void SetVertexBuffer(int slot, DataBuffer* buffer);
		virtual void SetIndexBuffer(DataBuffer* buffer);

		virtual Texture* CreateTexture(int w, int h, TextureFormat f, int l, bool rt, TextureType tp);

		int GetPrimitiveType(PrimitiveTopology type);
		int CalcPrimCount(PrimitiveTopology type, int primCount);
		virtual void Draw(PrimitiveTopology prim, int startVertex, int primCount);
		virtual void DrawIndexed(PrimitiveTopology prim, int startVertex, int startIndex, int primCount);

		virtual void SetAlphaBlend(bool enable);
		virtual void SetBlendFunc(BlendArg src, BlendArg dest);
		virtual void SetBlendOperation(BlendOp op);
		virtual void SetDepthTest(bool enable);
		virtual void SetDepthWriting(bool enable);
		virtual void SetDepthFunc(CompareFunc func);
		virtual void SetCulling(CullMode mode);
		virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset);

		virtual void SetViewport(const Viewport& viewport);
		virtual void GetViewport(Viewport& viewport);

		virtual void SetRenderTarget(int slot, Texture* rt);
		virtual void SetDepth(Texture* depth);
		virtual void RestoreRenderTarget();
	};
}