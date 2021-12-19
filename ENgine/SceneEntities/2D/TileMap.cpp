
#include "TileMap.h"
#include "Root/Root.h"

#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#include "Editor/TileSetWindow.h"
#endif

namespace Oak
{
	CLASSREG(SceneEntity, TileMap, "TileMap")

	META_DATA_DESC(TileMap)
		BASE_SCENE_ENTITY_PROP(TileMap)
		INT_PROP(TileMap, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		ASSET_TILE_SET_PROP(TileMap, tileSet, "Visual", "TileSet")
	META_DATA_DESC_END()

	void TileMap::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;

		transform.transformFlag = MoveXYZ;
	}

	void TileMap::ApplyProperties()
	{
		Math::Vector2 size = tileSet.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);

		transform.offset.x = 0.0f;
		transform.offset.y = 0.0f;

	#ifdef OAK_EDITOR
		Tasks(true)->DelAllTasks(this);
	#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&TileMap::Draw);
	}

	void TileMap::Draw(float dt)
	{
#ifdef OAK_EDITOR
		if (IsEditMode())
		{
			editor.gridOrigin = transform.GetGlobal().Pos();
			editor.gridStep = tileSet.GetSize();
		}
#endif

		if (IsVisible())
		{
			auto trans = transform;

			Math::Matrix mat = trans.global;
			auto pos = mat.Pos();

			for (auto tile : tiles)
			{
				mat.Pos() = pos + mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y;

				trans.global = mat;
				auto sz = tile.texture.GetSize();
				trans.size.x = sz.x;
				trans.size.y = sz.y;

				tile.texture.Draw(&trans, COLOR_WHITE, dt);
			}
		}
	}

	void TileMap::Load(JsonReader& reader)
	{
		SceneEntity::Load(reader);

		int count = 0;
		reader.Read("count", count);
		tiles.resize(count);

		for (int i = 0; i < count; i++)
		{
			Tile& tile = tiles[i];

			reader.EnterBlock("Tile");

			reader.Read("x", tile.x);
			reader.Read("y", tile.y);
			tile.texture.LoadData(reader, "Texture");

			reader.LeaveBlock();
		}
	}

	void TileMap::Save(JsonWriter& writer)
	{
		SceneEntity::Save(writer);

		int count = (int)tiles.size();
		writer.Write("count", count);

		writer.StartArray("Tile");

		for (int i = 0; i < count; i++)
		{
			Tile& tile = tiles[i];

			writer.StartBlock(nullptr);

			writer.Write("x", tile.x);
			writer.Write("y", tile.y);
			tile.texture.SaveData(writer, "Texture");

			writer.FinishBlock();
		}

		writer.FinishArray();
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
		else
		{
			if (tileSet.Get())
			{
				TileSetWindow::StartEdit(tileSet.Get());
			}
		}
	}

	void TileMap::Copy(SceneEntity* source)
	{
		SceneEntity::Copy(source);

		tiles = ((TileMap*)source)->tiles;
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

			for (int i = 0; i < tiles.size(); i++)
			{
				if (tiles[i].x == x && tiles[i].y == y)
				{
					tiles.erase(tiles.begin() + i);

					break;
				}
			}

			if (mode == Mode::Place && tileSet != nullptr && tileSet->IsTileSelected())
			{
				tiles.push_back({ x, y, tileSet->GetSelectedTile() });
			}
		}
	}

	void TileMap::OnLeftMouseDown()
	{
		if (tileSet.Get())
		{
			mode = Mode::Place;
		}
	}

	void TileMap::OnLeftMouseUp()
	{
		if (tileSet.Get())
		{
			mode = Mode::Inactive;
		}
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