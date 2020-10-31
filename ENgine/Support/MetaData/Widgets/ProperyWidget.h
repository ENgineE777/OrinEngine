
#pragma once

#include "Support/Support.h"

namespace Oak
{
	class ProperyWidget
	{
	public:
		void* owner = nullptr;
		bool changed = false;

		virtual void Init(const char* catName, const char* labelName);
		virtual void SetData(void* owner, void* data) = 0;
		virtual void Show(bool show);
		virtual void Release();
	};
}