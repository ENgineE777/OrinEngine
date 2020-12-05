
#pragma once

#include "Support/Transform.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	enum class TransformType
	{
		Move = 1,
		Scale = 2,
		Rotate = 4,
		Anchorn = 8
	};

	class Gizmo
	{
		enum class Axis
		{
			X = 1,
			Y = 2,
			Z = 4,
			XY = X | Y,
			XZ = X | Z,
			YZ = Y | Z,
			XYZ = X | Y | Z
		};

		struct AxisData
		{
			Axis type;

			Math::Vector3 axis;

			Math::Vector3 from;
			Math::Vector3 to;

			Math::Vector3 subPointFrom;
			Math::Vector3 subPointLeft;
			Math::Vector3 subPointRight;

			Math::Vector3 subPointFrom2;
			Math::Vector3 subPointLeft2;
			Math::Vector3 subPointRight2;

			void ResetData()
			{
				from = 0.0f;
				to = 0.0f;

				subPointFrom = 0.0f;
				subPointLeft = 0.0f;
				subPointRight = 0.0f;

				subPointFrom2 = 0.0f;
				subPointLeft2 = 0.0f;
				subPointRight2 = 0.0f;
			};

			void AdjustData()
			{
				to += from;

				subPointFrom += from;
				subPointLeft += from;
				subPointRight += from;

				subPointFrom2 += from;
				subPointLeft2 += from;
				subPointRight2 += from;
			};
		};

		float scale = 1.0f;
		AxisData axises[3];
		Math::Vector3 mouseOrigin;
		Math::Vector3 mouseDirection;
		Math::Vector3 selectionOffset;

		int transform2DActions = 0;
		Math::Vector2 pos2d = 0.0f;

		bool ignore2DCamera = true;
		Math::Vector2 origin;
		Math::Vector2 movedOrigin;
		Math::Vector2 ancorns[8];

		int selAxis = -1;
		bool mousedPressed = false;
		Math::Vector2 mousesDir = 0.0f;
		Math::Vector2 prevMouse = 0.0f;

		Math::Vector2 deltaMove = 0.0;

		Color CheckColor(Axis axis, bool ignoreSelection = false);
		void DrawAxis(AxisData& axis);
		void DrawCircle(Axis axis);
		bool CheckInersection(Math::Vector3 pos, Math::Vector3 pos2, Math::Vector2 ms,
		                      Math::Vector3 trans, bool check_trans,
		                      Math::Matrix view, Math::Matrix view_proj);

		void CheckSelectionTrans2D(Math::Vector2 ms);
		bool CheckSelectionTrans3D(AxisData& axis, Math::Vector2 ms);
		void CheckSelectionTrans3D(Math::Vector2 ms);

		void MoveTrans2D(Math::Vector2 ms);
		void MoveTrans3D(Math::Vector2 ms);

		void RenderTrans2D();
		void RenderTrans3D();

		Math::Vector3 TransformVetcor(Math::Vector3 pos);

		void AxisTranslation(AxisData& axis);
		void PlaneTranslation(AxisData& axisData);
		void PlaneTranslation(Math::Vector3& plane_normal, Math::Vector3& point);

		void CaclLocalMatrix();

	public:

		Transform* transform = nullptr;
		TransformType mode = TransformType::Move;
		bool useLocalSpace = false;
		Math::Vector2 align2D = 0.0f;

		Gizmo();

		void SetTransform2D(Transform* transform, int actions = 0xffff, bool ignore_2d_camera = false);
		void SetTransform2D(Math::Vector2 set_pos);
		Math::Vector2 GetTransform2D();

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