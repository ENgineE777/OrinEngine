
#include "TileMap.h"
#include "Root/Root.h"

#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#include "Editor/TileSetWindow.h"
#endif

namespace Oak
{
#ifdef OAK_EDITOR
	void TileMap::ShowTilsetWindow(void* owner)
	{
		TileMap* tileMap = (TileMap*)owner;

		if (tileMap->tileSet.Get())
		{
			TileSetWindow::StartEdit(tileMap->tileSet.Get());
		}
	}
#endif

	CLASSREG(SceneEntity, TileMap, "TileMap")

	META_DATA_DESC(TileMap)
		BASE_SCENE_ENTITY_PROP(TileMap)
		INT_PROP(TileMap, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		INT_PROP(TileMap, physGroup, 1, "Physics", "Physical group", "Physical group")
		ASSET_TILE_SET_PROP(TileMap, tileSet, "Visual", "TileSet")
#ifdef OAK_EDITOR
		CALLBACK_PROP(TileMap, TileMap::ShowTilsetWindow, "Properties", "Open Tileset")
#endif
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

	void TileMap::OnVisiblityChange(bool state)
	{
		for (auto* item : collition)
		{
			item->SetActive(state);
		}
	}

	void TileMap::Play()
	{
		Math::Matrix mat = transform.global;
		auto pos = mat.Pos();
		auto size = transform.size;
		size.z = 16.0f;
		size *= Sprite::pixelsPerUnitInvert;
		
		for (auto tile : tiles)
		{
			if (!tile.texture.HasCollision())
			{
				continue;
			}

			Math::Matrix mat = transform.global;
			auto pos = mat.Pos();

			int k = 0;

			mat.Pos() = pos + mat.Vx() * ((float)tile.x + 0.5f) * transform.size.x + mat.Vy() * ((float)tile.y - 0.5f) * transform.size.y;
			mat.Pos() *= Sprite::pixelsPerUnitInvert;

			PhysObject* box = root.GetPhysScene()->CreateBox(size, mat, Math::Matrix(), PhysObject::BodyType::Static, physGroup);
			box->SetActive(IsVisible());

			body.object = this;
			body.body = box;
			body.body->SetUserData(&body);

			collition.push_back(box);
		}
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

	TileMap::TileHit TileMap::IsPointHitTiles(Math::Vector2 point)
	{
		if (point.x < 0.0f)
		{
			point.x -= transform.size.x;
		}

		if (point.y > 0.0f)
		{
			point.y += transform.size.x;
		}

		int x = (int)(point.x / (transform.size.x));
		int y = (int)(point.y / (transform.size.y));

		for (auto tile : tiles)
		{
			if (tile.x == x && tile.y == y)
			{
				return !tile.texture.HasCollision() ? TileHit::Tile : TileHit::TileWithCollision;
			}
		}

		return TileHit::NoHit;
	}

	void TileMap::Release()
	{
		for (auto* item : collition)
		{
			RELEASE(item);
		}

		SceneEntity::Release();
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

	void TileMap::Copy(SceneEntity* source)
	{
		SceneEntity::Copy(source);

		tiles = ((TileMap*)source)->tiles;
	}

	void TileMap::OnMouseMove(Math::Vector2 ms)
	{
		if (mode != Mode::Inactive && TileSetWindow::tileSet)
		{
			Math::Vector3 mouseOrigin;
			Math::Vector3 mouseDirection;

			Math::GetMouseRay(ms, mouseOrigin, mouseDirection);
			auto trans = transform.GetGlobal();
			trans.Pos() *= Sprite::pixelsPerUnitInvert;

			Math::Vector3 pos;
			Math::IntersectPlaneRay(trans.Pos(), trans.Vz(), mouseOrigin, mouseDirection, pos);

			pos = (pos - trans.Pos()) * Sprite::pixelsPerUnit;

			if (pos.x < 0.0f)
			{
				pos.x -= transform.size.x;
			}

			int x = (int)(pos.x / transform.size.x);

			if (pos.y > 0.0f)
			{
				pos.y += transform.size.x;
			}

			int y = (int)(pos.y / transform.size.y);

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
				tiles.push_back({ x, y, 0, tileSet->GetSelectedTile() });
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