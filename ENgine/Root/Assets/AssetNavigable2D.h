#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class Scene;

	class CLASS_DECLSPEC AssetNavigable2D : public Asset
	{
		bool dargField = false;
		float camZoom = 1.0f;
		Math::Vector2 prevMs;
		Math::Vector2 camPos;
		Math::Vector2 mousePos;

	public:

		virtual void Draw(float dt);

		void OnMouseMove(Math::Vector2 ms) override;
		void OnMiddleMouseDown() override;
		void OnMiddleMouseUp() override;
	};
}