
#pragma once

#include "Support/Transform.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class Gizmo
	{
	public:

		enum Trans2D
		{
			trans_2d_move    = 1,
			trans_2d_scale   = 2,
			trans_2d_rotate  = 4,
			trans_2d_anchorn = 8
		};

		static Gizmo* inst;

		int     mode = 1;
		bool    useLocalSpace = false;
		float   scale = 1.0f;
		Math::Vector2 align2d = 0.0f;

		struct Transform2D
		{
			Math::Vector2* pos = nullptr;
			Math::Vector2* axis = nullptr;
			float*   depth = nullptr;
			Math::Vector2* size = nullptr;
			float*   rotation = nullptr;
			Math::Vector2* offset = nullptr;
			Math::Matrix   mat_local;
			Math::Matrix* mat_parent;
			Math::Matrix   mat_global;

			Transform2D() = default;
			Transform2D(Math::Vector2* set_pos, Math::Vector2* set_size, Math::Vector2* set_offset = nullptr, float* set_depth = nullptr, float* set_rotation = nullptr, Math::Vector2* set_axis = nullptr, Math::Matrix* mat_parent = nullptr);
			Transform2D(Oak::Transform2D& trans);
			void BuildMatrices();
		};

		Transform2D trans2D;
		Math::Vector2 pos2d = 0.0f;

	private:

		Math::Matrix* transform;
		bool    mousedPressed = false;

		bool use_trans2D = false;
		int     trans2D_actions = 0;
		bool    ignore_2d_camera = true;
		Math::Vector2 size2d = 0.0f;
		Math::Vector2 origin;
		Math::Vector2 moved_origin;
		Math::Vector2 ancorns[8];

		bool    enabled = false;

		int     selAxis = -1;
		Math::Vector2 ms_dir = 0.0f;
		Math::Vector2 prev_ms = 0.0f;

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

		Math::Vector2 delta_move = 0.0;

		void Init();

		void SetTrans2D(Transform2D trans, int actions = 0xffff, bool ignore_2d_camera = false);
		void SetTrans2D(Math::Vector2 pos);
		void SetTrans3D(Math::Matrix* transform);
		bool IsTrans2D();
		bool IsEnabled();
		void Disable();

		void OnMouseMove(Math::Vector2 ms);
		void OnLeftMouseDown();
		void OnLeftMouseUp();

		Math::Vector2 MakeAligned(Math::Vector2 pos);
		void Render();
	};
}