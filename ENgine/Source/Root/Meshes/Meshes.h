
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include "Support/Support.h"
#include "IMeshes.h"

namespace Oak
{
	class Meshes : public IMeshes
	{
		struct MeshRef
		{
			int count;
			Mesh* mesh;
		};

		eastl::map<eastl::string, MeshRef> meshes;

	public:

		void Init();
		Mesh::Instance* LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool) override;
		bool DecRef(Mesh* mesh);
	};
}
