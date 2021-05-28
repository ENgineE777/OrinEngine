
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

	void Data::LoadTexture()
	{
		texture = root.render.LoadTexture(texName.c_str(), _FL_);
		width = texture ? texture->GetWidth() : 0;
		height = texture ? texture->GetHeight() : 0;
	}

	void Load(JsonReader& loader, Data* sprite, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			loader.Read("width", sprite->width);
			loader.Read("height", sprite->height);
			loader.Read("type", (int&)sprite->type);
			loader.Read("color", sprite->color);

			loader.Read("texName", sprite->texName);
			loader.Read("texMode", (int&)sprite->mode);

			if (sprite->type == Type::Frames) loader.Read("frame_time", sprite->frameTime);

			int count = 1;
			loader.Read("count", count);
			sprite->frames.resize(count);

			for (int i = 0; i<count; i++)
			{
				Frame& frame = sprite->frames[i];

				loader.EnterBlock("Rect");

				loader.Read("pos", frame.pos);
				loader.Read("size", frame.size);
				loader.Read("uv", frame.uv);
				loader.Read("duv", frame.duv);
				loader.Read("offset", frame.offset);

				if (sprite->type == Type::Frames)
				{
					loader.Read("frame_time", frame.time);
				}

				loader.LeaveBlock();
			}

			loader.LeaveBlock();

			sprite->LoadTexture();
		}
	}

	void Save(JsonWriter& saver, Data* sprite, const char* name)
	{
		saver.StartBlock(name);

		saver.Write("width", sprite->width);
		saver.Write("height", sprite->height);
		saver.Write("type", (int)sprite->type);
		saver.Write("color", sprite->color);

		saver.Write("texName", sprite->texName.c_str());
		saver.Write("texMode", (int)sprite->mode);

		if (sprite->type == Type::Frames) saver.Write("frame_time", sprite->frameTime);

		int count = (int)sprite->frames.size();
		saver.Write("count", count);

		saver.StartArray("Rect");

		for (int i = 0; i<count; i++)
		{
			Frame& rect = sprite->frames[i];

			saver.StartBlock(nullptr);

			saver.Write("pos", rect.pos);
			saver.Write("size", rect.size);
			saver.Write("uv", rect.uv);
			saver.Write("duv", rect.duv);
			saver.Write("offset", rect.offset);

			if (sprite->type == Type::Frames) saver.Write("frame_time", rect.time);

			saver.FinishBlock();
		}

		saver.FinishArray();

		saver.FinishBlock();
	}

	void Copy(Data* src, Data* dest)
	{
		dest->width = src->width;
		dest->height = src->height;
		dest->type = src->type;
		dest->color = src->color;

		dest->mode = src->mode;
		dest->texName = src->texName;

		dest->frameTime = src->frameTime;

		dest->texture = src->texture;

		int count = (int)src->frames.size();
		dest->frames.resize(count);

		for (int i = 0; i < count; i++)
		{
			dest->frames[i] = src->frames[i];
		}
	}

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

	void UpdateFrame(Sprite::Data* data, FrameState* state, float dt)
	{
		UpdateFrame(data, state, dt, [](float from, float to) {});
	}

	void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, bool use_depth, bool flipped)
	{
		root.render.GetDevice()->SetVertexBuffer(0, buffer);
		root.render.GetDevice()->SetVertexDecl(vdecl);

		root.render.GetDevice()->SetProgram(quadPrg);

		Device::Viewport viewport;
		root.render.GetDevice()->GetViewport(viewport);

		Math::Vector4 params[3];
		params[0] = Math::Vector4(pos.x, pos.y, size.x, size.y);
		params[1] = Math::Vector4(uv.x, uv.y, duv.x, duv.y);

		if (flipped)
		{
			params[1] = Math::Vector4(uv.x + duv.x, uv.y, -duv.x, duv.y);
		}

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

	void Draw(Transform* trans, Color clr, Data* sprite, FrameState* state, bool use_depth, bool ignore_camera)
	{
		if (sprite->texture)
		{
			sprite->texture->SetAdress(sprite->mode);
		}

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

		clr *= sprite->color;

		if (!sprite->texture)
		{
			Draw(sprite->texture, clr, local_trans, pos, size, 0.0f, 1.0f, false);
		}
		else
		if (sprite->type == Type::Image)
		{
			Frame& frame = sprite->frames[0];
			Draw(sprite->texture, clr, local_trans, pos, size, frame.uv, frame.duv, use_depth, state->horz_flipped);
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
	}

	void Release()
	{
		vdecl.ReleaseRef();
		buffer.ReleaseRef();
		quadPrg.ReleaseRef();
	}
}



