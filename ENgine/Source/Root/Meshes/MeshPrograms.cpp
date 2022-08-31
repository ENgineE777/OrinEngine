
#include "MeshPrograms.h"

namespace Oak
{
	
	CLASSREGEX(Program, TriangleProgram, MeshPrograms::TriangleProgram, "TriangleProgram")
	CLASSREGEX_END(Program, TriangleProgram)
	CLASSREGEX(Program, ShTriangleProgram, MeshPrograms::ShTriangleProgram, "ShTriangleProgram")
	CLASSREGEX_END(Program, ShTriangleProgram)
	/*CLASSREGEX(Program, ColorProgram, MeshPrograms::ColorProgram, "ColorProgram")
	CLASSREGEX_END(Program, ColorProgram)
	CLASSREGEX(Program, BlurProgram, MeshPrograms::BlurProgram, "BlurProgram")
	CLASSREGEX_END(Program, BlurProgram)
	CLASSREGEX(Program, CombineProgram, MeshPrograms::CombineProgram, "CombineProgram")
	CLASSREGEX_END(Program, CombineProgram)
	CLASSREGEX(Program, QuadProgramDepth, MeshPrograms::QuadProgramDepth, "QuadProgramDepth")
	CLASSREGEX_END(Program, QuadProgramDepth)
	CLASSREGEX(Program, QuadProgramNoDepth, MeshPrograms::QuadProgramNoDepth, "QuadProgramNoDepth")
	CLASSREGEX_END(Program, QuadProgramNoDepth)*/

	Program* MeshPrograms::GetTranglPrg()
	{
		static ProgramRef prg;

		if (!prg.Get())
		{
			prg = root.render.GetProgram("TriangleProgram", _FL_);
		}

		return prg;
	}

	Program* MeshPrograms::GetShdTranglPrg()
	{
		static ProgramRef prg;

		if (!prg.Get())
		{
			prg = root.render.GetProgram("ShTriangleProgram", _FL_);
		}

		return prg;
	}
}
