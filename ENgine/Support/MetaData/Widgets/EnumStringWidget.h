
#pragma once

#include "ProperyWidget.h"

namespace Oak
{
	class EnumStringWidget : public ProperyWidget
	{
	public:
		eastl::string* data;

		typedef void(*Callback)(void* object);
		Callback callback;

		EnumStringWidget(Callback callback);
		void Init(const char* catName, const char* labelName) override;
		void SetData(void* owner, void* set_data) override;
	};
}