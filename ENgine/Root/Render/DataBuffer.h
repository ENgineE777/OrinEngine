
#pragma once

#include "Support/Support.h"

namespace Oak
{
	class DataBuffer
	{
	protected:

		int size;
		int stride;

	public:

		DataBuffer(int count, int strd) { size = count * strd; stride = strd; };
		virtual ~DataBuffer() {};

		virtual void* Lock() = 0;
		virtual void Unlock() = 0;

		int GetSize() { return size; };

		int GetStride() { return stride; };

		virtual void Release() = 0;
	};
}
