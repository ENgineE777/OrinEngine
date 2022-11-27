
#include "MeshRes.h"
#include "Root/Root.h"
#include "MeshPrograms.h"
#include "Support/fbx/ofbx.h"

namespace Oak
{
	void Mesh::Instance::Init(Mesh* model, TaskExecutor::SingleTaskPool* setTaskPool)
	{
		res = model;

		taskPool = setTaskPool;
		taskPool->AddTask(10, this, (Object::Delegate)&Mesh::Instance::Render);

		//render.AddDelegate("toshadow", this, (Object::Delegate)&Model::Drawer::Render, 0);
		//render.AddDelegate("geometry", this, (Object::Delegate)&Model::Drawer::Render, 0);
		//render.AddDelegate("shgeometry", this, (Object::Delegate)&Model::Drawer::ShRender, 0);
	}

	Math::Vector3 Mesh::Instance::GetBBMin()
	{
		return res->bb_min;
	}

	Math::Vector3 Mesh::Instance::GetBBMax()
	{
		return res->bb_max;
	}

	int Mesh::Instance::GetSubMeshesCount()
	{
		return (int)res->meshes.size();
	}

	Math::Vector3 Mesh::Instance::GetBBMin(int sub_mesh_index)
	{
		return res->meshes[sub_mesh_index].bb_min;
	}

	Math::Vector3 Mesh::Instance::GetBBMax(int sub_mesh_index)
	{
		return res->meshes[sub_mesh_index].bb_max;
	}

	void Mesh::Instance::Render(float dt)
	{
		Render(MeshPrograms::GetTranglPrg());
	}

	void Mesh::Instance::ShRender(float dt)
	{
		Render(MeshPrograms::GetShdTranglPrg());
	}

	void Mesh::Instance::Render(RenderTechnique* prg)
	{
		if (!visible)
		{
			return;
		}

		Math::Matrix trans;
		root.render.GetTransform(TransformStage::WrldViewProj, trans);

		root.render.GetDevice()->SetRenderTechnique(prg);

		if (transforms.size() == 0)
		{
			prg->SetMatrix(ShaderType::Vertex, "trans", &transform, 1);
		}

		prg->SetMatrix(ShaderType::Vertex, "view_proj", &trans, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&color, 1);

		root.render.GetDevice()->SetVertexDecl(res->vdecl);

		for (int i = 0; i < res->meshes.size(); i++)
		{
			SubMesh &mesh = res->meshes[i];

			if (!mesh.indices)
			{
				continue;
			}

			root.render.GetDevice()->SetVertexBuffer(0, mesh.vertices);
			root.render.GetDevice()->SetIndexBuffer(mesh.indices);

			if (transforms.size() > 0)
			{
				prg->SetMatrix(ShaderType::Vertex, "trans", &transforms[i], 1);
			}

			prg->SetTexture(ShaderType::Pixel, "diffuseMap", mesh.texture != -1 ? res->textures[mesh.texture] : nullptr);
			root.render.GetDevice()->DrawIndexed(PrimitiveTopology::TrianglesList, 0, 0, mesh.num_triangles);
		}
	}

	void Mesh::Instance::GetLocatorTransform(const char* name, Math::Matrix& loc_transform)
	{
		if (res->locators.count(name))
		{
			loc_transform = res->locators[name] * transform;
		}
		else
		{
			loc_transform = transform;
		}
	}

	void Mesh::Instance::Release()
	{
		taskPool->DelAllTasks(this);

		if (root.meshes.DecRef(res))
		{
			res->Release();
		}

		delete this;
	}

	bool Mesh::Load(const char* filename)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float2, ElementSemantic::Texcoord, 0 },{ ElementType::Float3, ElementSemantic::Texcoord, 1 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(3, desc, _FL_);

		return LoadFBX(filename);
	}

	bool Mesh::LoadFBX(const char* filename)
	{
		FileInMemory file;
		if (!file.Load(filename))
		{
			return false;
		}

		auto* scene = ofbx::load(file.GetData(), file.GetSize(), 1 << 0);

		if (!scene)
		{
			return false;
		}

		auto objects = scene->getAllObjects();

		for (int i = 0; i < scene->getAllObjectCount(); i++)
		{
			auto node = objects[i];
		
			if (node->getType() == ofbx::Object::Type::NULL_NODE)
			{
				auto fbx_mat = node->getGlobalTransform();
				Math::Matrix mat;

				for (int j = 0; j < 16; j++)
				{
					mat.matrix[j] = (float)fbx_mat.m[j];
				}

				Math::Matrix scale;
				scale.Scale(Math::Vector3(1.0f, 1.0f, -1.0f));

				mat *= scale;

				//FIX ME: Restore rotatio
				/*locators[node->name].Vx() = mat.Vx();
				locators[node->name].Vx().Normalize();

				locators[node->name].Vy() = mat.Vy();
				locators[node->name].Vy().Normalize();

				locators[node->name].Vz() = mat.Vz();
				locators[node->name].Vz().Normalize();*/

				locators[node->name].Pos() = mat.Pos();
			}
		}

		meshes.resize(scene->getMeshCount());
 
		char path[512];
		StringUtils::GetPath(filename, path);

		for (int i = 0; i<scene->getMeshCount(); i++)
		{
			auto* fbx_mesh = scene->getMesh(i);

			auto fbx_mat = fbx_mesh->getGlobalTransform();
			Math::Matrix mat;

			for (int j = 0; j < 16; j++)
			{
				mat.matrix[j] = (float)fbx_mat.m[j];
			}

			Math::Matrix scale;
			scale.Scale(Math::Vector3(1.0f, 1.0f, -1.0f));

			mat *= scale;

			auto& mesh = meshes[i];

			auto* geometry = fbx_mesh->getGeometry();

			if (fbx_mesh->getMaterialCount() > 0)
			{
				auto tex = fbx_mesh->getMaterial(0)->getTexture(ofbx::Texture::TextureType::DIFFUSE);

				if (tex)
				{
					auto texture_name = tex->getFileName();

					char name[512];
					int len = (int)(texture_name.end - texture_name.begin);
					memcpy(name, texture_name.begin, len);
					name[len] = 0;

					char fileName[512];
					StringUtils::GetFileName(name, fileName);

					StringUtils::Printf(name, 512, "%s%s", path, fileName);

					for (int i = 0; i < textures.size(); i++)
					{
						if (textures[i] && StringUtils::IsEqual(textures[i]->GetName(), name))
						{
							mesh.texture = i;
							break;
						}
					}

					if (mesh.texture == -1)
					{
						textures.push_back(root.render.LoadTexture(name, _FL_));
						mesh.texture = (int)textures.size() - 1;
					}
				}
			}

			mesh.num_triangles = geometry->getIndexCount() / 3;

			mesh.indices = root.render.GetDevice()->CreateBuffer(geometry->getIndexCount(), sizeof(uint32_t), _FL_);
			uint32_t* mesh_indices = (uint32_t*)mesh.indices->Lock();

			auto* indices = geometry->getFaceIndices();

			for (int j = 0; j < geometry->getIndexCount(); j++)
			{
				int index = indices[j];
				if (index < 0)
				{
					index = -index - 1;
				}

				mesh_indices[j] = index;
			}

			for (int j = 0; j < mesh.num_triangles; j++)
			{
				auto tmp = mesh_indices[j * 3];
				mesh_indices[j * 3] = mesh_indices[j * 3 + 1];
				mesh_indices[j * 3 + 1] = tmp;
			}

			mesh.indices->Unlock();

			auto kl = geometry->getVertexCount();

			mesh.vertices = root.render.GetDevice()->CreateBuffer(geometry->getVertexCount(), sizeof(MeshVertex), _FL_);
			MeshVertex* mesh_vertices = (MeshVertex*)mesh.vertices->Lock();

			auto* vertices = geometry->getVertices();
			auto* normals = geometry->getNormals();
			auto* uvs = geometry->getUVs();

			for (int j = 0; j < geometry->getVertexCount(); j++)
			{
				MeshVertex& vertex = mesh_vertices[j];
				auto& fbx_vertex = vertices[j];

				vertex.pos = Math::Vector3((float)fbx_vertex.x, (float)fbx_vertex.y, (float)fbx_vertex.z) * mat;

				if (normals)
				{
					auto& fbx_normal = normals[j];
					vertex.normal = Math::Vector3((float)fbx_normal.x, (float)fbx_normal.z, -(float)fbx_normal.y);
				}

				if (uvs)
				{
					auto& fbx_uv = uvs[j];
					vertex.uv = Math::Vector2((float)fbx_uv.x, 1.0f - (float)fbx_uv.y);
				}

				mesh.bb_max.Max(vertex.pos);
				mesh.bb_min.Min(vertex.pos);
			}

			bb_max.Max(mesh.bb_max);
			bb_min.Min(mesh.bb_min);

			mesh.vertices->Unlock();
		}

		scene->destroy();

		return true;
	}

	void Mesh::Release()
	{
		delete this;
	}
}