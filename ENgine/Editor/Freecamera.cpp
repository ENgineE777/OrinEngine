#include "FreeCamera.h"
#include "Root/Root.h"

namespace Oak
{
	void FreeCamera::Init()
	{
		proj.BuildProjection(45.0f * Math::Radian, 600.0f / 800.0f, 1.0f, 1000.0f);

		angles = Math::Vector2(0.0f, -0.5f);
		pos = Math::Vector3(0.0f, 6.0f, 0.0f);

		alias_forward = root.controls.GetAlias("FreeCamera.MOVE_FORWARD");
		alias_strafe = root.controls.GetAlias("FreeCamera.MOVE_STRAFE");
		alias_fast = root.controls.GetAlias("FreeCamera.MOVE_FAST");
		alias_rotate_active = root.controls.GetAlias("FreeCamera.ROTATE_ACTIVE");
		alias_rotate_x = root.controls.GetAlias("FreeCamera.ROTATE_X");
		alias_rotate_y = root.controls.GetAlias("FreeCamera.ROTATE_Y");
	}

	void FreeCamera::Update(float dt)
	{
		if (root.controls.GetAliasState(alias_rotate_active, Controls::AliasAction::Active))
		{
			angles.x -= root.controls.GetAliasValue(alias_rotate_x, true) * 0.01f;
			angles.y -= root.controls.GetAliasValue(alias_rotate_y, true) * 0.01f;

			if (angles.y > Math::HalfPI)
			{
				angles.y = Math::HalfPI;
			}

			if (angles.y < -Math::HalfPI)
			{
				angles.y = -Math::HalfPI;
			}
		}

		float forward = root.controls.GetAliasValue(alias_forward, false);
		float strafe = root.controls.GetAliasValue(alias_strafe, false);
		float fast = root.controls.GetAliasValue(alias_fast, false);

		float speed = (100.0f + 350.0f * fast) * dt;

		Math::Vector3 dir = Math::Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x));
		pos += dir * speed * forward;
	
		Math::Vector3 dir_strafe = Math::Vector3(dir.z, 0,-dir.x);
		pos += dir_strafe * speed * strafe;

		view.BuildView(pos, pos + Math::Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x)), Math::Vector3(0, 1, 0));

		root.render.SetTransform(Render::View, view);

		proj.BuildProjection(45.0f * Math::Radian, (float)root.render.GetDevice()->GetHeight() / (float)root.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
		root.render.SetTransform(Render::Projection, proj);
	}
}