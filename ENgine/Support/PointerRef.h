
#pragma once

#include <stdint.h>
#include "Defines.h"

namespace Oak
{
	template<class T>
	class PointerRef
	{
		T* ptr = nullptr;

		struct FileLine
		{
			const char* file = nullptr;
			int line = 0;

			FileLine(const char* setFile, int setLine)
			{
				file = setFile;
				line = setLine;
			}
		};

		FileLine* fileLine = nullptr;

	public:

		PointerRef() = default;

		PointerRef(T* setPtr, const char* file, int line)
		{
			ptr = setPtr;
			ptr->refCounter++;

			fileLine = new(file, line) FileLine(file, line);
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
				fileLine = new(ref.fileLine->file, ref.fileLine->line) FileLine(ref.fileLine->file, ref.fileLine->line);
			}
		}

		void ReleaseRef()
		{
			DELETE_PTR(fileLine)

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