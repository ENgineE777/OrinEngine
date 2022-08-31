#include "PointerRef.h"
#include "Root/Root.h"

namespace Oak
{
	FileLine* FileLine::Alloc(const char* file, int line)
	{
		FileLine* fl = (FileLine*)root.memory.Alloc(sizeof(FileLine), file, line);

		fl->file = file;
		fl->line = line;

		return fl;
	}

	void FileLine::Free(FileLine* fl)
	{
		root.memory.Free(fl);
	}
}