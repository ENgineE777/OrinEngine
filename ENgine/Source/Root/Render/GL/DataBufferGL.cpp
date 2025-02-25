
#include "DataBufferGL.h"

namespace Orin
{
	DataBufferGL::DataBufferGL(int sz, int strd) : DataBuffer(sz, strd)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	}

	void* DataBufferGL::Lock()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		return (void*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT);
	}

	void DataBufferGL::Unlock()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void DataBufferGL::Release()
	{
		glDeleteBuffers(1, &buffer);
		delete this;
	}
}