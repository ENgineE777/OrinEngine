#include "FreeCamera.h"
#include "Root/Root.h"
#include "Support/Sprite.h"
#include "Editor.h"

namespace Oak
{
	void FreeCamera::Init()
	{
		proj.BuildProjection(45.0f * Math::Radian, 600.0f / 800.0f, 1.0f, 1000.0f);

		angles = Math::Vector2(0.0f, -0.1f);
		pos = Math::Vector3(0.0f, 6.0f, 0.0f);

		alias_forward = root.controls.GetAlias("FreeCamera.MOVE_FORWARD");
		alias_strafe = root.controls.GetAlias("FreeCamera.MOVE_STRAFE");
		alias_fast = root.controls.GetAlias("FreeCamera.MOVE_FAST");
		alias_move2d_active = root.controls.GetAlias("FreeCamera.MOVE2D_ACTIVE");
		alias_move2d_zoom = root.controls.GetAlias("FreeCamera.MOVE2D_ZOOM");
		alias_rotate_active = root.controls.GetAlias("FreeCamera.ROTATE_ACTIVE");
		alias_rotate_x = root.controls.GetAlias("FreeCamera.ROTATE_X");
		alias_rotate_y = root.controls.GetAlias("FreeCamera.ROTATE_Y");
		alias_reset_view = root.controls.GetAlias("FreeCamera.RESET_VIEW");
	}

	void FreeCamera::Update(float dt)
	{
		if (mode2D)
		{
			if (root.controls.GetAliasState(alias_move2d_active, AliasAction::Pressed))
			{
				float k = root.render.GetDevice()->GetHeight() / Sprite::pixelsHeight;
				pos2D.x -= root.controls.GetAliasValue(alias_rotate_x, true) / zoom2D / k;
				pos2D.y += root.controls.GetAliasValue(alias_rotate_y, true) / zoom2D / k;
			}

			if (editor.vireportHowered)
			{
				zoom2D += root.controls.GetAliasValue(alias_move2d_zoom, true) * (0.015f + rotationSensivity * 0.007f);
				ClampZoom2D();
			}
		}
		else
		{
			if (root.controls.GetAliasState(alias_rotate_active, AliasAction::Pressed))
			{
				float sensivity = 0.0025f + rotationSensivity * 0.00075f;
				angles.x -= root.controls.GetAliasValue(alias_rotate_x, true) * sensivity;
				angles.y -= root.controls.GetAliasValue(alias_rotate_y, true) * sensivity;

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

			float speed = (moveSpeed + moveFastSpeed * fast) * Math::Vector2(forward, strafe).Length();
			float accel = fabsf(speed) > 0.0f ? moveAcceleration : moveDeacceleration;

			if (cur_speed < speed)
			{
				cur_speed += dt * accel;

				if (cur_speed > speed)
				{
					cur_speed = speed;
				}
			}
			else
			{
				cur_speed -= dt * accel;

				if (cur_speed < speed)
				{
					cur_speed = speed;
				}
			}

			Math::Vector3 dir = Math::Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x));
			pos += dir * cur_speed * forward * dt;
	
			Math::Vector3 dir_strafe = Math::Vector3(dir.z, 0,-dir.x);
			pos += dir_strafe * cur_speed * strafe * dt;
		}

		if (mode2D)
		{
			float dist = (Sprite::pixelsHeight * 0.5f * Sprite::pixelsPerUnitInvert) / (tanf(22.5f * Math::Radian) * zoom2D);
			Math::Vector2 pos = pos2D * Sprite::pixelsPerUnitInvert;

			view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), Math::Vector3(0, 1, 0));
		}
		else
		{
			view.BuildView(pos, pos + Math::Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x)), Math::Vector3(0, 1, 0));
		}

		root.render.SetTransform(TransformStage::View, view);

		proj.BuildProjection(45.0f * Math::Radian, (float)root.render.GetDevice()->GetHeight() / (float)root.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
		root.render.SetTransform(TransformStage::Projection, proj);
	}

	void FreeCamera::ClampZoom2D()
	{
		zoom2D = Math::Clamp(zoom2D, 0.1f, 5.0f);
	}
}