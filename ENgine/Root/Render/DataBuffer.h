
#pragma once

#include "Support/Support.h"
#include "Support/PointerRef.h"

namespace Oak
{
	class DataBuffer
	{
		friend class PointerRef<DataBuffer>;

	protected:

		int size;
		int stride;
		int refCounter = 0;

	public:

		DataBuffer(int count, int strd) { size = count * strd; stride = strd; };
		virtual ~DataBuffer() {};

		virtual void* Lock() = 0;
		virtual void Unlock() = 0;

		int GetSize() { return size; };

		int GetStride() { return stride; };

		virtual void Release() = 0;
	};

	typedef PointerRef<DataBuffer> DataBufferRef;
}
