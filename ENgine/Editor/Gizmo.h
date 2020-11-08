
#pragma once

#include "Support/Transform.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	enum TransformType
	{
		Move = 1,
		Scale = 2,
		Rotate = 4,
		Anchorn = 8
	};

	class Gizmo
	{
		float scale = 1.0f;
		Math::Vector2 align2d = 0.0f;
		int transform2DActions = 0;
		Math::Vector2 pos2d = 0.0f;

		bool mousedPressed = false;

		bool ignore2DCamera = true;
		Math::Vector2 origin;
		Math::Vector2 movedOrigin;
		Math::Vector2 ancorns[8];

		int selAxis = -1;
		Math::Vector2 mousesDir = 0.0f;
		Math::Vector2 prevMouse = 0.0f;

		Math::Vector2 deltaMove = 0.0;

		Color CheckColor(int axis);
		void DrawAxis(int axis);
		void DrawCircle(int axis);
		bool CheckInersection(Math::Vector3 pos, Math::Vector3 pos2, Math::Vector2 ms,
		                      Math::Vector3 trans, bool check_trans,
		                      Math::Matrix view, Math::Matrix view_proj);

		void CheckSelectionTrans2D(Math::Vector2 ms);
		bool CheckSelectionTrans3D(int axis, Math::Vector2 ms);
		void CheckSelectionTrans3D(Math::Vector2 ms);

		void MoveTrans2D(Math::Vector2 ms);
		void MoveTrans3D(Math::Vector2 ms);

		void RenderTrans2D();
		void RenderTrans3D();

	public:

		Transform* transform = nullptr;
		TransformType mode = TransformType::Move;
		bool useLocalSpace = false;

		void SetTransform2D(Transform* transform, int actions = 0xffff, bool ignore_2d_camera = false);
		void SetTransform3D(Transform* transform);

		bool IsEnabled();
		void Disable();

		void OnMouseMove(Math::Vector2 ms);
		void OnLeftMouseDown();
		void OnLeftMouseUp();

		Math::Vector2 MakeAligned(Math::Vector2 pos);

		void Render();
	};
}