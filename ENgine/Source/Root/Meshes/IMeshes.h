
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include "Support/Support.h"
#include "MeshRes.h"

namespace Oak
{
	class CLASS_DECLSPEC IMeshes
	{
	public:

		virtual Mesh::Instance* LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool) = 0;
	};
}
