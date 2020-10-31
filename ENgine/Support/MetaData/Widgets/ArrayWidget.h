
#pragma once

#include "ProperyWidget.h"

namespace Oak
{
	class ArrayWidget : public ProperyWidget
	{
	public:

		int32_t pre_sel_item = -1;

		uint8_t* value;
		void* prop;
		void* root;
		int32_t* sel_item = nullptr;
		void Init(const char* catName, const char* labelName) override;
		void Show(bool show) override;
		void SetData(void* owner, void* set_data) override;
	};
}