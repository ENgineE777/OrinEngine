#include "DebugTriangles.h"
#include "DebugPrograms.h"

namespace Oak
{
	void DebugTriangles::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float3, ElementSemantic::Texcoord, 0 },{ ElementType::Ubyte4, ElementSemantic::Color, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(3, desc, _FL_);

		vbuffer = root.render.GetDevice()->CreateBuffer(3000, sizeof(Vertex), _FL_);

		prg = root.render.GetProgram("DbgTriangle", _FL_);

		debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugTriangles::Draw);
	}

	void DebugTriangles::AddTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color)
	{
		triangles.push_back(Triangle());
		Triangle* triangle = &triangles[triangles.size()-1];

		triangle->p[0] = p1;
		triangle->p[1] = p2;
		triangle->p[2] = p3;
		triangle->color = color.Get();
	}

	void DebugTriangles::Draw(float dt)
	{
		if (triangles.size() == 0)
		{
			return;
		}

		root.render.GetDevice()->SetProgram(prg);

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, vbuffer);

		Math::Matrix view_proj;
		Math::Matrix tmp;
		root.render.SetTransform(TransformStage::World, tmp);
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);
		view.Inverse();
		Math::Vector4 vz = Math::Vector4(-view.Vz());

		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "lightDir", &vz, 1);

		Math::Matrix trans;
		Color color = COLOR_WHITE;
		prg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&color, 1);

		int index = 0;
		Vertex* vertices = (Vertex*)vbuffer->Lock();

		for (int i=0;i<triangles.size();i++)
		{
			Triangle& triangle = triangles[i];

			Math::Vector3 normal = (triangle.p[0] - triangle.p[1]);
			Math::Vector3 dir = triangle.p[2] - triangle.p[1];
			normal.Cross(dir);

			for (int j = 0; j < 3; j++)
			{
				vertices[index * 3 + j].pos = triangle.p[j];
				vertices[index * 3 + j].normal = normal;
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

	void DebugTriangles::Release()
	{
		delete this;
	}
}