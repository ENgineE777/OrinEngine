#include "DebugTriangles2D.h"
#include "DebugPrograms.h"

namespace Oak
{
	void DebugTriangles2D::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 },{ ElementType::Ubyte4, ElementSemantic::Color, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(2, desc, _FL_);

		vbuffer = root.render.GetDevice()->CreateBuffer(3000, sizeof(Vertex), _FL_);

		prg = root.render.GetProgram("DbgTriangle2D");

		debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugTriangles2D::Draw);
	}

	void DebugTriangles2D::AddTriangle(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color)
	{
		triangles.push_back(Triangle());
		Triangle* triangle = &triangles[triangles.size()-1];

		triangle->p[0] = p1;
		triangle->p[1] = p2;
		triangle->p[2] = p3;
		triangle->color = color.Get();
	}

	void DebugTriangles2D::Draw(float dt)
	{
		if (triangles.size() == 0)
		{
			return;
		}

		root.render.GetDevice()->SetProgram(prg);

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, vbuffer);

		Math::Vector4 param = Math::Vector4((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight(), 0, 0);
		prg->SetVector(ShaderType::Vertex, "param", &param, 1);

		int index = 0;
		Vertex* vertices = (Vertex*)vbuffer->Lock();

		for (int i=0;i<triangles.size();i++)
		{
			Triangle& triangle = triangles[i];

			for (int j = 0; j < 3; j++)
			{
				vertices[index * 3 + j].pos = triangle.p[j];
				vertices[index * 3 + j].color = triangle.color;
			}

			index++;

			if (index > 330)
			{
				vbuffer->Unlock();
				root.render.GetDevice()->Draw(PrimitiveTopology::TrianglesList, 0, index);

				index = 0;
				vertices = (Vertex*)vbuffer->Lock();
			}
		}

		vbuffer->Unlock();

		if (index > 0)
		{
			root.render.GetDevice()->Draw(PrimitiveTopology::TrianglesList, 0, index);
		}

		triangles.clear();
	}

	void DebugTriangles2D::Release()
	{
		RELEASE(vbuffer)
		RELEASE(vdecl)
		RELEASE(prg)

		delete this;
	}
}