#include "DebugSpheres.h"
#include "DebugPrograms.h"

namespace Oak
{
	void DebugSpheres::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float3, ElementSemantic::Texcoord, 0 }, { ElementType::Ubyte4, ElementSemantic::Color, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(3, desc, _FL_);

		vbuffer = root.render.GetDevice()->CreateBuffer(SidesCount * (RigsCount + 1), sizeof(Vertex), _FL_);

		Vertex* vertices = (Vertex*)vbuffer->Lock();

		float deltaTheta = Math::PI / RigsCount;
		float deltaPhi = Math::TwoPI / SidesCount;
		float curTheta = 0.0f;

		int index = 0;

		for (int i = 0; i <= RigsCount; i++)
		{
			float y = cosf(curTheta);
			float sinTheta = sinf(curTheta);
			float curPhi = 0.0f;

			for (int j = 0; j < SidesCount; j++)
			{
				vertices[index].pos = Math::Vector3(sinTheta * sinf(curPhi), y, -sinTheta * cosf(curPhi));
				vertices[index].normal = vertices[index].pos;
				vertices[index].normal.Normalize();
				vertices[index].color = 0xffffffff;
				index++;

				curPhi += deltaPhi;
			}

			curTheta += deltaTheta;
		}

		vbuffer->Unlock();

		ibuffer = root.render.GetDevice()->CreateBuffer(PrimCount * 3, sizeof(int), _FL_);

		int* indices = (int*)ibuffer->Lock();

		index = 0;

		for (int i = 0; i < RigsCount; i++)
		{
			int top_index = i * SidesCount;
			int bottom_index = (i+1) * SidesCount;

			for (int j = 0; j < SidesCount; j++)
			{
				indices[index + 0] = (top_index + ((j + 0) % SidesCount));
				indices[index + 1] = (bottom_index + ((j + 1) % SidesCount));
				indices[index + 2] = (bottom_index + ((j + 0) % SidesCount));

				indices[index + 3] = (bottom_index + ((j + 1) % SidesCount));
				indices[index + 4] = (top_index + ((j + 0) % SidesCount));
				indices[index + 5] = (top_index + ((j + 1) % SidesCount));
				index += 6;
			}
		}

		ibuffer->Unlock();

		prg = root.render.GetProgram("DbgTriangle", _FL_);

		debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugSpheres::Draw);
	}

	void DebugSpheres::AddSphere(Math::Vector3 pos, Color color, float radius)
	{
		spheres.push_back(Sphere());
		Sphere* sphere = &spheres[spheres.size()-1];

		sphere->pos = pos;
		sphere->color = color;
		sphere->radius = radius;
	}

	void DebugSpheres::Draw(float dt)
	{
		if (spheres.size() == 0)
		{
			return;
		}

		root.render.GetDevice()->SetProgram(prg);

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, vbuffer);
		root.render.GetDevice()->SetIndexBuffer(ibuffer);

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

		root.render.GetDevice()->SetAlphaBlend(true);

		for (int i=0;i<spheres.size();i++)
		{
			Sphere& sphere = spheres[i];

			Math::Matrix mat;
			Math::Vector3 scale = sphere.radius;
			mat.Scale(scale);
			mat.Pos() = sphere.pos;

			prg->SetMatrix(ShaderType::Vertex, "trans", &mat, 1);
			prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&sphere.color, 1);

			root.render.GetDevice()->DrawIndexed(PrimitiveTopology::TrianglesList, 0, 0, PrimCount);
		}

		spheres.clear();
	}

	void DebugSpheres::Release()
	{
		delete this;
	}
}