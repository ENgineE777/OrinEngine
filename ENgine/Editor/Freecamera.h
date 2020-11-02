
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
		Math::Vector3  pos;
		Math::Matrix  view;
		Math::Matrix  proj;

		int alias_forward;
		int alias_strafe;
		int alias_fast;
		int alias_rotate_active;
		int alias_rotate_x;
		int alias_rotate_y;

		float prev_ed_zoom = 1.0f;

		void Init();

		void Update(float dt);
	};
}
