
#pragma once

#include "Support/Transform.h"
#include "EditorAction.h"

/**
\ingroup gr_code_editor
*/

namespace Orin
{
	enum class TransformMode
	{
		None,
		Move,
		Scale,
		Rotate,
		Rectangle
	};

	enum Axis : uint32_t
	{
		None = 0,
		X = 1,
		Y = 2,
		Z = 4,
		XY = X | Y,
		XZ = X | Z,
		YZ = Y | Z,
		XYZ = X | Y | Z
	};

	class Gizmo
	{
		class TransformAction : public IEditorAction
		{
			Transform* target = nullptr;
			Transform savedData;
			Transform data;

		public:

			TransformAction(void* owner, Transform* target, Transform& savedData);
			void Apply() override;
			void Undo() override;
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

		Math::Vector2 origin;
		Math::Vector3 movedOrigin;
		Math::Vector2 ancorns[8];

		Math::Vector3 trans2DProjection;
		Math::Vector3 trans2DPrevProjection;

		int selAxis = -1;
		bool mousedPressed = false;
		Math::Vector2 mousesDir = 0.0f;
		Math::Vector2 prevMouse = 0.0f;

		Math::Vector3 rectPos = 0.0f;

		Color CheckColor(Axis axis, bool ignoreSelection = false);
		void DrawAxis(AxisData& axis);
		void DrawCircle(Axis axis);
		bool CheckInersection(Math::Vector3 pos, Math::Vector3 pos2, Math::Vector2 ms,
		                      Math::Vector3 trans, bool check_trans,
		                      Math::Matrix view, Math::Matrix view_proj);

		void CheckSelectionTransRect(Math::Vector2 ms);
		bool CheckSelectionTrans3D(AxisData& axis, Math::Vector2 ms);
		void CheckSelectionTrans3D(Math::Vector2 ms);

		void MoveTransRect(Math::Vector2 ms);
		void MoveTrans3D(Math::Vector2 ms);

		void RenderTransRect();
		void RenderTrans3D();

		Math::Vector3 TransformVetcor(Math::Vector3 pos);

		void AxisTranslation(AxisData& axis);
		void PlaneTranslation(AxisData& axisData);
		void PlaneTranslation(Math::Vector3& plane_normal, Math::Vector3& point);

		Math::Vector3 GetGlobalPos();
		void SetGlobalPos(Math::Vector3 pos);
		bool IsValidMode();

	public:

		void* owner = nullptr;
		Transform* transform = nullptr;
		Transform* savedTransformPtr = nullptr;
		Transform savedTransform;
		TransformMode mode = TransformMode::Move;
		bool useLocalSpace = false;
		Math::Vector3 alignGrid = 8.0f;
		bool useAlignGrid = false;
		Axis alignGridFlag = Axis::XYZ;
		Math::Vector3 alignGridOffset = 8.0f;
		Axis alignOffsetGridFlag = Axis::XYZ;
		bool useAlignGridOffset = false;

		Gizmo();

		void SetTransform(void* owner, Transform* transform);

		bool IsEnabled();
		void Disable();

		void OnMouseMove(Math::Vector2 ms);
		void OnLeftMouseDown();
		void OnLeftMouseUp();

		Math::Vector3 AligneRectPos(Math::Vector3& pos);

		void Render();
	};
}