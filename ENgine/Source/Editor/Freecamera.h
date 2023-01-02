
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_editor
*/

namespace Orin
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

		float rotationSensivity = 5.0f;
		float moveSpeed = 100.0f;
		float moveFastSpeed = 350.0f;
		float moveAcceleration = 50.0f;
		float moveDeacceleration = 100.0f;
		float zoom2Dsensivity = 5.0f;

		void Init();

		void Update(float dt, bool viewportFocused);

		void ClampZoom2D();
	};
}
