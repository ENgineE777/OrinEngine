
#pragma once

#include "Support/Support.h"
#include "Shader.h"
#include "DataBuffer.h"
#include "RenderTechnique.h"
#include "VertexDecl.h"
#include "Texture.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC PrimitiveTopology
	{
		LineStrip = 0,
		LinesList,
		TriangleStrip,
		TrianglesList
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC CullMode
	{
		CullNone = 0 /*!< Culling disabled */,
		CullCW /*!< Clockwise culling */,
		CullCCW /*!< Counter clockwise culling */
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC BlendOp
	{
		BlendAdd = 0,
		BlendSub,
		BlendRevSub,
		BlendMin,
		BlendMax
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC CompareFunc
	{
		CmpNever = 0,
		CmpLess,
		CmpEqual,
		CmpLessEqual,
		CmpGreater,
		CmpNotequal,
		CmpGreaterqual,
		CmpAlways
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC BlendArg
	{
		ArgZero = 0,
		ArgOne,
		ArgSrcColor,
		ArgInvSrcColor,
		ArgSrcAlpha,
		ArgInvSrcAlpha,
		ArgDestAlpha,
		ArgInvDestAlpha,
		ArgDestColor,
		ArgInvDestColor
	};

	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief Device

	This is abstrac Device which is resposable for creation of GAPI object, changing states and redering.

	*/

	class CLASS_DECLSPEC Device
	{
		friend class Render;
		friend class RenderTechnique;
		friend class DeviceDX11;
		friend class DeviceGL;

		#ifndef DOXYGEN_SKIP

		int scr_w = 0;
		int scr_h = 0;
		int cur_rt_w = 0;
		int cur_rt_h = 0;
		float cur_aspect = 0.0f;

		virtual bool Init(void* external_device) = 0;
		virtual void PrepareRenderTechnique(RenderTechnique* program) = 0;
		virtual void Release() = 0;

		RenderTechnique* currentRenderTechnique = nullptr;

		#endif

	public:

		struct Viewport
		{
			int x, y;
			int width, height;
			float minZ, maxZ;
		};

		struct Rect
		{
			int left, right;
			int top, bottom;
		};

		/**
		\brief Set backbuffer as current

		\param[in] id Width of video mode
		\param[in] wgt Width of video mode
		\param[in] hgt Height of video mode
		\param[in] data Platform specific data for set up of a video mode

		*/
		virtual bool SetBackBuffer(int id, int wgt, int hgt, void* data) = 0;

		virtual void SetFullscreenState(bool state) = 0;

		/**
		\brief Get current back buffer

		\return Current back buffer
		*/
		virtual void* GetBackBuffer() { return nullptr; };

		/**
		\brief Get width of a current render target

		\return Width of a current render target
		*/
		virtual int GetWidth() = 0;

		/**
		\brief Get height of a current render target

		\return Height of a current render target
		*/
		virtual int GetHeight() = 0;

		/**
		\brief Get aspect ration of a current render target

		\return Aspect ration of a current render target
		*/
		virtual float GetAspect() = 0;

		/**
		\brief Clears current render target

		\param[in] renderTarget Should render target be cleared
		\param[in] color Color of fiil of render target
		\param[in] zbuffer Should z buffer be cleared
		\param[in] zValue Z value of fill z depth buffer
		*/
		virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue) = 0;

		/**
		\brief Present result of a render into main back buffer
		*/
		virtual void Present() = 0;

		/**
		\brief Set program as current

		\param[in] program Pointer to a Program
		*/
		virtual void SetRenderTechnique(RenderTechnique* renderTechnique) = 0;

		/**
		\brief Create vertex declaration from an array of element descriptors

		\param[in] count Count of elemnts in array
		\param[in] elems Array of element descriptors
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		*/
		virtual VertexDeclRef CreateVertexDecl(int count, VertexDecl::ElemDesc* elems, const char* file, int line) = 0;

		/**
		\brief Set vertex declaration as current

		\param[in] vdecl Pointer to a VertexDecl
		*/
		virtual void SetVertexDecl(VertexDecl* vdecl) = 0;

		/**
		\brief Create a buffer

		\param[in] count Size in bytes of a buffer
		\param[in] stride Stride of a buffer.
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to a buffer
		*/
		virtual DataBufferRef CreateBuffer(int count, int stride, const char* file, int line) = 0;

		/**
		\brief Bind a buffer in a slot

		\param[in] slot Number of a slot
		\param[in] buffer Pointer to a DataBuffer

		*/
		virtual void SetVertexBuffer(int slot, DataBuffer* buffer) = 0;

		/**
		\brief Bind a index buffer=

		\param[in] buffer Pointer to a DataBuffer

		*/
		virtual void SetIndexBuffer(DataBuffer* buffer) = 0;

		/**
		\brief Create a texture

		\param[in] w Width of a texture
		\param[in] h Height of a texture
		\param[in] f Format of a texture
		\param[in] l Number of lods of a texture
		\param[in] rt Shhoul be used texture a srender target
		\param[in] tp Type of a texture
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		*/
		virtual TextureRef CreateTexture(int w, int h, TextureFormat f, int l, bool rt, TextureType tp, const char* file, int line) = 0;

		/**
		\brief Draw indexed primitives

		\param[in] prim Primitives type
		\param[in] startVertex Index of start vertex
		\param[in] primCount Count of primitives

		*/
		virtual void Draw(PrimitiveTopology prim, int startVertex, int primCount) = 0;

		/**
		\brief Draw indexed primitives

		\param[in] prim Primitives type
		\param[in] startVertex Index of start vertex
		\param[in] startIndex Index of start index
		\param[in] primCount Count of primitives

		*/
		virtual void DrawIndexed(PrimitiveTopology prim, int startVertex, int startIndex, int primCount) = 0;

		/**
		\brief Controls alpha blending

		\param[in] enable Should alpha blending be enabled
		*/
		virtual void SetAlphaBlend(bool enable) = 0;

		/**
		\brief Set blend functions

		\param[in] src Blend function for source
		\param[in] dest Blend function for destination
		*/
		virtual void SetBlendFunc(BlendArg src, BlendArg dest) = 0;

		/**
		\brief Set blend operation

		\param[in] op Type of blend operation
		*/
		virtual void SetBlendOperation(BlendOp op) = 0;

		/**
		\brief Controls depth test

		\param[in] enable Should depth testing be enabled
		*/
		virtual void SetDepthTest(bool enable) = 0;

		/**
		\brief Controls depth writing

		\param[in] enable Should depth writing be enabled
		*/
		virtual void SetDepthWriting(bool enable) = 0;

		/**
		\brief Set depth function

		\param[in] func Depth function
		*/
		virtual void SetDepthFunc(CompareFunc func) = 0;

		/**
		\brief Set culling

		\param[in] mode Culling mode
		*/
		virtual void SetCulling(CullMode mode) = 0;

		/**
		\brief Set up slope ZBias

		\param[in] enable Controls usage of slope ZBias
		\param[in] slopeZBias Value of ZBias
		\param[in] depthOffset Depth offset

		*/
		virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset) = 0;

		/**
		\brief Set viewport as current

		\param[in] viewport
		*/
		virtual void SetViewport(const Viewport& viewport) = 0;

		/**
		\brief Get current viewport

		\param[out] viewport
		*/
		virtual void GetViewport(Viewport& viewport) = 0;


		/**
		\brief Controls scisors

		\param[in] enable Should alpha blending be enabled
		*/
		virtual void SetScissors(bool enable) = 0;

		/**
		\brief Set scissor rect

		\param[in] rect Rect of scissors
		*/
		virtual void SetScissorRect(Rect rect) = 0;

		/**
		\brief Bind a texture as a render target in a slot

		\param[in] slot Number of a slot
		\param[in] rt Pointer to a Texture
		*/
		virtual void SetRenderTarget(int slot, Texture* rt) = 0;

		/**
		\brief Set texture as a Z-buffer

		\param[in] depth Pointer to a Texture
		*/
		virtual void SetDepth(Texture* depth) = 0;

		/**
		\brief Restore render target to initinal back buffer
		*/
		virtual void RestoreRenderTarget() = 0;

	protected:

		#ifndef DOXYGEN_SKIP

		virtual Shader* CreateShader(ShaderType type, const char* name) = 0;
		virtual Texture* CreateTextureInner(int w, int h, TextureFormat f, int l, bool rt, TextureType tp, const char* file, int line) = 0;

		#endif
	};
}