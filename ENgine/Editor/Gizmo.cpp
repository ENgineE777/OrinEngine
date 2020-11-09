
#include "Gizmo.h"
#include "Root/Root.h"
#include "Support/Sprite.h"
#include "EditorDrawer.h"

namespace Oak
{
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

	Color Gizmo::CheckColor(int axis)
	{
		Color color;

		if (axis == selAxis)
		{
			color = COLOR_YELLOW;
		}
		else
		{
			if (axis == 0)
			{
				color = COLOR_RED;
			}
			else
			if (axis == 1)
			{
				color = COLOR_GREEN;
			}
			else
			{
				color = COLOR_BLUE;
			}
		}

		return color;
	}

	void Gizmo::DrawAxis(int axis)
	{
		Math::Vector3 tr = transform->local.Pos();

		Color color = CheckColor(axis);
		Math::Vector3 dir;
		dir = 0.0f;

		if (axis == 0)
		{
			dir.x = scale;
		}
		else
		if (axis == 1)
		{
			dir.y = scale;
		}
		else
		{
			dir.z = scale;
		}

		if (useLocalSpace || mode == TransformType::Scale)
		{
			dir = TransformVetcor(dir);
		}

		dir += tr;

		root.render.DebugLine(dir, color, tr, color, false);

		float hgt = 0.85f * scale;
		float r = 0.05f * scale;
		int nums = 64;

		for (int i = 0; i < nums; i++)
		{
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
			float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

			Math::Vector3 pos;

			if (axis == 0)
			{
				pos = Math::Vector3(hgt, dx, dz);
			}
			else
			if (axis == 1)
			{
				pos = Math::Vector3(dx, hgt, dz);
			}
			else
			{
				pos = Math::Vector3(dx, dz, hgt);
			}

			if (useLocalSpace || mode == TransformType::Scale)
			{
				pos = TransformVetcor(pos);
			}

			root.render.DebugLine(dir, color, pos + tr, color, false);
		}
	}

	void Gizmo::DrawCircle(int axis)
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

				if (axis == 0)
				{
					pos.Set(0.0f, last_dx, last_dz);
					pos2.Set(0.0f, dx, dz);
				}
				else
				if (axis == 1)
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

		if (transform2DActions & TransformType::Scale)
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

		if (transform2DActions & TransformType::Anchorn)
		{
			if (transform2D->offset.x - 7 < ms.x && ms.x < transform2D->offset.x + 7 &&
				transform2D->offset.y - 7 < ms.y && ms.y < transform2D->offset.y + 7)
			{
				selAxis = 10;
				movedOrigin = transform2D->offset;
			}
		}

		if (selAxis == -1)
		{
			if (Math::IsInsideTriangle(ms, ancorns[0], ancorns[1], ancorns[2]) ||
				Math::IsInsideTriangle(ms, ancorns[0], ancorns[2], ancorns[3]))
			{
				//SetCursor(cr_move);

				if (transform2DActions & TransformType::Move)
				{
					selAxis = 0;
				}
			}
			else
			{
				//SetCursor(cr_rotate);

				if (transform2DActions & TransformType::Rotate)
				{
					selAxis = 9;
				}
			}
		}
	}

	bool Gizmo::CheckSelectionTrans3D(int axis, Math::Vector2 ms)
	{
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);
		view_proj = view * view_proj;

		if (mode == TransformType::Move)
		{
			Math::Vector3 dir;
			dir = 0.0f;

			if (axis == 0)
			{
				dir.x = scale;
			}
			else
			if (axis == 1)
			{
				dir.y = scale;
			}
			else
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

					if (axis == 0)
					{
						pos = Math::Vector3(0.0f, last_dx, last_dz);
						pos2 = Math::Vector3(0.0f, dx, dz);
					}
					else
					if (axis == 1)
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
		else
		if (mode == TransformType::Scale)
		{
			Math::Vector3 dir;
			dir = 0.0f;

			if (axis == 0)
			{
				dir.x = scale;
			}
			else
			if (axis == 1)
			{
				dir.y = scale;
			}
			else
			{
				dir.z = scale;
			}

			int count = 7;

			for (int i = 0; i < count; i++)
			{
				Math::Vector3 pos = dir * (float)i * (1.0f / (float)count);
				Math::Vector3 pos2 = dir * (float)(i + 1) * (1.0f / (float)count);
				Math::Vector3 tr(0.0f);

				if (CheckInersection(pos, pos2, ms, tr, false, view, view_proj))
				{
					return true;
				}
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
			if (CheckSelectionTrans3D(i, ms))
			{
				selAxis = i;
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

			if (selAxis == 0)
			{
				transform->local._41 += da;
			}
			else
			if (selAxis == 1)
			{
				transform->local._42 += da;
			}
			else
			{
				transform->local._43 += da;
			}
		}
		else
		if (mode == TransformType::Rotate)
		{
			da *= -5.0f;

			Math::Matrix rot;

			if (selAxis == 0)
			{
				rot.RotateX(da);
			}
			else
			if (selAxis == 1)
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

			if (selAxis == 0)
			{
				float length = transform->local.Vx().Normalize();
				length += da;
				transform->local.Vx() *= fmaxf(length, 0.1f);
			}
			else
			if (selAxis == 1)
			{
				float length = transform->local.Vy().Normalize();
				length += da;
				transform->local.Vy() *= fmaxf(length, 0.1f);
			}
			else
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

		root.render.DebugSprite(editorDrawer.center, ((selAxis == 10) ? movedOrigin : origin) - Math::Vector2(4.0f), Math::Vector2(8.0f));
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

		if (mode == TransformType::Move)
		{
			DrawAxis(0);
			DrawAxis(1);
			DrawAxis(2);
		}
		else
		if (mode == TransformType::Rotate)
		{
			DrawCircle(0);
			DrawCircle(1);
			DrawCircle(2);
		}
		else
		if (mode == TransformType::Scale)
		{
			DrawAxis(0);
			DrawAxis(1);
			DrawAxis(2);
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