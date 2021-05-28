
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

	void Gizmo::SetTransform(Transform* set_transform)
	{
		OAK_ASSERT(set_transform != nullptr, "Gizmo::set_transform3d == null")
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

		axis.from = GetGlobalPos();

		Color color = CheckColor(axis.type);

		if (mode == TransformMode::Move)
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
		if (mode == TransformMode::Scale)
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

		if (useLocalSpace || mode == TransformMode::Scale)
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

		axis.axis = axis.to - axis.from;
		axis.axis.Normalize();

		root.render.DebugLine(axis.from, color, axis.to, color, false);

		bool needHighlight = ((axis.type == Axis::X && (selAxis == (int)Axis::XY || selAxis == (int)Axis::XYZ)) ||
		                      (axis.type == Axis::Y && (selAxis == (int)Axis::XY || selAxis == (int)Axis::XYZ)) ||
		                      (axis.type == Axis::Z && (selAxis == (int)Axis::YZ || selAxis == (int)Axis::XYZ)));

		color = needHighlight ? COLOR_YELLOW : CheckColor(axis.type, true);

		root.render.DebugLine(axis.subPointLeft, color, axis.subPointFrom, color, false);

		if (mode == TransformMode::Scale)
		{
			root.render.DebugLine(axis.subPointLeft2, color, axis.subPointFrom2, needHighlight ? color : CheckColor(axis.type, true), false);
		}

		needHighlight = ((axis.type == Axis::X && (selAxis == (int)Axis::XZ || selAxis == (int)Axis::XYZ)) ||
		                 (axis.type == Axis::Y && (selAxis == (int)Axis::YZ || selAxis == (int)Axis::XYZ)) ||
		                 (axis.type == Axis::Z && (selAxis == (int)Axis::XZ || selAxis == (int)Axis::XYZ)));

		color = needHighlight ? COLOR_YELLOW : CheckColor(axis.type, true);

		root.render.DebugLine(axis.subPointRight, color, axis.subPointFrom , color, false);

		if (mode == TransformMode::Scale)
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

			if (useLocalSpace || mode == TransformMode::Scale)
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
			mat = transform->global;
		}
		
		mat.Pos() = GetGlobalPos();

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
		if (useLocalSpace || mode == TransformMode::Scale)
		{
			pos = TransformVetcor(pos);
			pos2 = TransformVetcor(pos2);
		}

		Math::Vector3 tr = GetGlobalPos();

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

	void Gizmo::CheckSelectionTransRect(Math::Vector2 ms)
	{
		if (transform->transformFlag & TransformFlag::RectSizeXY)
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

		if (origin.x - 7 < ms.x && ms.x < origin.x + 7 &&
			origin.y - 7 < ms.y && ms.y < origin.y + 7 && transform->transformFlag & TransformFlag::RectAnchorn)
		{
			selAxis = 10;
			movedOrigin = trans2DProjection;
		}

		if (selAxis == -1)
		{
			if (Math::IsInsideTriangle(ms, ancorns[0], ancorns[1], ancorns[2]) ||
				Math::IsInsideTriangle(ms, ancorns[0], ancorns[2], ancorns[3]))
			{
				//SetCursor(cr_move);

				selAxis = 0;

				rectPos = transform->position;
			}
			else
			{
				//SetCursor(cr_rotate);

				selAxis = 9;
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

		if (mode == TransformMode::Move || mode == TransformMode::Scale)
		{
			TransformFlag transformFlag = (mode == TransformMode::Move) ? TransformFlag::MoveX : TransformFlag::ScaleX;

			if (axis.type == Axis::Y)
			{
				transformFlag = (mode == TransformMode::Move) ? TransformFlag::MoveY : TransformFlag::ScaleY;
			}
			else
			if (axis.type == Axis::Z)
			{
				transformFlag = (mode == TransformMode::Move) ? TransformFlag::MoveZ : TransformFlag::ScaleZ;
			}

			if (!(transformFlag & transform->transformFlag))
			{
				return false;
			}

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
		if (mode == TransformMode::Rotate)
		{
			TransformFlag transformFlag = TransformFlag::RotateX;

			if (axis.type == Axis::Y)
			{
				transformFlag = TransformFlag::RotateY;
			}
			else
			if (axis.type == Axis::Z)
			{
				transformFlag = TransformFlag::RotateZ;
			}

			if (!(transformFlag & transform->transformFlag))
			{
				return false;
			}

			Math::Vector3 trans = GetGlobalPos() * view;

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
 
		for (int i = 0; i < 3; i++)
		{
			if (CheckSelectionTrans3D(axises[i], ms))
			{
				selAxis = (int)axises[i].type;

				if (axises[i].type == Axis::X)
				{
					Math::IntersectPlaneRay(GetGlobalPos(), axises[2].axis, mouseOrigin, mouseDirection, selectionOffset);
					selectionOffset -= GetGlobalPos();
				}

				if (axises[i].type == Axis::Y)
				{
					Math::IntersectPlaneRay(GetGlobalPos(), axises[0].axis, mouseOrigin, mouseDirection, selectionOffset);
					selectionOffset -= GetGlobalPos();
				}

				if (axises[i].type == Axis::Z)
				{
					Math::IntersectPlaneRay(GetGlobalPos(), axises[1].axis, mouseOrigin, mouseDirection, selectionOffset);
					selectionOffset -= GetGlobalPos();
				}
			}
		}

		if (mode == TransformMode::Move)
		{
			if (transform->transformFlag & TransformFlag::MoveX && transform->transformFlag & TransformFlag::MoveZ &&
				(Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::XZ;
				Math::IntersectPlaneRay(GetGlobalPos(), axises[1].axis, mouseOrigin, mouseDirection,selectionOffset);
				selectionOffset -= GetGlobalPos();
			}

			if (transform->transformFlag & TransformFlag::MoveX && transform->transformFlag & TransformFlag::MoveY &&
				(Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::XY;
				Math::IntersectPlaneRay(GetGlobalPos(), axises[2].axis, mouseOrigin, mouseDirection, selectionOffset);
				selectionOffset -= GetGlobalPos();
			}

			if (transform->transformFlag & TransformFlag::MoveY && transform->transformFlag & TransformFlag::MoveZ &&
				(Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::YZ;
				Math::IntersectPlaneRay(GetGlobalPos(), axises[0].axis, mouseOrigin, mouseDirection, selectionOffset);
				selectionOffset -= GetGlobalPos();
			}
		}

		if (mode == TransformMode::Scale)
		{
			if (transform->transformFlag & TransformFlag::ScaleX && transform->transformFlag & TransformFlag::ScaleZ &&
				(Math::IntersectTrianglrRay(axises[0].subPointFrom2, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[0].subPointRight2, axises[0].subPointFrom, axises[0].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].subPointFrom2, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].subPointRight2, axises[2].subPointFrom, axises[2].subPointRight, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::XZ;
			}

			if (transform->transformFlag & TransformFlag::ScaleX && transform->transformFlag & TransformFlag::ScaleY &&
				(Math::IntersectTrianglrRay(axises[0].subPointFrom2, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[0].subPointLeft2, axises[0].subPointFrom, axises[0].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].subPointFrom2, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].subPointLeft2, axises[1].subPointFrom, axises[1].subPointLeft, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::XY;
			}

			if (transform->transformFlag & TransformFlag::ScaleY && transform->transformFlag & TransformFlag::ScaleZ &&
				(Math::IntersectTrianglrRay(axises[1].subPointFrom2, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].subPointRight2, axises[1].subPointFrom, axises[1].subPointRight, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].subPointFrom2, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].subPointLeft2, axises[2].subPointFrom, axises[2].subPointLeft, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::YZ;
			}

			if (transform->transformFlag & TransformFlag::ScaleY && transform->transformFlag & TransformFlag::ScaleY && transform->transformFlag & TransformFlag::ScaleZ &&
				(Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom2, axises[0].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom2, axises[2].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[0].from, axises[0].subPointFrom2, axises[0].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom2, axises[1].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[1].from, axises[1].subPointFrom2, axises[1].subPointRight2, mouseOrigin, mouseDirection, 1000.0f) ||
				 Math::IntersectTrianglrRay(axises[2].from, axises[2].subPointFrom2, axises[2].subPointLeft2, mouseOrigin, mouseDirection, 1000.0f)))
			{
				selAxis = (int)Axis::XYZ;
			}
		}
	}

	void Gizmo::MoveTransRect(Math::Vector2 ms)
	{
		Math::Matrix inv = transform->global;
		inv.Inverse();

		Math::Vector3 delta = trans2DProjection * (*transform->unitsScale) * inv - trans2DPrevProjection * (*transform->unitsScale) * inv;

		if (ms.Length() < 0.001f)
		{
			return;
		}

		if (selAxis == 0)
		{
			rectPos.x += delta.x * transform->local.Vx().x;
			rectPos.y += delta.x * transform->local.Vx().y;

			rectPos.x += delta.y * transform->local.Vy().x;
			rectPos.y += delta.y * transform->local.Vy().y;

			transform->position = AligneRectPos(rectPos);
		}
		else
		if (selAxis == 9)
		{
			if (!root.controls.DebugKeyPressed("KEY_Z", AliasAction::Pressed))
			{
				return;
			}

			float sign = mouseDirection.Dot(transform->global.Vz()) > 0.1f ? -1.0f : 1.0f ;

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

				transform->rotation.z -= k * sign / Math::Radian;
			}
		}
		else
		if (selAxis == 10)
		{
			movedOrigin = trans2DProjection;
		}
		else
		if (selAxis > 0)
		{
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

			delta.x *= k1;
			delta.y *= k2;

			if (root.controls.DebugKeyPressed("KEY_LSHIFT", AliasAction::Pressed))
			{
				delta.y = transform->size.y / transform->size.x * delta.x;
			}

			transform->size.x += delta.x;

			auto pos = transform->position;

			if (selAxis == 1 || selAxis == 4 || selAxis == 8)
			{
				pos.x -= (1.0f - transform->offset.x) * delta.x * transform->local.Vx().x;
				pos.y -= (1.0f - transform->offset.x) * delta.x * transform->local.Vx().y;
			}
			else
			if (selAxis == 2 || selAxis == 3 || selAxis == 6)
			{
				pos.x += transform->offset.x * delta.x * transform->local.Vx().x;
				pos.y += transform->offset.x * delta.x * transform->local.Vx().y;
			}

			transform->size.y += delta.y;

			if (selAxis == 1 || selAxis == 2 || selAxis == 5)
			{
				pos.x -= delta.y * transform->offset.y * transform->local.Vy().x;
				pos.y -= delta.y * transform->offset.y * transform->local.Vy().y;
			}
			else
			if (selAxis == 3 || selAxis == 4 || selAxis == 7)
			{
				pos.x += delta.y * (1.0f - transform->offset.y) * transform->local.Vy().x;
				pos.y += delta.y * (1.0f - transform->offset.y) * transform->local.Vy().y;
			}

			transform->position = pos;
		}
	}

	void Gizmo::AxisTranslation(AxisData& axisData)
	{
		Math::Vector3 original_point = GetGlobalPos() + selectionOffset;
		Math::Vector3 point = original_point;

		Math::Vector3 plane_tangent = axisData.axis.Cross(point - mouseOrigin);
		Math::Vector3 plane_normal = axisData.axis.Cross(plane_tangent);
		PlaneTranslation(plane_normal, point);

		point = original_point + axisData.axis * (point - original_point).Dot(axisData.axis);

		SetGlobalPos(point - selectionOffset);
	}

	void Gizmo::PlaneTranslation(AxisData& axisData)
	{
		const float denom = mouseDirection.Dot(axisData.axis);
		if (std::abs(denom) == 0) return;

		const float t = (GetGlobalPos() + selectionOffset - mouseOrigin).Dot(axisData.axis) / denom;
		if (t < 0) return;

		SetGlobalPos(mouseOrigin + mouseDirection * t - selectionOffset);
	}

	void Gizmo::PlaneTranslation(Math::Vector3& plane_normal, Math::Vector3& point)
	{
		const float denom = mouseDirection.Dot(plane_normal);
		if (std::abs(denom) == 0) return;

		const float t = (point - mouseOrigin).Dot(plane_normal) / denom;
		if (t < 0) return;

		point = mouseOrigin + mouseDirection * t;
	}

	void Gizmo::CaclLocalMatrix()
	{
		Math::Matrix inverse = transform->parent ? (*transform->parent) : Math::Matrix();
		inverse.Inverse();

		transform->local = transform->global * inverse;
		transform->SetData(transform->local);
	}

	Math::Vector3 Gizmo::GetGlobalPos()
	{
		return transform->unitsInvScale ? (transform->global.Pos() * (*transform->unitsInvScale)) : transform->global.Pos();
	}

	void Gizmo::SetGlobalPos(Math::Vector3 pos)
	{
		transform->global.Pos() = pos;
		
		if (transform->unitsScale)
		{
			transform->global.Pos() *= (*transform->unitsScale);
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

		if (mode == TransformMode::Move)
		{
			if (selAxis == (int)Axis::X)
			{
				AxisTranslation(axises[0]);
			}

			if (selAxis == (int)Axis::Y)
			{
				AxisTranslation(axises[1]);
			}

			if (selAxis == (int)Axis::Z)
			{
				AxisTranslation(axises[2]);
			}

			if (selAxis == (int)Axis::YZ)
			{
				PlaneTranslation(axises[0]);
			}

			if (selAxis == (int)Axis::XZ)
			{
				PlaneTranslation(axises[1]);
			}

			if (selAxis == (int)Axis::XY)
			{
				PlaneTranslation(axises[2]);
			}

			CaclLocalMatrix();
		}
		else
		if (mode == TransformMode::Rotate)
		{
			da *= 5.0f;

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
				Math::Matrix scaleMat;
				scaleMat.Scale(Math::Vector3(transform->local.Vx().Normalize(), transform->local.Vy().Normalize(), transform->local.Vz().Normalize()));

				transform->local = scaleMat * rot * transform->local;
				transform->SetData(transform->local);
			}
			else
			{
				Math::Vector3 tr = transform->global.Pos();
				transform->global = transform->global * rot;
				transform->global.Pos() = tr;

				CaclLocalMatrix();
			}
		}
		else
		if (mode == TransformMode::Scale)
		{
			da *= scale * 16;

			if (selAxis & (int)Axis::X)
			{
				transform->scale.x = fmaxf(transform->scale.x + da, 0.1f);
			}

			if (selAxis & (int)Axis::Y)
			{
				transform->scale.y = fmaxf(transform->scale.y + da, 0.1f);
			}

			if (selAxis & (int)Axis::Z)
			{
				transform->scale.z = fmaxf(transform->scale.z + da, 0.1f);
			}
		}
	}

	void Gizmo::RenderTransRect()
	{
		transform->BuildMatrices();

		Math::Vector3 p1, p2;

		for (int phase = 1; phase <= 2; phase++)
		{
			for (int i = 0; i < 4 * phase; i++)
			{
				if (i == 0)
				{
					p1 = Math::Vector3(0, 0, 0);
					p2 = Math::Vector3(transform->size.x, 0, 0);
				}
				else
				if (i == 1)
				{
					p1 = Math::Vector3(transform->size.x, 0, 0);
					p2 = Math::Vector3(transform->size.x, transform->size.y, 0);
				}
				else
				if (i == 2)
				{
					p1 = Math::Vector3(transform->size.x, transform->size.y, 0);
					p2 = Math::Vector3(0, transform->size.y, 0);
				}
				else
				if (i == 3)
				{
					p1 = Math::Vector3(0, transform->size.y, 0);
					p2 = Math::Vector3(0, 0, 0);
				}
				else
				if (i == 4)
				{
					p1 = Math::Vector3(transform->size.x * 0.5f, 0, 0);
				}
				else
				if (i == 5)
				{
					p1 = Math::Vector3(transform->size.x, transform->size.y * 0.5f, 0);
				}
				else
				if (i == 6)
				{
					p1 = Math::Vector3(transform->size.x * 0.5f, transform->size.y, 0);
				}
				else
				if (i == 7)
				{
					p1 = Math::Vector3(0, transform->size.y * 0.5f, 0);
				}

				p1 -= Math::Vector3(transform->offset.x * transform->size.x, (1.0f - transform->offset.y) * transform->size.y, 0);
				p1 = p1 * transform->global;
				p1 *= *transform->unitsInvScale;
				p2 -= Math::Vector3(transform->offset.x * transform->size.x, (1.0f - transform->offset.y) * transform->size.y, 0);
				p2 = p2 * transform->global;
				p2 *= *transform->unitsInvScale;

				Math::Vector2 tmp = Math::Vector2(p1.x, p1.y);
				p1 = Math::Vector3(tmp.x, tmp.y, p1.z);

				tmp = Math::Vector2(p2.x, p2.y);
				p2 = Math::Vector3(tmp.x, tmp.y, p1.z);

				if (phase == 1)
				{
					root.render.DebugLine(p1, COLOR_WHITE, p2, COLOR_WHITE, false);
				}
				else
				{
					Math::Vector3 pos = root.render.TransformToScreen(p1, 2);

					if (pos.z > 0.01f)
					{
						ancorns[i] = Math::Vector2(pos.x, pos.y);
						root.render.DebugSprite(editorDrawer.anchorn, ancorns[i] - Math::Vector2(4.0f), Math::Vector2(8.0f), selAxis == (i + 1) ? Color(1.0, 0.9f, 0.0f, 1.0f) : COLOR_WHITE);
					}
					else
					{
						ancorns[i] = -100.0f;
					}
				}
			}
		}

		if (selAxis == 10 && mousedPressed)
		{
			p1 = movedOrigin;
		}
		else
		{
			p1 = Math::Vector3(0.0f, 0.0f, 0.0f);
			p1 = p1 * transform->global;
			p1 *= *transform->unitsInvScale;;
		}

		p1 = root.render.TransformToScreen(p1, 2);

		if (p1.z > 0.01f)
		{
			origin = Math::Vector2(p1.x, p1.y);

			root.render.DebugSprite(editorDrawer.center, origin - Math::Vector2(4.0f), Math::Vector2(8.0f), (selAxis == 10 ? Color(1.0, 0.9f, 0.0f, 1.0f) : COLOR_WHITE));
		}
		else
		{
			origin = -100.0f;
		}
	}

	void Gizmo::RenderTrans3D()
	{
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);
		view_proj = view * view_proj;

		Math::Vector3 pos = GetGlobalPos();

		float z = pos.x * view_proj._13 + pos.y * view_proj._23 + pos.z * view_proj._33 + view_proj._43;

		scale = 0.15f * (1.0f + z);
		scale = fabsf(scale);

		if (mode == TransformMode::Move || mode == TransformMode::Scale)
		{
			for (int i = 0; i < 3; i++)
			{
				DrawAxis(axises[i]);
			}
		}
		else
		if (mode == TransformMode::Rotate)
		{
			DrawCircle(Axis::X);
			DrawCircle(Axis::Y);
			DrawCircle(Axis::Z);
		}
	}

	Math::Vector3 Gizmo::TransformVetcor(Math::Vector3 pos)
	{
		Math::Matrix matrix = transform->global;
		matrix.RemoveScale();

		return matrix.MulNormal(pos);
	}

	Math::Vector3 Gizmo::AligneRectPos(Math::Vector3& pos)
	{
		Math::Vector3 res = pos;

		if (useAlignRect)
		{
			res.x = alignRect.x * ((int)(res.x / alignRect.x));
			res.y = alignRect.y * ((int)(res.y / alignRect.y));
		}

		return res;
	}

	void Gizmo::Render()
	{
		if (!transform) return;

		if (mode == TransformMode::Rectangle)
		{
			RenderTransRect();
		}
		else
		{
			RenderTrans3D();
		} 
	}

	void Gizmo::OnMouseMove(Math::Vector2 ms)
	{
		if (!transform) return;

		Math::Vector2 msConv = ms;
		msConv.x /= (float)root.render.GetDevice()->GetWidth();
		msConv.y /= (float)root.render.GetDevice()->GetHeight();

		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);

		Math::Vector3 v;
		v.x = (2.0f * msConv.x - 1) / view_proj._11;
		v.y = -(2.0f * msConv.y - 1) / view_proj._22;
		v.z = 1.0f;

		Math::Matrix inv_view = view;
		inv_view.Inverse();
		mouseOrigin = inv_view.Pos();

		mouseDirection.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		mouseDirection.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		mouseDirection.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		mouseDirection.Normalize();

		Math::IntersectPlaneRay(GetGlobalPos(), Math::Vector3(0.0f, 0.0f, -1.0f), mouseOrigin, mouseDirection, trans2DProjection);

		if (mousedPressed)
		{
			if (mode == TransformMode::Rectangle)
			{
				MoveTransRect(ms - prevMouse);
			}
			else
			{
				MoveTrans3D(ms - prevMouse);
			}
		}
		else
		{
			selAxis = -1;

			if (mode == TransformMode::Rectangle)
			{
				CheckSelectionTransRect(ms);
			}
			else
			{
				CheckSelectionTrans3D(ms);
			}
		}

		prevMouse = ms;

		trans2DPrevProjection = trans2DProjection;
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

		if (selAxis == 10 && mode == TransformMode::Rectangle)
		{
			Math::Matrix inv = transform->global;
			inv.Inverse();

			Math::Vector3 pos = movedOrigin * (*transform->unitsScale) * inv / Math::Vector3(transform->size.x, transform->size.y, 1.0f);
			transform->offset = Math::Vector3(transform->offset.x + pos.x, transform->offset.y - pos.y, 0.0f);

			transform->position.x += pos.x * transform->local.Vx().x * transform->size.x;
			transform->position.y += pos.x * transform->local.Vx().y * transform->size.x;

			transform->position.x += pos.y * transform->local.Vy().x * transform->size.y;
			transform->position.y += pos.y * transform->local.Vy().y * transform->size.y;

			selAxis = -1;
		}
	}
}