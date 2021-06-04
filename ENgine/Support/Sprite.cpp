
#include "Sprite.h"
#include "Root/Root.h"

namespace Oak::Sprite
{
	class QuadProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	CLASSREGEX(Program, QuadProgram, QuadProgram, "QuadProgram")
	CLASSREGEX_END(Program, QuadProgram)

	ProgramRef quadPrg;
	VertexDeclRef vdecl;
	DataBufferRef buffer;

	float pixelsPerUnit = 50.0f;
	float pixelsPerUnitInvert = 1.0f / pixelsPerUnit;
	float pixelsHeight = 1080.0f;

	void Init()
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(1, desc, _FL_);

		int stride = sizeof(Math::Vector2);
		buffer = root.render.GetDevice()->CreateBuffer(4, stride, _FL_);

		Math::Vector2* v = (Math::Vector2*)buffer->Lock();

		v[0] = Math::Vector2(0.0f, 1.0f);
		v[1] = Math::Vector2(1.0f, 1.0f);
		v[2] = Math::Vector2(0.0f, 0.0f);
		v[3] = Math::Vector2(1.0f, 0.0f);

		buffer->Unlock();

		quadPrg = root.render.GetProgram("QuadProgram", _FL_);
	}

	/*void UpdateFrame(Sprite::Data* data, FrameState* state, float dt)
	{
		UpdateFrame(data, state, dt, [](float from, float to) {});
	}*/

	void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv)
	{
		root.render.GetDevice()->SetVertexBuffer(0, buffer);
		root.render.GetDevice()->SetVertexDecl(vdecl);

		root.render.GetDevice()->SetProgram(quadPrg);

		Device::Viewport viewport;
		root.render.GetDevice()->GetViewport(viewport);

		Math::Vector4 params[3];
		params[0] = Math::Vector4(pos.x, pos.y, size.x, size.y);
		params[1] = Math::Vector4(uv.x, uv.y, duv.x, duv.y);
		params[2] = Math::Vector4((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight(), 0.5f, pixelsPerUnitInvert);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		trans.Pos() *= pixelsPerUnitInvert;

		quadPrg->SetVector(ShaderType::Vertex, "desc", &params[0], 3);
		quadPrg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		quadPrg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		quadPrg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		quadPrg->SetTexture(ShaderType::Pixel, "diffuseMap", texture ? texture : root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
	}

	/*void Draw(Transform* trans, Color clr, AssetTextureRef texture, int sheetIndex)
	{
		Math::Matrix local_trans = trans->global;
		Math::Vector3 pos3d = Math::Vector3(trans->offset.x, 1.0f - trans->offset.y, trans->offset.z) * trans->size * Math::Vector3(-1.0f, -1.0f, -1.0f);
		Math::Vector2 pos = Math::Vector2(pos3d.x, pos3d.y);
		Math::Vector2 size = Math::Vector2(trans->size.x, trans->size.y);

		/*Math::Vector3 min_pos(10000000.0f);
		Math::Vector3 max_pos(-10000000.0f);

		Math::Vector3 tmp[] = { Math::Vector3(pos.x, pos.y, 0), Math::Vector3(pos.x + size.x, pos.y, 0), Math::Vector3(pos.x + size.x, pos.y + size.y, 0), Math::Vector3(pos.x, pos.y + size.y, 0) };
	
		for (int i = 0; i < 4; i++)
		{
			Math::Vector3 temp = tmp[i] * local_trans;
			min_pos.Min(temp);
			max_pos.Max(temp);
		}

		if (max_pos.x < 0 || min_pos.x > root.render.GetDevice()->GetWidth() ||
			max_pos.y < 0 || min_pos.y > root.render.GetDevice()->GetHeight())
		{
			return;
		}*/

		/*if (texture.sliceIndex == -1 || texture.sliceIndex >= texture->spriteSheet.slices.size())
		{
			Draw(sprite->texture, clr, local_trans, pos, size, 0.0f, 1.0f);
		}
		else
		{
			Sprite::Sheet& sheet = texture->spriteSheet;
			Sprite::Slice& slice = sheet.slices[texture.sliceIndex];
			Draw(texture->GetTexture(), clr, local_trans, pos, size, Math::Vector2(slice.pos.x, sheet.size.y - slice.pos.y) / sheet.size, slice.size / sheet.size);
		}*/

		/*else
		if (sprite->type == Type::Image)
		{
			Frame& frame = sprite->frames[0];
			Draw(sprite->texture, clr, local_trans, pos, size, frame.uv, frame.duv);
		}
		else
		if (sprite->type == Type::Frames)
		{
			Frame& startFrame = sprite->frames[0];
			Frame& frame = sprite->frames[state->curFrame];

			Math::Vector2 frameScale = size / startFrame.size;
			Math::Vector2 sz = frameScale * frame.size;
			Math::Vector2 dpos = frameScale * frame.offset;

			if (state->horz_flipped)
			{
				dpos.x = -dpos.x;
			}

			dpos += Math::Vector2((size.x - sz.x) * 0.5f, size.y - sz.y);

			Draw(sprite->texture, clr, local_trans, pos + dpos, sz, frame.uv, frame.duv, use_depth, state->horz_flipped);
		}
		else
		if (sprite->type == Type::ThreeSlicesVert)
		{
			float y[] = { 0, sprite->frames[0].size.y, size.y - sprite->frames[2].size.y, size.y };

			for (int i = 0; i < 3; i++)
			{
				Frame& frame = sprite->frames[i];
				Draw(sprite->texture, clr, local_trans, pos + Math::Vector2(0.0f, y[i]), Math::Vector2(size.x, y[i+1] - y[i]), frame.uv, frame.duv, use_depth);
			}
		}
		else
		if (sprite->type == Type::ThreeSlicesHorz)
		{
			float x[] = { 0, sprite->frames[0].size.x, size.x - sprite->frames[2].size.x, size.x };

			for (int i = 0; i < 3; i++)
			{
				Frame& frame = sprite->frames[i];
				Draw(sprite->texture, clr, local_trans, pos + Math::Vector2(x[i], 0.0f), Math::Vector2(x[i + 1] - x[i], size.y), frame.uv, frame.duv, use_depth);
			}
		}
		else
		if (sprite->type == Type::NineSlices)
		{
			float x[] = { 0, sprite->frames[0].size.x, size.x - sprite->frames[2].size.x, size.x };
			float y[] = { 0, sprite->frames[0].size.y, size.y - sprite->frames[8].size.y, size.y };

			int index = 0;
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				{
					Frame& frame = sprite->frames[index];
					Draw(sprite->texture, clr, local_trans, pos + Math::Vector2(x[j], y[i]), Math::Vector2(x[j + 1] - x[j], y[i + 1] - y[i]), frame.uv, frame.duv, use_depth);
					index++;
				}
		}
	}*/

	void Release()
	{
		vdecl.ReleaseRef();
		buffer.ReleaseRef();
		quadPrg.ReleaseRef();
	}
}



