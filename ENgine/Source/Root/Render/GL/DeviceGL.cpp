
#include "DeviceGL.h"
#include "ShaderGL.h"
#include "DataBufferGL.h"
#include "VertexDeclGL.h"
#include "TextureGL.h"
#include "Root/Root.h"

namespace Orin
{
	DeviceGL* DeviceGL::instance = nullptr;

	DeviceGL::DeviceGL()
	{
		instance = this;

		for (int i = 0; i < 6; i++)
		{
			cur_rt[i] = nullptr;
		}
	}

	bool DeviceGL::Init(void* data)
	{
		root.Log("Render", "Version %i", GL_VERSION);
		root.Log("Render", "Vendor %i", GL_VENDOR);
		root.Log("Render", "Renderer %i", GL_RENDERER);
		root.Log("Render", "Extensions %i", GL_EXTENSIONS);

		glGenFramebuffers(1, &frame_buffer);

		return true;
	}

	void DeviceGL::SetVideoMode(int wgt, int hgt, void* data)
	{
		if (scr_w != wgt && scr_h != hgt)
		{
			root.Log("Render", "Set videomode : %i x %i", wgt, hgt);
		}

		scr_w = wgt;
		scr_h = hgt;

		cur_aspect = (float)hgt / (float)wgt;

		glViewport(0, 0, scr_w, scr_h);
	}

	int DeviceGL::GetWidth()
	{
		return scr_w;
	}

	int DeviceGL::GetHeight()
	{
		return scr_h;
	}

	float DeviceGL::GetAspect()
	{
		return cur_aspect;
	}

	void DeviceGL::Clear(bool renderTarget, Color color, bool zbuffer, float zValue)
	{
		UpdateStates();

		glClearColor(color.r, color.g, color.b, color.a);
		glClearDepthf(zbuffer);

		glClear((renderTarget ? GL_COLOR_BUFFER_BIT : 0) | (zbuffer ? GL_DEPTH_BUFFER_BIT : 0));
	}

	void DeviceGL::Present()
	{
	}

	void DeviceGL::PrepareRenderTechnique(RenderTechnique* program)
	{
		program->program = glCreateProgram();

		if (program->vshader)
		{
			((ShaderGL*)program->vshader)->program = program->program;
			glAttachShader(program->program, ((ShaderGL*)program->vshader)->shader);
		}

		if (program->pshader)
		{
			((ShaderGL*)program->pshader)->program = program->program;
			glAttachShader(program->program, ((ShaderGL*)program->pshader)->shader);
		}

		glLinkProgram(program->program);

		GLint linkSuccess;
		glGetProgramiv(program->program, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE)
		{
			GLchar messages[256];
			glGetProgramInfoLog(program->program, sizeof(messages), 0, &messages[0]);
			root.Log("Render", "Error in linking program: %s", messages);
		}
	}

	void DeviceGL::SetRenderTechnique(RenderTechnique* program)
	{
		if (currentRenderTechnique != program)
		{
			currentRenderTechnique = program;

			SetAlphaBlend(false);
			SetDepthTest(true);
			SetDepthWriting(true);

			if (currentRenderTechnique)
			{
				glUseProgram(currentRenderTechnique->program);
				currentRenderTechnique->ApplyStates();
			}
		}
	}

	VertexDecl* DeviceGL::CreateVertexDecl(int count, VertexDecl::ElemDesc* elems)
	{
		return new VertexDeclGL(count, elems);
	}

	void DeviceGL::SetVertexDecl(VertexDecl* vdecl)
	{
		if (cur_vdecl != vdecl)
		{
			cur_vdecl = (VertexDeclGL*)vdecl;

			if (cur_vdecl)
			{
				need_apply_vdecl = true;
			}
		}

	}

	DataBuffer* DeviceGL::CreateBuffer(int count, int stride)
	{
		return new DataBufferGL(count, stride);
	}

	void DeviceGL::SetVertexBuffer(int slot, DataBuffer* buffer)
	{
		if (cur_buff == buffer)
		{
			return;
		}

		cur_buff = (DataBufferGL*)buffer;

		if (cur_buff)
		{
			if (cur_vdecl)
			{
				need_apply_vdecl = true;
			}

			glBindBuffer(GL_ARRAY_BUFFER, cur_buff->buffer);
		}
	}

	void DeviceGL::SetIndexBuffer(DataBuffer* buffer)
	{
		if (cur_ibuff == buffer)
		{
			return;
		}

		cur_ibuff = (DataBufferGL*)buffer;

		if (cur_ibuff)
		{
			ibuff_type = (buffer->GetStride() == 4) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cur_ibuff->buffer);
		}
	}

	Shader* DeviceGL::CreateShader(ShaderType type, const char* name)
	{
		return new ShaderGL(type, name);
	}

	Texture* DeviceGL::CreateTexture(int w, int h, TextureFormat f, int l, bool rt, TextureType tp)
	{
		return new TextureGL(w, h, f, l, rt, tp);
	}

	int DeviceGL::GetPrimitiveType(PrimitiveTopology type)
	{
		int tp = GL_TRIANGLES;
		switch (type)
		{
		case PrimitiveTopology::LineStrip:
		{
			tp = GL_LINE_STRIP;
		}
		break;
		case PrimitiveTopology::LinesList:
		{
			tp = GL_LINES;
		}
		break;
		case PrimitiveTopology::TriangleStrip:
		{
			tp = GL_TRIANGLE_STRIP;
		}
		break;
		case PrimitiveTopology::TrianglesList:
		{
			tp = GL_TRIANGLES;
		}
		break;
		}

		return tp;
	}

	int DeviceGL::CalcPrimCount(PrimitiveTopology type, int primCount)
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

	void DeviceGL::Draw(PrimitiveTopology prim, int startVertex, int primCount)
	{
		UpdateStates();

		glDrawArrays(GetPrimitiveType(prim), startVertex, CalcPrimCount(prim, primCount));
	}

	void DeviceGL::DrawIndexed(PrimitiveTopology prim, int startVertex, int startIndex, int primCount)
	{
		if (!cur_ibuff)
		{
			return;
		}

		UpdateStates();

		glDrawElements(GetPrimitiveType(prim), CalcPrimCount(prim, primCount), ibuff_type, nullptr);
	}

	void DeviceGL::UpdateStates()
	{
		if (need_set_rt)
		{
			if (use_def_backbuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, scr_w, scr_h);
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

				for (int i = 0; i < 1; i++)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, cur_rt[i] ? cur_rt[i]->texture : 0, 0);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cur_depth ? cur_depth->texture : 0, 0);

				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					root.Log("Render", "glBindFramebuffer error - %i", status);
				}

				glViewport(0, 0, cur_rt_w, cur_rt_h);
			}

			need_set_rt = false;
		}

		if (need_apply_vdecl)
		{
			cur_vdecl->Apply();
			need_apply_vdecl = false;
		}
	}

	void DeviceGL::SetAlphaBlend(bool enable)
	{
		if (enable)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	int DeviceGL::GetBlendArg(BlendArg arg)
	{
		int arg_op = 0;

		switch (arg)
		{
		case BlendArg::ArgZero:
			arg_op = GL_ZERO;
			break;
		case BlendArg::ArgOne:
			arg_op = GL_ONE;
			break;
		case BlendArg::ArgSrcColor:
			arg_op = GL_SRC_COLOR;
			break;
		case BlendArg::ArgInvSrcColor:
			arg_op = GL_ONE_MINUS_SRC_COLOR;
			break;
		case BlendArg::ArgSrcAlpha:
			arg_op = GL_SRC_ALPHA;
			break;
		case BlendArg::ArgInvSrcAlpha:
			arg_op = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case BlendArg::ArgDestAlpha:
			arg_op = GL_DST_ALPHA;
			break;
		case BlendArg::ArgInvDestAlpha:
			arg_op = GL_ONE_MINUS_DST_ALPHA;
			break;
		case BlendArg::ArgDestColor:
			arg_op = GL_DST_COLOR;
			break;
		case BlendArg::ArgInvDestColor:
			arg_op = GL_ONE_MINUS_DST_COLOR;
			break;
		}

		return arg_op;
	}

	int DeviceGL::GetCompareFunc(CompareFunc func)
	{
		int cmp = 0;

		switch (func)
		{
		case CompareFunc::CmpNever:
			cmp = GL_NEVER;
			break;
		case CompareFunc::CmpLess:
			cmp = GL_LESS;
			break;
		case CompareFunc::CmpEqual:
			cmp = GL_EQUAL;
			break;
		case CompareFunc::CmpLessEqual:
			cmp = GL_LEQUAL;
			break;
		case CompareFunc::CmpGreater:
			cmp = GL_GREATER;
			break;
		case CompareFunc::CmpNotequal:
			cmp = GL_NOTEQUAL;
			break;
		case CompareFunc::CmpGreaterqual:
			cmp = GL_GEQUAL;
			break;
		case CompareFunc::CmpAlways:
			cmp = GL_ALWAYS;
			break;
		}

		return cmp;
	}

	int DeviceGL::GetBlendOp(BlendOp op)
	{
		int blend_op = 0;

		switch (op)
		{
		case BlendOp::BlendAdd:
			blend_op = GL_FUNC_ADD;
			break;
		case BlendOp::BlendSub:
			blend_op = GL_FUNC_SUBTRACT;
			break;
		case BlendOp::BlendMin:
			blend_op = GL_MIN;
			break;
		case BlendOp::BlendMax:
			blend_op = GL_MAX;
			break;
		case BlendOp::BlendRevSub:
			blend_op = GL_FUNC_REVERSE_SUBTRACT;
			break;
		}

		return blend_op;
	}

	void DeviceGL::SetBlendFunc(BlendArg src, BlendArg dest)
	{
		glBlendFunc(GetBlendArg(src), GetBlendArg(dest));
	}

	void DeviceGL::SetBlendOperation(BlendOp op)
	{
		glBlendEquation(GetBlendOp(op));
	}

	void DeviceGL::SetDepthTest(bool enable)
	{
		if (enable)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void DeviceGL::SetDepthWriting(bool enable)
	{
		if (enable)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			//glDepthMask(GL_FALSE);
		}
	}

	void DeviceGL::SetDepthFunc(CompareFunc func)
	{
		glDepthFunc(GetCompareFunc(func));
	}

	void DeviceGL::SetCulling(CullMode mode)
	{
		if (mode == CullMode::CullNone)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);

			if (mode == CullMode::CullCW)
			{
				glFrontFace(GL_CCW);
			}
			else
			{
				glFrontFace(GL_CW);
			}
		}
	}

	void DeviceGL::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
	{
		if (enable)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(slopeZBias, depthOffset);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	void DeviceGL::SetViewport(const Viewport& viewport)
	{
		cur_viewport = viewport;
		glViewport(cur_viewport.x, cur_viewport.y, cur_viewport.width, cur_viewport.height);
		glDepthRangef(cur_viewport.minZ, cur_viewport.maxZ);

		vp_was_setted = true;
	}

	void DeviceGL::GetViewport(Viewport& viewport)
	{
		if (vp_was_setted)
		{
			viewport.width = cur_viewport.width;
			viewport.height = cur_viewport.height;
			viewport.x = cur_viewport.x;
			viewport.y = cur_viewport.y;
			viewport.minZ = cur_viewport.minZ;
			viewport.maxZ = cur_viewport.maxZ;
		}
		else
		{
			viewport.x = 0;
			viewport.y = 0;

			if (cur_rt[0])
			{
				viewport.width = cur_rt_w;
				viewport.height = cur_rt_h;
			}
			else
			{
				viewport.width = cur_depth_w;
				viewport.height = cur_depth_h;
			}

			viewport.minZ = 0.0f;
			viewport.maxZ = 1.0f;
		}
	}

	void DeviceGL::SetRenderTarget(int slot, Texture* rt)
	{
		vp_was_setted = false;
		need_set_rt = true;
		use_def_backbuffer = false;
		cur_rt[slot] = rt ? (TextureGL*)rt : nullptr;

		if (slot == 0)
		{
			cur_rt_w = rt ? rt->GetWidth() : -1;
			cur_rt_h = rt ? rt->GetHeight() : -1;
		}
	}

	void DeviceGL::SetDepth(Texture* depth)
	{
		vp_was_setted = false;
		need_set_rt = true;
		use_def_backbuffer = false;
		cur_depth = depth ? (TextureGL*)depth : nullptr;
		cur_depth_w = cur_depth ? cur_depth->GetWidth() : -1;
		cur_depth_h = cur_depth ? cur_depth->GetHeight() : -1;
	}

	void DeviceGL::RestoreRenderTarget()
	{
		vp_was_setted = false;
		cur_rt_w = scr_w;
		cur_rt_h = scr_h;

		need_set_rt = true;
		use_def_backbuffer = true;

		for (int i = 0; i < 6; i++)
		{
			cur_rt[i] = nullptr;
		}

		cur_depth = nullptr;
	}

	void DeviceGL::Release()
	{
		delete this;
	}
}