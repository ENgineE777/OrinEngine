
#include "Gizmo.h"
#include "Root/Root.h"
#include "Support/Sprite.h"
#include "EditorDrawer.h"

namespace Oak
{
	Gizmo* Gizmo::inst = nullptr;

	Gizmo::Transform2D::Transform2D(Math::Vector2* set_pos, Math::Vector2* set_size, Math::Vector2* set_offset, float* set_depth, float* set_rotation, Math::Vector2* set_axis, Math::Matrix* set_mat_parent)
	{
		pos = set_pos;
		size = set_size;
		offset = set_offset;
		rotation = set_rotation;
		depth = set_depth;
		axis = set_axis;
		mat_parent = set_mat_parent;
	}

	Gizmo::Transform2D::Transform2D(Oak::Transform2D& trans)
	{
		pos = &trans.pos;
		size = &trans.size;
		offset = &trans.offset;
		rotation = &trans.rotation;
		depth = &trans.depth;
		axis = &trans.axis;
		mat_parent = trans.parent;
	}

	void Gizmo::Transform2D::BuildMatrices()
	{
		mat_local.Identity();

		if (rotation)
		{
			mat_local.RotateZ(*rotation);
		}

		mat_local.Pos() = Math::Vector3((!axis || axis->x > 0.0f) ? pos->x : -pos->x - size->x,
			(!axis || axis->y > 0.0f) ? pos->y : -pos->y - size->y,
			depth ? *depth : 0.5f);

		mat_global = mat_parent ? (mat_local * (*mat_parent)) : mat_local;
	}

	void Gizmo::Init()
	{
		inst = this;
	}

	bool Gizmo::IsTrans2D()
	{
		return use_trans2D;
	}

	void Gizmo::SetTrans2D(Transform2D trans, int actions, bool set_ignore_2d_camera)
	{
		use_trans2D = true;

		trans2D = trans;
		pos2d = *trans2D.pos;
		size2d = *trans2D.size;

		trans2D_actions = actions;
		ignore_2d_camera = set_ignore_2d_camera;
		enabled = true;

		delta_move = 0.0f;
	}

	void Gizmo::SetTrans2D(Math::Vector2 set_pos)
	{
		if (!trans2D.pos || !enabled)
		{
			return;
		}

		set_pos = MakeAligned(set_pos);
		*(trans2D.pos) = set_pos;
		pos2d = set_pos;
	}

	void Gizmo::SetTrans3D(Math::Matrix* set_transform)
	{
		use_trans2D = false;

		transform = set_transform;
		enabled = true;
	}

	bool Gizmo::IsEnabled()
	{
		return enabled;
	}

	void Gizmo::Disable()
	{
		enabled = false;
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
		Math::Vector3 tr = transform->Pos();

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

		if (useLocalSpace)
		{
			dir = transform->MulNormal(dir);
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

			if (useLocalSpace)
			{
				pos = transform->MulNormal(pos);
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
			mat = *transform;
		}
		else
		{
			mat.Pos() = transform->Pos();
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
		if (useLocalSpace)
		{
			pos = transform->MulNormal(pos);
			pos2 = transform->MulNormal(pos2);
		}

		Math::Vector3 tr = transform->Pos();

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
				ms_dir = Math::Vector2(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y);
				ms_dir.Normalize();

				return true;
			}
		}

		return false;
	}

	void Gizmo::CheckSelectionTrans2D(Math::Vector2 ms)
	{
		if (trans2D_actions & trans_2d_scale)
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

		if (trans2D_actions & trans_2d_anchorn)
		{
			if (origin.x - 7 < ms.x && ms.x < origin.x + 7 &&
				origin.y - 7 < ms.y && ms.y < origin.y + 7)
			{
				selAxis = 10;
				moved_origin = origin;
			}
		}

		if (selAxis == -1)
		{
			if (Math::IsInsideTriangle(ms, ancorns[0], ancorns[1], ancorns[2]) ||
				Math::IsInsideTriangle(ms, ancorns[0], ancorns[2], ancorns[3]))
			{
				//SetCursor(cr_move);

				if (trans2D_actions & trans_2d_move)
				{
					selAxis = 0;
				}
			}
			else
			{
				//SetCursor(cr_rotate);

				if (trans2D_actions & trans_2d_rotate)
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

		if (mode == 0)
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
		if (mode == 1)
		{
			Math::Vector3 trans = transform->Pos() * view;

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
		ms *= (1024.0f / root.render.GetDevice()->GetHeight()) / Sprite::edCamZoom;

		if (ms.Length() < 0.001f)
		{
			return;
		}

		if (selAxis == 0)
		{
			pos2d.x += (!trans2D.axis || trans2D.axis->x > 0.0f) ? ms.x : -ms.x;
			pos2d.y += (!trans2D.axis || trans2D.axis->y > 0.0f) ? ms.y : -ms.y;

			Math::Vector2 prev_pos = *trans2D.pos;
			*trans2D.pos = MakeAligned(pos2d);
			delta_move += *trans2D.pos - prev_pos;
		}
		else
		if (selAxis == 9)
		{
			if (!root.controls.DebugKeyPressed("KEY_Z", AliasAction::Active))
			{
				return;
			}

			Math::Vector2 p1 = prev_ms + ms - origin;
			p1.Normalize();

			Math::Vector2 p2 = prev_ms - origin;
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

				if (trans2D.rotation)
				{
					*trans2D.rotation += k;
				}
			}
		}
		else
		if (selAxis == 10)
		{
			moved_origin += ms * Sprite::screenMul;
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

			dot1 = ms.x * trans2D.mat_global.Vx().x + ms.y * trans2D.mat_global.Vx().y;
			dot2 = ms.x * trans2D.mat_global.Vy().x + ms.y * trans2D.mat_global.Vy().y;

			float delta = dist * k1 * dot1;

			size2d.x += delta;

			if (selAxis == 1 || selAxis == 4 || selAxis == 8)
			{
				pos2d.x -= (1.0f - trans2D.offset->x) * delta;
			}
			else
			if (selAxis == 2 || selAxis == 3 || selAxis == 6)
			{
				pos2d.x += trans2D.offset->x * delta;
			}

			delta = dist * k2 * dot2;

			size2d.y += delta;

			if (selAxis == 1 || selAxis == 2 || selAxis == 5)
			{
				pos2d.y -= delta * (1.0f - trans2D.offset->y);
			}
			else
			if (selAxis == 3 || selAxis == 4 || selAxis == 7)
			{
				pos2d.y += delta * trans2D.offset->y;
			}

			*trans2D.size = size2d;

			Math::Vector2 prev_pos = *trans2D.pos;
			*trans2D.pos = pos2d;

			delta_move += *trans2D.pos - prev_pos;
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

		da *= cur_dir.Dot(ms_dir);

		if (mode == 0)
		{
			da *= scale * 16;

			if (selAxis == 0)
			{
				transform->_41 += da;
			}
			else
			if (selAxis == 1)
			{
				transform->_42 += da;
			}
			else
			{
				transform->_43 += da;
			}
		}
		else
		if (mode == 1)
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
				(*transform) = rot * (*transform);
			}
			else
			{
				Math::Vector3 tr = transform->Pos();
				(*transform) = (*transform) * rot;
				transform->Pos() = tr;
			}
		}
	}

	void Gizmo::RenderTrans2D()
	{
		trans2D.BuildMatrices();

		Math::Vector3 p1, p2;

		for (int phase = 1; phase <= 2; phase++)
		{
			for (int i = 0; i < 4 * phase; i++)
			{
				if (i == 0)
				{
					p1 = Math::Vector3(0, 0, 0);
					p2 = Math::Vector3(trans2D.size->x, 0, 0);
				}
				else
				if (i == 1)
				{
					p1 = Math::Vector3(trans2D.size->x, 0, 0);
					p2 = Math::Vector3(trans2D.size->x, trans2D.size->y, 0);
				}
				else
				if (i == 2)
				{
					p1 = Math::Vector3(trans2D.size->x, trans2D.size->y, 0);
					p2 = Math::Vector3(0, trans2D.size->y, 0);
				}
				else
				if (i == 3)
				{
					p1 = Math::Vector3(0, trans2D.size->y, 0);
					p2 = Math::Vector3(0, 0, 0);
				}
				else
				if (i == 4)
				{
					p1 = Math::Vector3(trans2D.size->x * 0.5f, 0, 0);
				}
				else
				if (i == 5)
				{
					p1 = Math::Vector3(trans2D.size->x, trans2D.size->y * 0.5f, 0);
				}
				else
				if (i == 6)
				{
					p1 = Math::Vector3(trans2D.size->x * 0.5f, trans2D.size->y, 0);
				}
				else
				if (i == 7)
				{
					p1 = Math::Vector3(0, trans2D.size->y * 0.5f, 0);
				}

				p1 -= Math::Vector3((trans2D.offset ? trans2D.offset->x : 0.5f) * trans2D.size->x, (trans2D.offset ? trans2D.offset->y : 0.5f) * trans2D.size->y, 0);
				p1 = p1 * trans2D.mat_global;
				p2 -= Math::Vector3((trans2D.offset ? trans2D.offset->x : 0.5f) * trans2D.size->x, (trans2D.offset ? trans2D.offset->y : 0.5f) * trans2D.size->y, 0);
				p2 = p2 * trans2D.mat_global;

				if (!ignore_2d_camera)
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
		p1 = p1 * trans2D.mat_global;

		if (!ignore_2d_camera)
		{
			Math::Vector2 tmp = Sprite::MoveToCamera(Math::Vector2(p1.x, p1.y));
			p1 = Math::Vector3(tmp.x, tmp.y, p1.z);
		}
		else
		{
			p1 *= Sprite::screenMul;
		}

		origin = Math::Vector2(p1.x, p1.y);

		root.render.DebugSprite(editorDrawer.center, ((selAxis == 10) ? moved_origin : origin) - Math::Vector2(4.0f), Math::Vector2(8.0f));
	}

	void Gizmo::RenderTrans3D()
	{
		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);
		view_proj = view * view_proj;

		Math::Vector3 pos = transform->Pos();
		float z = pos.x*view_proj._13 + pos.y*view_proj._23 + pos.z*view_proj._33 + view_proj._43;

		scale = 0.1f * (1.0f + z);
		scale = fabsf(scale);

		if (mode == 0)
		{
			DrawAxis(0);
			DrawAxis(1);
			DrawAxis(2);
		}
		else
		{
			DrawCircle(0);
			DrawCircle(1);
			DrawCircle(2);
		}
	}

	Math::Vector2 Gizmo::MakeAligned(Math::Vector2 pos)
	{
		if (align2d.x > 0.01f && pos.x < 0.0f)
		{
			pos.x -= align2d.x;
		}

		if (align2d.y > 0.01f && pos.y < 0.0f)
		{
			pos.y -= align2d.y;
		}

		return Math::Vector2((align2d.x > 0.01f) ? (align2d.x * ((int)(pos.x / align2d.x))) : pos.x,
					   (align2d.y > 0.01f) ? (align2d.y * ((int)(pos.y / align2d.y))) : pos.y);
	}

	void Gizmo::Render()
	{
		if (!enabled) return;

		if (IsTrans2D())
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
		if (!enabled) return;

		if (mousedPressed)
		{
			if (IsTrans2D())
			{
				MoveTrans2D(ms - prev_ms);
			}
			else
			{
				MoveTrans3D(ms - prev_ms);
			}
		}
		else
		{
			selAxis = -1;

			if (IsTrans2D())
			{
				CheckSelectionTrans2D(ms);
			}
			else
			{
				CheckSelectionTrans3D(ms);
			}
		}

		prev_ms = ms;
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

		if (selAxis == 10 && IsTrans2D())
		{
			Math::Matrix inv = trans2D.mat_global;
			inv.Inverse();

			moved_origin /= Sprite::screenMul;

			if (!ignore_2d_camera)
			{
				moved_origin += (Sprite::edCamPos - Sprite::halfScreen) * Sprite::invScreenMul;
			}

			Math::Vector3 pos = Math::Vector3(moved_origin.x, moved_origin.y, 0.0f) * inv / Math::Vector3(trans2D.size->x, trans2D.size->y, 1.0f);
			*trans2D.offset += Math::Vector2(pos.x, pos.y);
			*trans2D.pos += Math::Vector2(pos.x, pos.y) * (*trans2D.size);
			pos2d = *trans2D.pos;

			selAxis = -1;
		}
	}
}