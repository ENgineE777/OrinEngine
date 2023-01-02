
#pragma once

#include <stdint.h>
#include "Defines.h"

/**
\ingroup gr_code_common
*/

namespace Orin
{
	struct CLASS_DECLSPEC FileLine
	{
		const char* file = nullptr;
		int line = 0;

		static FileLine* Alloc(const char* file, int line);
		static void Free(FileLine* fl);
	};

	template<class T>
	class PointerRef
	{
		T* ptr = nullptr;

		FileLine* fileLine = nullptr;

	public:

		PointerRef() = default;

		PointerRef(T* setPtr, const char* file, int line)
		{
			ptr = setPtr;

			if (ptr)
			{
				ptr->refCounter++;

				fileLine = FileLine::Alloc(file, line);
			}
		}

		PointerRef(const PointerRef& ref)
		{
			Copy(ref);
		}

		~PointerRef()
		{
			ReleaseRef();
		}

		operator T* () const
		{
			return ptr;
		}

		T* Get()
		{
			return ptr;
		}

		T* operator->() const
		{
			return ptr;
		}

		PointerRef& operator=(const PointerRef& ref)
		{
			Copy(ref);

			return *this; 
		}

		void Copy(const PointerRef& ref)
		{
			if (ptr == ref.ptr)
			{
				return;
			}

			ReleaseRef();

			ptr = ref.ptr;

			if (ptr)
			{
				ptr->refCounter++;
			}

			if (ref.fileLine)
			{
				fileLine = FileLine::Alloc(ref.fileLine->file, ref.fileLine->line);
			}
		}

		void ReleaseRef()
		{
			if (fileLine != nullptr)
			{
				FileLine::Free(fileLine);
				fileLine = nullptr;
			}

			if (ptr)
			{
				ptr->refCounter--;

				if (ptr->refCounter == 0)
				{
					ptr->Release();
				}

				ptr = nullptr;
			}
		}
	};
}