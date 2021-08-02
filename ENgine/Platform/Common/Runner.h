#pragma once

#include "IRunner.h"
#include "Root/Root.h"

/**
\ingroup gr_code_platform_common
*/

namespace Oak
{
	class Runner : public IRunner
	{
		friend class Root;

		HWND hwnd;

		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;	

	public:

		bool Init(HWND hwnd) override;
		void Update() override;
		void Render(float dt);
		void OnResize(int width, int height) override;

		void Release() override;
	};
}
