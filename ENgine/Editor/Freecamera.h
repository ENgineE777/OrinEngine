
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class FreeCamera
	{
	public:

		Math::Vector2 angles;
		Math::Vector3 pos;
		Math::Matrix view;
		Math::Matrix proj;

		Math::Vector2 pos2D;
		float zoom2D = 1.0f;

		bool mode2D = false;

		int alias_forward;
		int alias_strafe;
		int alias_fast;
		int alias_move2d_active;
		int alias_move2d_zoom;
		int alias_rotate_active;
		int alias_rotate_x;
		int alias_rotate_y;
		int alias_reset_view;

		float cur_speed = 0.0f;
		float prev_ed_zoom = 1.0f;

		void Init();

		void Update(float dt);

		void ClampZoom2D();
	};
}
