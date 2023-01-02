
#include "Terrain.h"
#include "Root/Root.h"
#include "Root/Meshes/MeshPrograms.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Terrain, "3D/Geometry", "Terrain")

	META_DATA_DESC(Terrain)
		BASE_SCENE_ENTITY_PROP(Terrain)
		FLOAT_PROP(Terrain, scaleh, 0.5f, "Geometry", "ScaleH", "Horizontal scale")
		FLOAT_PROP(Terrain, scalev, 0.1f, "Geometry", "ScaleV", "Vertical scale")
		FILENAME_PROP(Terrain, tex_name, "", "Geometry", "Texture")
		FILENAME_PROP(Terrain, hgt_name, "", "Geometry", "Heightmap")
		COLOR_PROP(Terrain, color, COLOR_WHITE, "Geometry", "Color")
	META_DATA_DESC_END()

	Terrain::Terrain() : SceneEntity()
	{

	}

	Terrain::~Terrain()
	{

	}

	void Terrain::Init()
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float2, ElementSemantic::Texcoord, 0 },{ ElementType::Float3, ElementSemantic::Texcoord, 1 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(3, desc, _FL_);

		Tasks(true)->AddTask(10, this, (Object::Delegate)&Terrain::Render);

		GetScene()->AddToGroup(this, "Terrain");
	}

	void Terrain::ApplyProperties()
	{
		int stride = sizeof(VertexTri);

		LoadHMap(hgt_name.c_str());

		sz = (hwidth) * (hheight) * 2;

		buffer = root.render.GetDevice()->CreateBuffer(sz * 3, stride, _FL_);

		VertexTri* v_tri = (VertexTri*)buffer->Lock();

		float start_x = -hwidth * 0.5f * scaleh;
		float start_z = -hheight * 0.5f * scaleh;

		float du = 1.0f / (hwidth - 1.0f);
		float dv = 1.0f / (hheight - 1.0f);

		bool shift = false;

		for (int j = 0; j < hheight - 1; j++)
		{
			for (int i = 0; i < hwidth - 1; i++)
			{
				if (shift)
				{
					if (i % 2 == 0)
					{
						v_tri[1].position = GetVecHeight(i,j);
						v_tri[1].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[0].position = GetVecHeight(i+1, j);
						v_tri[0].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[2].position = GetVecHeight(i+1, j+1);
						v_tri[2].texCoord = Math::Vector2(1.0f, 1.0f);

						v_tri[4].position = GetVecHeight(i, j);
						v_tri[4].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[3].position = GetVecHeight(i + 1, j + 1);
						v_tri[3].texCoord = Math::Vector2(1.0f, 1.0f);
						v_tri[5].position = GetVecHeight(i, j + 1);
						v_tri[5].texCoord = Math::Vector2(0.0f, 1.0f);
					}
					else
					{
						v_tri[1].position = GetVecHeight(i+1,j);
						v_tri[1].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[0].position = GetVecHeight(i+1,j+1);
						v_tri[0].texCoord = Math::Vector2(1.0f, 1.0f);
						v_tri[2].position = GetVecHeight(i, j+1);
						v_tri[2].texCoord = Math::Vector2(0.0f, 1.0f);

						v_tri[4].position = GetVecHeight(i, j);
						v_tri[4].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[3].position = GetVecHeight(i+1, j);
						v_tri[3].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[5].position = GetVecHeight(i, j + 1);
						v_tri[5].texCoord = Math::Vector2(0.0f, 1.0f);
					}
				}
				else
				{
					if (i % 2 == 0)
					{
						v_tri[1].position = GetVecHeight(i, j);
						v_tri[1].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[0].position = GetVecHeight(i + 1, j);
						v_tri[0].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[2].position = GetVecHeight(i, j + 1);
						v_tri[2].texCoord = Math::Vector2(0.0f, 1.0f);

						v_tri[4].position = GetVecHeight(i + 1, j);
						v_tri[4].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[3].position = GetVecHeight(i + 1, j + 1);
						v_tri[3].texCoord = Math::Vector2(1.0f, 1.0f);
						v_tri[5].position = GetVecHeight(i, j + 1);
						v_tri[5].texCoord = Math::Vector2(0.0f, 1.0f);
					}
					else
					{
						v_tri[1].position = GetVecHeight(i, j);
						v_tri[1].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[0].position = GetVecHeight(i + 1, j + 1);
						v_tri[0].texCoord = Math::Vector2(1.0f, 1.0f);
						v_tri[2].position = GetVecHeight(i, j + 1);
						v_tri[2].texCoord = Math::Vector2(0.0f, 1.0f);

						v_tri[4].position = GetVecHeight(i, j);
						v_tri[4].texCoord = Math::Vector2(0.0f, 0.0f);
						v_tri[3].position = GetVecHeight(i + 1, j);
						v_tri[3].texCoord = Math::Vector2(1.0f, 0.0f);
						v_tri[5].position = GetVecHeight(i + 1, j + 1);
						v_tri[5].texCoord = Math::Vector2(1.0f, 1.0f);
					}
				}

				float cur_du = du * i;
				float cur_dv = dv * (hheight - 1 - j);

				for (int k = 0; k < 6; k++)
				{
					v_tri[k].position.x = start_x + v_tri[k].position.x * scaleh;
					v_tri[k].position.z = start_z - v_tri[k].position.z * scaleh;
					v_tri[k].texCoord = Math::Vector2(cur_du + v_tri[k].texCoord.x * du, cur_dv - v_tri[k].texCoord.y * dv);
				}

				for (int k = 0; k < 2; k++)
				{
					Math::Vector3 v1 = v_tri[k * 3 + 1].position - v_tri[k * 3 + 0].position;
					Math::Vector3 v2 = v_tri[k * 3 + 2].position - v_tri[k * 3 + 0].position;
				
					v1.Normalize();
					v2.Normalize();

					Math::Vector3 normal;
				
					normal = v1.Cross(v2);
					normal.Normalize();

					for (int l = 0; l < 3; l++)
					{
						v_tri[k * 3 + l].normal = normal;
					}
				}

				v_tri += 6;
			}

			shift = !shift;
		}

		buffer->Unlock();

		texture = root.render.LoadTexture(tex_name.c_str(), _FL_);
	}

	float Terrain::GetHeight(int i, int j)
	{
		return hmap ? hmap[((j)* hwidth + i)] * scalev : 1.0f;
	}

	Math::Vector3 Terrain::GetVecHeight(int i, int j)
	{
		return Math::Vector3((float)i, GetHeight(i, j), -(float)j);
	}

	void Terrain::LoadHMap(const char* hgt_name)
	{
		FileInMemory hbuffer;

		FREE_PTR(hmap)

		uint8_t* ptr = nullptr;
		int colorMode = 4;

		if (!hbuffer.Load(hgt_name))
		{
			hwidth = 512;
			hheight = 512;
		}
		else
		{
			ptr = hbuffer.GetData();
			ptr += 2;

			uint8_t imageTypeCode = *((uint8_t*)ptr);

			if (imageTypeCode != 2 && imageTypeCode != 3)
			{
				return;
			}

			ptr += 10;

			short int imageWidth = *((short int*)ptr);
			ptr += 2;

			short int imageHeight = *((short int*)ptr);
			ptr += 2;

			uint8_t bitCount = *((uint8_t*)ptr);
			ptr += 2;

			colorMode = bitCount / 8;

			hwidth = imageWidth;
			hheight = imageHeight;
		}

		hmap = (uint8_t*)malloc(hwidth * hheight);

		for (int i = 0; i < hheight; i++)
		{
			for (int j = 0; j < hwidth; j++)
			{
				hmap[i * hheight + j] = ptr ? ptr[((j)* hwidth + i) * colorMode] : 0;
			}
		}

	#ifdef PLATFORM_WIN
		eastl::string cooked_name = hgt_name + eastl::string("hm");

		if (!root.files.IsFileExist(cooked_name.c_str()))
		{
			root.physics.CookHeightmap(hwidth, hheight, hmap, cooked_name.c_str());
		}
	#endif
	}

	void Terrain::Render(float dt)
	{
		Render(MeshPrograms::GetTranglPrg());
	}

	void Terrain::ShRender(float dt)
	{
		Render(MeshPrograms::GetShdTranglPrg());
	}

	void Terrain::Render(RenderTechnique* prg)
	{
		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, buffer);

		root.render.GetDevice()->SetRenderTechnique(prg);

		root.render.SetTransform(TransformStage::World, Math::Matrix());

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		Math::Matrix mat;
		Math::Matrix world;

		prg->SetMatrix(ShaderType::Vertex, "trans", &world, 1);
		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&color, 1);
		prg->SetTexture(ShaderType::Pixel, "diffuseMap", texture);

		root.render.GetDevice()->Draw(PrimitiveTopology::TrianglesList, 0, sz);
	}

	void Terrain::Play()
	{
		SceneEntity::Play();

		body.object = this;
		body.body = root.GetPhysScene()->CreateHeightmap(hwidth, hheight, Math::Vector2(scaleh, scalev), (hgt_name + eastl::string("hm")).c_str(), 1);
		body.body->SetUserData(&body);
	}

	void Terrain::Release()
	{
		RELEASE(body.body);

		SceneEntity::Release();
	}
}