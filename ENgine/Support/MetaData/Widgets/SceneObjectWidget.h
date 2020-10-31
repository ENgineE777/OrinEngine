
#pragma once

#include "ProperyWidget.h"

namespace Oak
{
	class SceneObjectWidget : public ProperyWidget
	{
	public:

		//EUILabel* nameLabel;

		struct SceneObjectRef* ref = nullptr;

		void Init(const char* catName, const char* labelName) override;
		void SetData(void* owner, void* data) override;
		void SetObject(class SceneObject* object);
	};
}