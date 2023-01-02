
#pragma once

#include "Support/Support.h"
#include "Support/PointerRef.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief DataBuffer

	Data buffer which can store gemotry, indices, constants.

	*/

	class CLASS_DECLSPEC DataBuffer
	{
		friend class PointerRef<DataBuffer>;

	protected:

		#ifndef DOXYGEN_SKIP
		int size;
		int stride;
		int refCounter = 0;
		virtual void Release() = 0;

	public:

		DataBuffer(int count, int strd) { size = count * strd; stride = strd; };
		virtual ~DataBuffer() {};
		#endif

		/**
		\brief Lock buffer of a writing

		\return Pointer for a writing data into buffer
		*/
		virtual void* Lock() = 0;

		/**
		\brief Unlock buffer after writing
		*/
		virtual void Unlock() = 0;

		/**
		\brief Get size in bytes

		\return size in bytes
		*/
		int GetSize() { return size; };

		/**
		\brief Get stride

		\return Stride
		*/
		int GetStride() { return stride; };
	};

	typedef PointerRef<DataBuffer> DataBufferRef;
}
