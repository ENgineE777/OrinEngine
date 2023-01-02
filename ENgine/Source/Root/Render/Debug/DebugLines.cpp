#include "DebugLines.h"
#include "DebugTechiques.h"

namespace Orin
{
	void DebugLines::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 }, { ElementType::Ubyte4, ElementSemantic::Color, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(2, desc, _FL_);

		buffer = root.render.GetDevice()->CreateBuffer(MaxSize * 2, sizeof(Vertex), _FL_);

		prg = root.render.GetRenderTechnique<DebugTechiques::Line>(_FL_);
		prgDepth = root.render.GetRenderTechnique<DebugTechiques::LineWithDepth>(_FL_);

		debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugLines::Draw);
	}

	void DebugLines::AddLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth)
	{
		eastl::vector<Vertex>* ln;

		if (use_depth)
		{
			ln = &lines_with_depth;
		}
		else
		{
			ln = &lines;
		}

		ln->push_back(Vertex(from, from_clr.Get()));
		ln->push_back(Vertex(to, to_clr.Get()));
	}

	void DebugLines::AddLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr)
	{
		lines_2d.push_back(Vertex(Math::Vector3(from.x, from.y, 1.0f), from_clr.Get()));
		lines_2d.push_back(Vertex(Math::Vector3(to.x, to.y, 1.0f), to_clr.Get()));
	}

	void DebugLines::DrawCircle(int axis, Math::Vector3 pos, Color color, float radius)
	{
		float last_dx = -radius * 2.0f;
		float last_dz = -radius * 2.0f;
		int nums = 32;

		for (int i = 0; i < nums + 1; i++)
		{
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * radius;
			float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * radius;

			if (last_dx > -radius * 1.5f)
			{
				Math::Vector3 ps;
				Math::Vector3 ps2;

				if (axis == 0)
				{
					ps.Set(0.0f, last_dx, last_dz);
					ps2.Set(0.0f, dx, dz);
				}
				else
				if (axis == 1)
				{
					ps.Set(last_dx, 0.0f, last_dz);
					ps2.Set(dx, 0.0f, dz);
				}
				else
				{
					ps.Set(last_dx, last_dz, 0.0f);
					ps2.Set(dx, dz, 0.0f);
				}

				ps += pos;
				ps2 += pos;

				AddLine(ps, color, ps2, color, false);
			}

			last_dx = dx;
			last_dz = dz;
		}
	}

	void DebugLines::DrawLines(RenderTechnique* prog, eastl::vector<Vertex>& lines, bool is2d)
	{
		if (lines.size()==0) return;

		root.render.GetDevice()->SetRenderTechnique(prog);

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, buffer);

		Math::Matrix view_proj;
		root.render.SetTransform(TransformStage::World, Math::Matrix());
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		Math::Matrix view, proj, inv_view;

		if (is2d)
		{
			root.render.GetTransform(TransformStage::View, view);
			root.render.GetTransform(TransformStage::Projection, proj);

			inv_view = view;
			inv_view.Inverse();
		}

		prog->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);

		Vertex* v = (Vertex*)buffer->Lock();

		int count = 0;

		float screenWidth = (float)root.render.GetDevice()->GetWidth();
		float screenHeight = (float)root.render.GetDevice()->GetHeight();

		for (int i=0; i<lines.size(); i+=2)
		{
			v[count * 2 + 0] = lines[i];
			v[count * 2 + 1] = lines[i+1];

			if (is2d)
			{
				for (int j = 0; j < 2; j++)
				{
					v[count * 2 + j].p.x = (2.0f * v[count * 2 + j].p.x / screenWidth - 1) / proj._11;
					v[count * 2 + j].p.y = -(2.0f * v[count * 2 + j].p.y / screenHeight - 1) / proj._22;

					Math::Vector3 dir = inv_view.MulNormal(v[count * 2 + j].p);
					v[count * 2 + j].p = inv_view.Pos() + dir * 10.0f;
				}
			}

			count++;

			if (count == MaxSize)
			{
				buffer->Unlock();

				root.render.GetDevice()->Draw(PrimitiveTopology::LinesList, 0, MaxSize);

				count = 0;
				v = (Vertex*)buffer->Lock();
			}
		}
	
		buffer->Unlock();

		if (count > 0)
		{
			root.render.GetDevice()->Draw(PrimitiveTopology::LinesList, 0, count);
		}

		lines.clear();
	}

	void DebugLines::Draw(float dt)
	{
		DrawLines(prgDepth, lines_with_depth, false);
		DrawLines(prg, lines, false);
		DrawLines(prg, lines_2d, true);
	}

	void DebugLines::Release()
	{
		delete this;
	}
}