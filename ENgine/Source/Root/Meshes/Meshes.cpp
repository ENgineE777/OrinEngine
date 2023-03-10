
#include "Root/Root.h"

namespace Orin
{
	void Meshes::Init()
	{
	}

	Mesh::Instance* Meshes::LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool)
	{
		Mesh* mesh = nullptr;

		if (meshes.count(name) > 0)
		{
			MeshRef& ref = meshes[name];

			ref.count++;
			mesh = ref.mesh;
		}
		else
		{
			mesh = new Mesh();

			if (!mesh->Load(name))
			{
				delete mesh;
				return nullptr;
			}

			MeshRef& ref = meshes[name];

			ref.count = 1;
			ref.mesh = mesh;
		}

		Mesh::Instance* instance = new Mesh::Instance();
		instance->Init(mesh, taskPool);

		return instance;
	}

	bool Meshes::DecRef(Mesh* mesh)
	{
		typedef eastl::map<eastl::string, MeshRef>::iterator it_type;

		for (it_type iterator = meshes.begin(); iterator != meshes.end(); iterator++)
		{
			if (iterator->second.mesh == mesh)
			{
				iterator->second.count--;

				if (iterator->second.count == 0)
				{
					meshes.erase(iterator);
					return true;
				}

				return false;
			}
		}

		return true;
	}
}