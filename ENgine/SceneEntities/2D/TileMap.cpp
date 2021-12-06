
#include "TileMap.h"
#include "Root/Root.h"
#include "Editor/Editor.h"

namespace Oak
{
	CLASSREG(SceneEntity, TileMap, "TileMap")

	META_DATA_DESC(TileMap)
		BASE_SCENE_ENTITY_PROP(TileMap)
		INT_PROP(TileMap, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		ASSET_TEXTURE_PROP(TileMap, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	void TileMap::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;

		transform.transformFlag = MoveXYZ;
	}

	void TileMap::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);

		transform.offset.x = 0.0f;
		transform.offset.y = 0.0f;

	#ifdef EDITOR
		Tasks(true)->DelAllTasks(this);
	#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&TileMap::Draw);
	}

	void TileMap::Draw(float dt)
	{
		if (IsEditMode())
		{
			editor.gridOrigin = transform.GetGlobal().Pos();
			editor.gridStep = texture.GetSize();
		}

		/*if (GetState() == Invisible)
		{
			return;
		}*/

		/*trans.offset = sprite_asset->trans.offset;
		trans.size = sprite_asset->trans.size + 0.5f;

		Vector2 pos = trans.pos;
		trans.pos *= axis_scale;

		Vector2 cam_pos = 0.0f;
		if (Sprite::use_ed_cam)
		{
			cam_pos = Sprite::ed_cam_pos;
			Sprite::ed_cam_pos *= axis_scale;
		}
		else
		{
			cam_pos = Sprite::cam_pos;
			Sprite::cam_pos *= axis_scale;
		}*/

		for (auto inst : instances)
		{
			/*if (GetState() == Active)
			{
				Sprite::UpdateFrame(&sprite_asset->sprite[inst.index], &inst.frame_state, dt);
			}*/

			//if (inst.color.a < 0.01f)
			{
				//continue;
			}

			/*Vector2 pos = inst.GetPos();
			trans.mat_global.Pos() = { pos.x, pos.y, trans.depth };
			inst.frame_state.horz_flipped = inst.GetFlipped();
			inst.color.a = inst.GetAlpha();

			Sprite::Draw(&trans, inst.color, &sprite_asset->sprite[inst.index], &inst.frame_state, true, false);*/
		}

		/*trans.size = sprite_asset->trans.size;

	#ifdef EDITOR
		if (rect_select)
		{
			for (auto& index : sel_instances)
			{
				auto& inst = instances[index];
				Vector2 pos = Sprite::MoveToCamera(inst.GetPos() - Asset()->trans.offset * Asset()->trans.size);
				Vector2 pos2 = Sprite::MoveToCamera(inst.GetPos() - Asset()->trans.offset * Asset()->trans.size + Asset()->trans.size);

				core.render.DebugRect2D(pos, pos2, COLOR_WHITE);
			}
		}

		if (edited)
		{
			if (sel_inst != -1)
			{
				trans.pos = instances[sel_inst].GetPos();
				trans.rotation = instances[sel_inst].GetAngle();
				trans.BuildMatrices();
			}
		}
	#endif*/

		if (IsVisible())
		{
			auto trans = transform;

			Math::Matrix mat = trans.global;
			auto pos = mat.Pos();

			for (auto inst : instances)
			{
				mat.Pos() = pos + mat.Vx() * (float)inst.x * transform.size.x + mat.Vy() * (float)inst.y * transform.size.y;

				trans.global = mat;

				texture.Draw(&trans, COLOR_WHITE, dt);
			}
		}
	}

#ifdef OAK_EDITOR
	void TileMap::SetEditMode(bool ed)
	{
		SceneEntity::SetEditMode(ed);

		editor.ownGrid = ed;

		if (!ed)
		{
			mode = Mode::Inactive;
		}
	}

	void TileMap::OnMouseMove(Math::Vector2 ms)
	{
		if (mode != Mode::Inactive)
		{
			Math::Vector3 mouseOrigin;
			Math::Vector3 mouseDirection;

			Math::GetMouseRay(ms, mouseOrigin, mouseDirection);
			auto trans = transform.GetGlobal();
			trans.Pos() *= Sprite::pixelsPerUnitInvert;

			Math::Vector3 pos;
			Math::IntersectPlaneRay(trans.Pos(), trans.Vz(), mouseOrigin, mouseDirection, pos);

			pos = pos - trans.Pos();

			if (pos.x < 0.0f)
			{
				pos.x -= (transform.size.x * Sprite::pixelsPerUnitInvert);
			}

			int x = (int)(pos.x / (transform.size.x * Sprite::pixelsPerUnitInvert));

			if (pos.y > 0.0f)
			{
				pos.y += (transform.size.x * Sprite::pixelsPerUnitInvert);
			}

			int y = (int)(pos.y / (transform.size.y * Sprite::pixelsPerUnitInvert));

			bool newOne = true;

			for (int i = 0; i < instances.size(); i++)
			{
				if (instances[i].x == x && instances[i].y == y)
				{
					if (mode == Mode::Erase)
					{
						instances.erase(instances.begin() + i);
					}

					newOne = false;

					break;
				}
			}

			if (newOne && mode == Mode::Place)
			{
				instances.push_back({ x, y });
			}
		}
	}

	void TileMap::OnLeftMouseDown()
	{
		mode = Mode::Place;
	}

	void TileMap::OnLeftMouseUp()
	{
		mode = Mode::Inactive;
	}

	void TileMap::OnRightMouseDown()
	{
		mode = Mode::Erase;
	}

	void TileMap::OnRightMouseUp()
	{
		mode = Mode::Inactive;
	}
#endif
}