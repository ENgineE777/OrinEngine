#include "DebugBoxes.h"
#include "Root/Root.h"

namespace Oak
{
	void DebugBoxes::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float3, ElementSemantic::Texcoord, 0 },{ ElementType::Ubyte4, ElementSemantic::Color, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(3, desc, _FL_);

		vbuffer = root.render.GetDevice()->CreateBuffer(24, sizeof(Vertex), _FL_);

		Vertex* vertices = (Vertex*)vbuffer->Lock();

		Math::Vector3 base_vertices[] =
		{
			{-0.5f, 0.5f, -0.5f},
			{0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, 0.5f},
			{0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f},
			{ 0.5f, -0.5f, 0.5f}
		};

		int bace_indices[] =
		{
			0, 1, 2, 2, 1, 3,
			4, 0, 6, 6, 0, 2,
			7, 5, 6, 6, 5, 4,
			3, 1, 7, 7, 1, 5,
			4, 5, 0, 0, 5, 1,
			3, 7, 2, 2, 7, 6
		};

		int box_indices[] =
		{
			0, 1, 2, 2, 1, 3,
			4, 5, 6, 6, 5, 7,
			8, 9,10,10, 9,11,
			12, 13, 14, 14, 13, 15,
			16, 17, 18, 18, 17, 19,
			20, 21, 22, 22, 21, 23
		};

		for (int i = 0; i < 36; i++)
		{
			vertices[box_indices[i]].pos = base_vertices[bace_indices[i]];
			vertices[i].color = 0xffffffff;
		}

		Math::Vector3 noramls[] =
		{
			{ 0.0f, 0.0f,-1.0f},
			{ 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f},
			{ 1.0f, 0.0f, 0.0f},
			{ 0.0f, 1.0f, 0.0f},
			{ 0.0f,-1.0f, 0.0f}
		};

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				vertices[i * 4 + j].normal = noramls[i];
			}
		}

		vbuffer->Unlock();

		ibuffer = root.render.GetDevice()->CreateBuffer(12 * 3, sizeof(int), _FL_);

		int* indices = (int*)ibuffer->Lock();

		for (int i = 0; i < 36; i++)
		{
			indices[i] = box_indices[i];
		}

		ibuffer->Unlock();

		prg = root.render.GetProgram("DbgTriangle");

		debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugBoxes::Draw);
	}

	void DebugBoxes::AddBox(Math::Matrix trans, Color color, Math::Vector3 scale)
	{
		boxes.push_back(Box());
		Box* box = &boxes[boxes.size()-1];

		Math::Matrix scale_mat;
		scale_mat.Scale(scale);
		box->trans = scale_mat * trans;
		box->color = color;
	}

	void DebugBoxes::Draw(float dt)
	{
		if (boxes.size() == 0)
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

		root.render.GetDevice()->SetAlphaBlend(false);

		for (int i=0;i<boxes.size();i++)
		{
			Box& box = boxes[i];

			prg->SetMatrix(ShaderType::Vertex, "trans", &box.trans, 1);
			prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&box.color, 1);

			root.render.GetDevice()->DrawIndexed(PrimitiveTopology::TrianglesList, 0, 0, 12);
		}

		boxes.clear();
	}

	void DebugBoxes::Release()
	{
		RELEASE(vbuffer)
		RELEASE(ibuffer)
		RELEASE(vdecl)
		RELEASE(prg)

		delete this;
	}
}