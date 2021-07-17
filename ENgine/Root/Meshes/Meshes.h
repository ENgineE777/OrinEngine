
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include "Support/Support.h"
#include "MeshRes.h"

namespace Oak
{
	class CLASS_DECLSPEC Meshes
	{
		struct CLASS_DECLSPEC MeshRef
		{
			int count;
			Mesh* mesh;
		};

		eastl::map<eastl::string, MeshRef> meshes;

	public:

		void Init();
		Mesh::Instance* LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool);
		bool DecRef(Mesh* mesh);
	};
}
