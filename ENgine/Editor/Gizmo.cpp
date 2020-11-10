
#include "Gizmo.h"
#include "Root/Root.h"
#include "Support/Sprite.h"
#include "EditorDrawer.h"

namespace Oak
{
	Gizmo::Gizmo()
	{
		axises[0].type = Axis::X;
		axises[1].type = Axis::Y;
		axises[2].type = Axis::Z;
	}

	void Gizmo::SetTransform2D(Transform* set_transform, int actions, bool ignore_2d_camera)
	{
		OAK_ASSERT(set_transform != nullptr, "Gizmo::set_transform2d == null")
		OAK_ASSERT(set_transform->Is2D(), "Gizmo::set_transform2d not 2D")

		transform = set_transform;

		pos2d = ((Transform2D*)transform)->pos;

		transform2DActions = actions;
		ignore2DCamera = ignore_2d_camera;

		deltaMove = 0.0f;
	}

	void Gizmo::SetTransform2D(Math::Vector2 set_pos)
	{
		if (!transform)
		{
			return;
		}

		Transform2D* transform2D = (Transform2D*)transform;

		set_pos = MakeAligned(set_pos);
		transform2D->pos = set_pos;
		pos2d = set_pos;
	}

	Math::Vector2 Gizmo::GetTransform2D()
	{
		if (!transform)
		{
			return 0.0f;
		}

		Transform2D* transform2D = (Transform2D*)transform;
		return transform2D->pos;
	}

	void Gizmo::SetTransform3D(Transform* set_transform)
	{
		OAK_ASSERT(set_transform != nullptr, "Gizmo::set_transform3d == null")
		OAK_ASSERT(!set_transform->Is2D(), "Gizmo::set_transform3d not 3D")

		transform = set_transform;
	}

	bool Gizmo::IsEnabled()
	{
		return transform != nullptr;
	}

	void Gizmo::Disable()
	{
		transform = nullptr;
	}

	Color Gizmo::CheckColor(Axis axis, bool ignoreSelection)
	{
		Color color;

		if (!ignoreSelection && selAxis != -1 && (int)axis & selAxis)
		{
			color = COLOR_YELLOW;
		}
		else
		{
			if (axis == Axis::X)
			{
				color = COLOR_RED;
			}
			else
			if (axis == Axis::Y)
			{
				color = COLOR_GREEN;
			}
			else
			if (axis == Axis::Z)
			{
				color = COLOR_BLUE;
			}
		}

		return color;
	}

	void Gizmo::DrawAxis(AxisData& axis)
	{
		axis.ResetData();

		axis.from = transform->local.Pos();

		Color color = CheckColor(axis.type);

		if (mode == TransformType::Move)
		{
			float subScale = scale * 0.4f;

			if (axis.type == Axis::X)
			{
				axis.to.x = scale;

				axis.subPointFrom.x = subScale;

				axis.subPointLeft.x = axis.subPointFrom.x;
				axis.subPointLeft.y = subScale;

				axis.subPointRight.x = axis.subPointFrom.x;
				axis.subPointRight.z = subScale;
			}
			else
			if (axis.type == Axis::Y)
			{
				axis.to.y = scale;

				axis.subPointFrom.y = subScale;

				axis.subPointLeft.y = axis.subPointFrom.y;
				axis.subPointLeft.x = subScale;

				axis.subPointRight.y = axis.subPointFrom.y;
				axis.subPointRight.z = subScale;
			}
			else
			if (axis.type == Axis::Z)
			{
				axis.to.z = scale;

				axis.subPointFrom.z = subScale;

				axis.subPointLeft.z = axis.subPointFrom.z;
				axis.subPointLeft.y = subScale;

				axis.subPointRight.z = axis.subPointFrom.z;
				axis.subPointRight.x = subScale;
			}
		}
		else
		if (mode == TransformType::Scale)
		{
			float subScale = scale * 0.625f;
			float subScale2 = scale * 0.4f;

			if (axis.type == Axis::X)
			{
				axis.to.x = scale;

				axis.subPointFrom.x = subScale;

				axis.subPointLeft.x = subScale * 0.5f;
				axis.subPointLeft.y = subScale * 0.5f;

				axis.subPointRight.x = subScale * 0.5f;
				axis.subPointRight.z = subScale * 0.5f;

				axis.subPointFrom2.x = subScale2;

				axis.subPointLeft2.x = subScale2 * 0.5f;
				axis.subPointLeft2.y = subScale2 * 0.5f;

				axis.subPointRight2.x = subScale2 * 0.5f;
				axis.subPointRight2.z = subScale2 * 0.5f;
			}
			else
			if (axis.type == Axis::Y)
			{
				axis.to.y = scale;

				axis.subPointFrom.y = subScale;

				axis.subPointLeft.y = subScale * 0.5f;
				axis.subPointLeft.x = subScale * 0.5f;

				axis.subPointRight.y = subScale * 0.5f;
				axis.subPointRight.z = subScale * 0.5f;

				axis.subPointFrom2.y = subScale2;

				axis.subPointLeft2.y = subScale2 * 0.5f;
				axis.subPointLeft2.x = subScale2 * 0.5f;

				axis.subPointRight2.y = subScale2 * 0.5f;
				axis.subPointRight2.z = subScale2 * 0.5f;
			}
			else
			if (axis.type == Axis::Z)
			{
				axis.to.z = scale;

				axis.subPointFrom.z = subScale;

				axis.subPointLeft.z = subScale * 0.5f;
				axis.subPointLeft.y = subScale * 0.5f;

				axis.subPointRight.z = subScale * 0.5f;
				axis.subPointRight.x = subScale * 0.5f;

				axis.subPointFrom2.z = subScale2;

				axis.subPointLeft2.z = subScale2 * 0.5f;
				axis.subPointLeft2.y = subScale2 * 0.5f;

				axis.subPointRight2.z = subScale2 * 0.5f;
				axis.subPointRight2.x = subScale2 * 0.5f;
			}
		}

		if (useLocalSpace || mode == TransformType::Scale)
		{
			axis.to = TransformVetcor(axis.to);

			axis.subPointFrom = TransformVetcor(axis.subPointFrom);
			axis.subPointLeft = TransformVetcor(axis.subPointLeft);
			axis.subPointRight = TransformVetcor(axis.subPointRight);
			
			axis.subPointFrom2 = TransformVetcor(axis.subPointFrom2);
			axis.subPointLeft2 = TransformVetcor(axis.subPointLeft2);
			axis.subPointRight2 = TransformVetcor(axis.subPointRight2);
		}

		axis.AdjustData();

		root.render.DebugLine(axis.from, color, axis.to, color, false);

		bool needHighlight = ((axis.type == Axis::X && (selAxis == (int)Axis::XY || selAxis == (int)Axis::XYZ)) ||
		                      (axis.type == Axis::Y && (selAxis == (int)Axis::XY || selAxis == (int)Axis::XYZ)) ||
		                      (axis.type == Axis::Z && (selAxis == (int)Axis::YZ || selAxis == (int)Axis::XYZ)));

		color = needHighlight ? COLOR_YELLOW : CheckColor(axis.type, true);

		root.render.DebugLine(axis.subPointLeft, color, axis.subPointFrom, color, false);

		if (mode == TransformType::Scale)
		{
			root.render.DebugLine(axis.subPointLeft2, color, axis.subPointFrom2, needHighlight ? color : CheckColor(axis.type, true), false);
		}

		needHighlight = ((axis.type == Axis::X && (selAxis == (int)Axis::XZ || selAxis == (int)Axis::XYZ)) ||
		                 (axis.type == Axis::Y && (selAxis == (int)Axis::YZ || selAxis == (int)Axis::XYZ)) ||
		                 (axis.type == Axis::Z && (selAxis == (int)Axis::XZ || selAxis == (int)Axis::XYZ)));

		color = needHighlight ? COLOR_YELLOW : CheckColor(axis.type, true);

		root.render.DebugLine(axis.subPointRight, color, axis.subPointFrom , color, false);

		if (mode == TransformType::Scale)
		{
			root.render.DebugLine(axis.subPointRight2, color, axis.subPointFrom2, needHighlight ? color : CheckColor(axis.type, true), false);
		}

		color = CheckColor(axis.type, true);

		float hgt = 0.15f * scale;
		float r = 0.035f * scale;
		int nums = 64;

		for (int i = 0; i < nums; i++)
		{
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
			float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

			Math::Vector3 pos;

			if (axis.type == Axis::X)
			{
				pos = Math::Vector3(hgt, dx, dz);
			}
			else
			if (axis.type == Axis::Y)
			{
				pos = Math::Vector3(dx, hgt, dz);
			}
			else
			if (axis.type == Axis::Z)
			{
				pos = Math::Vector3(dx, dz, hgt);
			}

			if (useLocalSpace || mode == TransformType::Scale)
			{
				pos = TransformVetcor(pos);
			}

			root.render.DebugLine(axis.to, color, axis.to - pos, color, false);
		}
	}

	void Gizmo::DrawCircle(Axis axis)
	{
		Color color = CheckColor(axis);
		Color color_gray = Color(color.r * 0.4f, color.g * 0.4f, color.b * 0.4f);

		Math::Matrix mat;
		if (useLocalSpace)
		{
			mat = transform->local;
		}
		else
		{
			mat.Pos() = transform->local.Pos();
		}

		mat.RemoveScale();

		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Vector3 tr = mat.Pos();
		Math::Vector3 trans = tr * view;

		float last_dx = -scale * 2;
		float last_dz = -scale * 2;
		int nums = 32;

		for (int i = 0; i < nums+1; i++)
		{
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * scale;
			float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * scale;

			if (last_dx > -scale * 1.5f)
			{
				Math::Vector3 pos;
				Math::Vector3 pos2;

				if (axis == Axis::X)
				{
					pos.Set(0.0f, last_dx, last_dz);
					pos2.Set(0.0f, dx, dz);
				}
				else
				if (axis == Axis::Y)
				{
					pos.Set(last_dx, 0.0f, last_dz);
					pos2.Set(dx, 0.0f, dz);
				}
				else
				{
					pos.Set(last_dx, last_dz, 0.0f);
					pos2.Set(dx, dz, 0.0f);
				}

				pos = mat.MulNormal(pos);
				pos2 = mat.MulNormal(pos2);

				pos += tr;
				pos2 += tr;

				Math::Vector3 pos_post = pos * view;
				Math::Vector3 pos2_post = pos2 * view;

				if (pos_post.z > trans.z && pos2_post.z > trans.z)
				{
					root.render.DebugLine(pos, color_gray, pos2, color_gray, false);
				}
				else
				{
					root.render.DebugLine(pos, color, pos2, color, false);
				}
			}

			last_dx = dx;
			last_dz = dz;
		}
	}

	bool Gizmo::CheckInersection(Math::Vector3 pos, Math::Vector3 pos2, Math::Vector2 ms,
								 Math::Vector3 trans, bool check_trans,
								 Math::Matrix view, Math::Matrix view_proj)
	{
		if (useLocalSpace || mode == TransformType::Scale)
		{
			pos = TransformVetcor(pos);
			pos2 = TransformVetcor(pos2);
		}

		Math::Vector3 tr = transform->local.Pos();

		pos += tr;
		pos2 += tr;

		Math::Vector3 pos_post = pos * view;
		Math::Vector3 pos2_post = pos2 * view;

		bool proceed = true;
		if (check_trans)
		{
			if (pos_post.z > trans.z && pos2_post.z > trans.z)
			{
				proceed = false;
			}
		}

		if (proceed)
		{
			pos_post = root.render.TransformToScreen(pos, 1);
			pos2_post = root.render.TransformToScreen(pos2, 1);
		
			float x1 = fminf(pos_post.x , pos2_post.x );
			float x2 = fmaxf(pos_post.x , pos2_post.x );

			float y1 = fmaxf(pos_post.y , pos2_post.y );
			float y2 = fminf(pos_post.y , pos2_post.y );

			float sz = 0.025f;
			x1 += -sz;
			x2 +=  sz;
			y1 += -sz;
			y2 +=  sz;

			if (x1 < ms.x && ms.x < x2 &&
				y1 < ms.y && ms.y < y2)
			{
				mousesDir = Math::Vector2(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y);
				mousesDir.Normalize();

				return true;
			}
		}

		return false;
	}

	void Gizmo::CheckSelectionTrans2D(Math::Vector2 ms)
	{
		Transform2D* transform2D = (Transform2D*)transform;

		if (transform2DActions & (int)TransformType::Scale)
		{
			for (int i = 0; i < 8; i++)
			{
				if (ancorns[i].x - 7 < ms.x && ms.x < ancorns[i].x + 7 &&
					ancorns[i].y - 7 < ms.y && ms.y < ancorns[i].y + 7)
				{
					selAxis = i + 1;
					break;
				}
			}
		}

		if (transform2DActions & (int)TransformType::Anchorn)
		{
			if (origin.x - 7 < ms.x && ms.x < origin.x + 7 &&
				origin.y - 7 < ms.y && ms.y < origin.y + 7)
			{
				selAxis = 10;
				movedOrigin = origin;
			}
		}

		if (selAxis == -1)
		{
			if (Math::IsInsideTriangle(ms, ancorns[0], ancorns[1], ancorns[2]) ||
				Math::IsInsideTriangle(ms, ancorns[0], ancorns[2], ancorns[3]))
			{
				//SetCursor(cr_move);

				if (transform2DActions & (int)TransformType::Move)
				{
					selAxis = 0;
				}
			}
			else
			{
				//SetCursor(cr_rotate);

				if (transform2DActions & (int)TransformType::Rotate)
				{
					selAxis = 9;
				}
			}
		}
	}

	bool Gizmo::CheckSelectionTrans3D(AxisData& axis, Math::Vector2 ms)
	{
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);
		view_proj = view * view_proj;

		if (mode == TransformType::Move || mode == TransformType::Scale)
		{
			Math::Vector3 dir;

			if (axis.type == Axis::X)
			{
				dir.x = scale;
			}
			else
			if (axis.type == Axis::Y)
			{
				dir.y = scale;
			}
			else
			if (axis.type == Axis::Z)
			{
				dir.z = scale;
			}

			int count = 7;

			for (int i=0; i<count; i++)
			{
				Math::Vector3 pos = dir * (float)i * (1.0f / (float)count);
				Math::Vector3 pos2 = dir * (float)(i+1) * (1.0f / (float)count);
				Math::Vector3 tr(0.0f);

				if (CheckInersection(pos, pos2, ms, tr, false, view, view_proj))
				{
					return true;
				}
			}
		}
		else
		if (mode == TransformType::Rotate)
		{
			Math::Vector3 trans = transform->local.Pos() * view;

			float r = scale;
			float last_dx = -r * 2;
			float last_dz = -r * 2;
			int nums = 32;

			for (int i = 0; i < nums + 1; i++)
			{
				float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
				float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

				if (last_dx > -r * 1.5f)
				{
					Math::Vector3 pos;
					Math::Vector3 pos2;

					if (axis.type == Axis::X)
					{
						pos = Math::Vector3(0.0f, last_dx, last_dz);
						pos2 = Math::Vector3(0.0f, dx, dz);
					}
					else
					if (axis.type == Axis::Y)
					{
						pos = Math::Vector3(last_dx, 0.0f, last_dz);
						pos2 = Math::Vector3(dx, 0.0f, dz);
					}
					else
					{
						pos = Math::Vector3(last_dx, last_dz, 0.0f);
						pos2 = Math::Vector3(dx, dz, 0.0f);
					}

					if (CheckInersection(pos, pos2, ms, trans, true, view, view_proj))
					{
						return true;
					}
				}

				last_dx = dx;
				last_dz = dz;
			}
		}

		return false;
	}

	void Gizmo::CheckSelectionTrans3D(Math::Vector2 ms)
	{
		ms.x /= (float)root.render.GetDevice()->GetWidth();
		ms.y /= (float)root.render.GetDevice()->GetHeight();
 
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);

		Math::Vector3 v;
		v.x = (2.0f * ms.x - 1) / view_proj._11;
		v.y = -(2.0f * ms.y - 1) / view_proj._22;
		v.z = 1.0f;

		Math::Matrix inv_view = view;
		inv_view.Inverse();
		mouseOrigin = inv_view.Pos();

		mouseDirection.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		mouseDirection.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		mouseDirection.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		mouseDirection.Normalize();

		for (int i = 0; i < 3; i++)
		{
			if (CheckSelectionTrans3D(axises[i], ms))
			{
				selAxis = (int)axises[i].type;
			}
		}

		if (mode == TransformType::Move)
		{
			if (Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::XZ;
			}

			if (Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::XY;
			}

			if (Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::YZ;
			}
		}

		if (mode == TransformType::Scale)
		{
			if (Math::IntersectTrianglrRay(axises[0].subPointFrom2, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[0].subPointRight2, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].subPointFrom2, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].subPointRight2, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::XZ;
			}

			if (Math::IntersectTrianglrRay(axises[0].subPointFrom2, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[0].subPointLeft2, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].subPointFrom2, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].subPointLeft2, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::XY;
			}

			if (Math::IntersectTrianglrRay(axises[1].subPointFrom2, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].subPointRight2, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].subPointFrom2, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].subPointLeft2, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::YZ;
			}

			if (Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom2, axises[0].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom2, axises[2].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom2, axises[0].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom2, axises[1].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom2, axises[1].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom2, axises[2].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f))
			{
				selAxis = (int)Axis::XYZ;
			}
		}
	}

	void Gizmo::MoveTrans2D(Math::Vector2 ms)
	{
		Transform2D* transform2D = (Transform2D*)transform;

		ms *= (1024.0f / root.render.GetDevice()->GetHeight()) / Sprite::edCamZoom;

		if (ms.Length() < 0.001f)
		{
			return;
		}

		if (selAxis == 0)
		{
			pos2d.x += (transform2D->axis.x > 0.0f) ? ms.x : -ms.x;
			pos2d.y += (transform2D->axis.y > 0.0f) ? ms.y : -ms.y;

			Math::Vector2 prev_pos = transform2D->pos;
			transform2D->pos = MakeAligned(pos2d);
			deltaMove += transform2D->pos - prev_pos;
		}
		else
		if (selAxis == 9)
		{
			if (!root.controls.DebugKeyPressed("KEY_Z", AliasAction::Active))
			{
				return;
			}

			Math::Vector2 p1 = prevMouse + ms - origin;
			p1.Normalize();

			Math::Vector2 p2 = prevMouse - origin;
			p2.Normalize();

			float k = p1.x * p2.x + p1.y * p2.y;

			if (fabs(k) < 1.0f)
			{
				k = acosf(k);

				Math::Vector2 p3(-p2.y, p2.x);

				float k2 = p1.x * p3.x + p1.y * p3.y;

				if (k2 < 0)
				{
					k *= -1.0f;
				}

				transform2D->rotation += k;
			}
		}
		else
		if (selAxis == 10)
		{
			movedOrigin += ms * Sprite::screenMul;
		}
		else
		if (selAxis > 0)
		{
			float dist = ms.Length();

			ms.Normalize();

			float k1 = 1.0f;
			float k2 = 1.0f;

			if (selAxis == 5 || selAxis == 7)
			{
				k1 = 0.0f;
			}

			if (selAxis == 1 || selAxis == 2 || selAxis == 5)
			{
				k2 = -1.0f;
			}

			if (selAxis == 0 || selAxis== 1 || selAxis == 4 || selAxis == 8)
			{
				k1 = -1.0f;
			}

			if (selAxis == 6 || selAxis == 8)
			{
				k2 = 0.0f;
			}

			float dot1 = 0.0f;
			float dot2 = 0.0f;

			dot1 = ms.x * transform2D->global.Vx().x + ms.y * transform2D->global.Vx().y;
			dot2 = ms.x * transform2D->global.Vy().x + ms.y * transform2D->global.Vy().y;

			float delta = dist * k1 * dot1;

			transform2D->size.x += delta;

			if (selAxis == 1 || selAxis == 4 || selAxis == 8)
			{
				pos2d.x -= (1.0f - transform2D->offset.x) * delta;
			}
			else
			if (selAxis == 2 || selAxis == 3 || selAxis == 6)
			{
				pos2d.x += transform2D->offset.x * delta;
			}

			delta = dist * k2 * dot2;

			transform2D->size.y += delta;

			if (selAxis == 1 || selAxis == 2 || selAxis == 5)
			{
				pos2d.y -= delta * (1.0f - transform2D->offset.y);
			}
			else
			if (selAxis == 3 || selAxis == 4 || selAxis == 7)
			{
				pos2d.y += delta * transform2D->offset.y;
			}

			Math::Vector2 prev_pos = transform2D->pos;
			transform2D->pos = pos2d;

			deltaMove += transform2D->pos - prev_pos;
		}
	}

	void Gizmo::MoveTrans3D(Math::Vector2 ms)
	{
		if (selAxis == -1)
		{
			return;
		}

		Math::Vector2 cur_dir(ms.x / (float)root.render.GetDevice()->GetWidth(), ms.y / (float)root.render.GetDevice()->GetHeight());
		float da = cur_dir.Length();
		cur_dir.Normalize();

		da *= cur_dir.Dot(mousesDir);

		if (mode == TransformType::Move)
		{
			da *= scale * 16;

			if (selAxis & (int)Axis::X)
			{
				transform->local._41 += da;
			}

			if (selAxis & (int)Axis::Y)
			{
				transform->local._42 += da;
			}

			if (selAxis & (int)Axis::Z)
			{
				transform->local._43 += da;
			}
		}
		else
		if (mode == TransformType::Rotate)
		{
			da *= -5.0f;

			Math::Matrix rot;

			if (selAxis == (int)Axis::X)
			{
				rot.RotateX(da);
			}
			else
			if (selAxis == (int)Axis::Y)
			{
				rot.RotateY(-da);
			}
			else
			{
				rot.RotateZ(da);
			}

			if (useLocalSpace)
			{
				transform->local = rot * transform->local;
			}
			else
			{
				Math::Vector3 tr = transform->local.Pos();
				transform->local = transform->local * rot;
				transform->local.Pos() = tr;
			}
		}
		else
		if (mode == TransformType::Scale)
		{
			da *= scale * 16;

			if (selAxis & (int)Axis::X)
			{
				float length = transform->local.Vx().Normalize();
				length += da;
				transform->local.Vx() *= fmaxf(length, 0.1f);
			}

			if (selAxis & (int)Axis::Y)
			{
				float length = transform->local.Vy().Normalize();
				length += da;
				transform->local.Vy() *= fmaxf(length, 0.1f);
			}

			if (selAxis & (int)Axis::Z)
			{
				float length = transform->local.Vz().Normalize();
				length += da;
				transform->local.Vz() *= fmaxf(length, 0.1f);
			}
		}
	}

	void Gizmo::RenderTrans2D()
	{
		Transform2D* transform2D = (Transform2D*)transform;

		transform2D->BuildMatrices();

		Math::Vector3 p1, p2;

		for (int phase = 1; phase <= 2; phase++)
		{
			for (int i = 0; i < 4 * phase; i++)
			{
				if (i == 0)
				{
					p1 = Math::Vector3(0, 0, 0);
					p2 = Math::Vector3(transform2D->size.x, 0, 0);
				}
				else
				if (i == 1)
				{
					p1 = Math::Vector3(transform2D->size.x, 0, 0);
					p2 = Math::Vector3(transform2D->size.x, transform2D->size.y, 0);
				}
				else
				if (i == 2)
				{
					p1 = Math::Vector3(transform2D->size.x, transform2D->size.y, 0);
					p2 = Math::Vector3(0, transform2D->size.y, 0);
				}
				else
				if (i == 3)
				{
					p1 = Math::Vector3(0, transform2D->size.y, 0);
					p2 = Math::Vector3(0, 0, 0);
				}
				else
				if (i == 4)
				{
					p1 = Math::Vector3(transform2D->size.x * 0.5f, 0, 0);
				}
				else
				if (i == 5)
				{
					p1 = Math::Vector3(transform2D->size.x, transform2D->size.y * 0.5f, 0);
				}
				else
				if (i == 6)
				{
					p1 = Math::Vector3(transform2D->size.x * 0.5f, transform2D->size.y, 0);
				}
				else
				if (i == 7)
				{
					p1 = Math::Vector3(0, transform2D->size.y * 0.5f, 0);
				}

				p1 -= Math::Vector3(transform2D->offset.x * transform2D->size.x, transform2D->offset.y * transform2D->size.y, 0);
				p1 = p1 * transform2D->global;
				p2 -= Math::Vector3(transform2D->offset.x * transform2D->size.x, transform2D->offset.y * transform2D->size.y, 0);
				p2 = p2 * transform2D->global;

				if (!ignore2DCamera)
				{
					Math::Vector2 tmp = Sprite::MoveToCamera(Math::Vector2(p1.x, p1.y));
					p1 = Math::Vector3(tmp.x, tmp.y, p1.z);

					tmp = Sprite::MoveToCamera(Math::Vector2(p2.x, p2.y));
					p2 = Math::Vector3(tmp.x, tmp.y, p1.z);
				}
				else
				{
					p1 *= Sprite::screenMul;
					p2 *= Sprite::screenMul;
				}

				if (phase == 1)
				{
					root.render.DebugLine2D(Math::Vector2(p1.x, p1.y), COLOR_WHITE, Math::Vector2(p2.x, p2.y), COLOR_WHITE);
				}
				else
				{
					ancorns[i] = Math::Vector2(p1.x, p1.y);
					root.render.DebugSprite(editorDrawer.anchorn, ancorns[i] - Math::Vector2(4.0f), Math::Vector2(8.0f), selAxis == (i + 1) ? Color(1.0, 0.9f, 0.0f, 1.0f) : COLOR_WHITE);
				}
			}
		}

		p1 = Math::Vector3(0.0f, 0.0f, 0.0f);
		p1 = p1 * transform2D->global;

		if (!ignore2DCamera)
		{
			Math::Vector2 tmp = Sprite::MoveToCamera(Math::Vector2(p1.x, p1.y));
			p1 = Math::Vector3(tmp.x, tmp.y, p1.z);
		}
		else
		{
			p1 *= Sprite::screenMul;
		}

		origin = Math::Vector2(p1.x, p1.y);

		root.render.DebugSprite(editorDrawer.center, ((selAxis == 10) ? movedOrigin : origin) - Math::Vector2(4.0f), Math::Vector2(8.0f), (selAxis == 10 ? Color(1.0, 0.9f, 0.0f, 1.0f) : COLOR_WHITE));
	}

	void Gizmo::RenderTrans3D()
	{
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);
		view_proj = view * view_proj;

		Math::Vector3 pos = transform->local.Pos();
		float z = pos.x*view_proj._13 + pos.y*view_proj._23 + pos.z*view_proj._33 + view_proj._43;

		scale = 0.1f * (1.0f + z);
		scale = fabsf(scale);

		if (mode == TransformType::Move || mode == TransformType::Scale)
		{
			for (int i = 0; i < 3; i++)
			{
				DrawAxis(axises[i]);
			}
		}
		else
		if (mode == TransformType::Rotate)
		{
			DrawCircle(Axis::X);
			DrawCircle(Axis::Y);
			DrawCircle(Axis::Z);
		}
	}

	Math::Vector3 Gizmo::TransformVetcor(Math::Vector3 pos)
	{
		Math::Matrix matrix = transform->local;
		matrix.RemoveScale();

		return matrix.MulNormal(pos);
	}

	Math::Vector2 Gizmo::MakeAligned(Math::Vector2 pos)
	{
		if (align2D.x > 0.01f && pos.x < 0.0f)
		{
			pos.x -= align2D.x;
		}

		if (align2D.y > 0.01f && pos.y < 0.0f)
		{
			pos.y -= align2D.y;
		}

		return Math::Vector2((align2D.x > 0.01f) ? (align2D.x * ((int)(pos.x / align2D.x))) : pos.x, (align2D.y > 0.01f) ? (align2D.y * ((int)(pos.y / align2D.y))) : pos.y);
	}

	void Gizmo::Render()
	{
		if (!transform) return;

		if (transform->Is2D())
		{
			RenderTrans2D();
		}
		else
		{
			RenderTrans3D();
		} 
	}

	void Gizmo::OnMouseMove(Math::Vector2 ms)
	{
		if (!transform) return;

		if (mousedPressed)
		{
			if (transform->Is2D())
			{
				MoveTrans2D(ms - prevMouse);
			}
			else
			{
				MoveTrans3D(ms - prevMouse);
			}
		}
		else
		{
			selAxis = -1;

			if (transform->Is2D())
			{
				CheckSelectionTrans2D(ms);
			}
			else
			{
				CheckSelectionTrans3D(ms);
			}
		}

		prevMouse = ms;
	}

	void Gizmo::OnLeftMouseDown()
	{
		if (selAxis != -1)
		{
			mousedPressed = true;
		}
	}

	void Gizmo::OnLeftMouseUp()
	{
		mousedPressed = false;

		if (!transform) return;

		if (selAxis == 10 && transform->Is2D())
		{
			Transform2D* transform2D = (Transform2D*)transform;

			Math::Matrix inv = transform2D->global;
			inv.Inverse();

			movedOrigin /= Sprite::screenMul;

			if (!ignore2DCamera)
			{
				movedOrigin += (Sprite::edCamPos - Sprite::halfScreen) * Sprite::invScreenMul;
			}

			Math::Vector3 pos = Math::Vector3(movedOrigin.x, movedOrigin.y, 0.0f) * inv / Math::Vector3(transform2D->size.x, transform2D->size.y, 1.0f);
			transform2D->offset += Math::Vector2(pos.x, pos.y);
			transform2D->pos += Math::Vector2(pos.x, pos.y) * transform2D->size;
			pos2d = transform2D->pos;

			selAxis = -1;
		}
	}
}