
#include "MeshPrograms.h"

namespace Oak
{
	RenderTechniqueRef MeshPrograms::GetTranglPrg()
	{
		static RenderTechniqueRef prg;

		if (!prg.Get())
		{
			prg = root.render.GetRenderTechnique<TriangleProgram>(_FL_);
		}

		return prg;
	}

	RenderTechniqueRef MeshPrograms::GetShdTranglPrg()
	{
		static RenderTechniqueRef prg;

		if (!prg.Get())
		{
			prg = root.render.GetRenderTechnique<ShTriangleProgram>(_FL_);
		}

		return prg;
	}
}
