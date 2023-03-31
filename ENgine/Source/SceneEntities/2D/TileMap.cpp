
#include "TileMap.h"
#include "DefferedLight.h"
#include "Root/Root.h"

#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#include "Editor/TileSetWindow.h"
#endif

namespace Orin
{
#ifdef ORIN_EDITOR
	TileMap::TileMapAction::TileMapAction(void* owner, eastl::vector<Tile>& savedTiles, eastl::vector<Tile>& savedSelectedTiles,
											eastl::vector<Tile>& tiles, eastl::vector<Tile>& selectedTiles) : IEditorAction(owner)
	{
		savedData = savedTiles;
		savedSelectedData = savedSelectedTiles;

		data = tiles;
		selectedData = selectedTiles;
	}

	void TileMap::TileMapAction::ApplyTileSet(eastl::vector<Tile>& data, eastl::vector<Tile>& selectedData)
	{
		auto* tileMap = reinterpret_cast<TileMap*>(owner);
		tileMap->tiles = data;
		tileMap->tilesSelected = selectedData;
	}

	void TileMap::TileMapAction::Apply()
	{
		ApplyTileSet(data, selectedData);
	}


	void TileMap::TileMapAction::Undo()
	{
		ApplyTileSet(savedData, savedSelectedData);
	}
#endif

	ENTITYREG(SceneEntity, TileMap, "2D/Sprites", "TileMap")

	META_DATA_DESC(TileMap)
		BASE_SCENE_ENTITY_PROP(TileMap)
		INT_PROP(TileMap, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		ASSET_TILE_SET_PROP(TileMap, tileSet, "Visual", "TileSet")
		BOOL_PROP(TileMap, paralaxInEditor, false, "Visual", "paralaxInEditor", "paralaxInEditor")
		FLOAT_PROP(TileMap, paralax.x, 1.0f, "Visual", "paralax X", "X-axis paralax")
		FLOAT_PROP(TileMap, paralax.y, 1.0f, "Visual", "paralax Y", "Y-axis paralax")
	META_DATA_DESC_END()

	void TileMap::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ;
	}

	void TileMap::ApplyProperties()
	{
		Math::Vector2 size = tileSet.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);

		transform.offset.x = 0.0f;
		transform.offset.y = 0.0f;

	#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
	#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&TileMap::Draw);

		Tasks(true)->AddTask(500, this, (Object::Delegate)&TileMap::DrawOccluders);
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
		Math::Matrix mat = transform.GetGlobal();
		auto pos = Sprite::ToPixels(mat.Pos());

		auto size = transform.size;
		size.z = 16.0f;

		auto sizeInUnits = size * Sprite::ToUnits(1.0f);


		for (auto tile : tiles)
		{
			if (!tile.texture.HasCollision())
			{
				continue;
			}

			Math::Matrix mat = transform.GetGlobal();
			auto pos = mat.Pos();

			mat.Pos() += Sprite::ToUnits(mat.Vx() * ((float)tile.x + 0.5f) * size.x + mat.Vy() * ((float)tile.y - 0.5f) * size.y);

			PhysObject* box = root.GetPhysScene()->CreateBox(sizeInUnits, mat, Math::Matrix(), PhysObject::BodyType::Static, tile.texture.GetPhysGroup(), tile.texture.GetPhysGroup());
			box->SetActive(IsVisible());

			body.object = this;
			body.body = box;
			body.body->SetUserData(&body);

			collition.push_back(box);
		}
	}

	void TileMap::Draw(float dt)
	{
		if (IsEditMode())
		{
			if (tileSet.Get())
			{
				TileSetWindow::StartEdit(tileSet.Get());
			}
			else
			{
				TileSetWindow::StopEdit();
			}

			lastViewportSize = {(float)root.GetRender()->GetDevice()->GetWidth(), (float)root.GetRender()->GetDevice()->GetHeight()};

			if (mode == Mode::RectPlace || mode == Mode::RectErase || mode == Mode::TilesSelection)
			{
				root.render.DebugLine2D({ rectStart.x, rectStart.y }, COLOR_GREEN, { rectEnd.x, rectStart.y }, COLOR_GREEN);
				root.render.DebugLine2D({ rectStart.x, rectEnd.y }, COLOR_GREEN, { rectEnd.x, rectEnd.y }, COLOR_GREEN);
				root.render.DebugLine2D({ rectStart.x, rectStart.y }, COLOR_GREEN, { rectStart.x, rectEnd.y }, COLOR_GREEN);
				root.render.DebugLine2D({ rectEnd.x, rectStart.y }, COLOR_GREEN, { rectEnd.x, rectEnd.y }, COLOR_GREEN);
			}
		}

		if (IsVisible())
		{
			Transform trans;
			trans.objectType = ObjectType::Object2D;
			trans.offset.x = 0.5f;
			trans.offset.y = 0.5f;
			trans.size = transform.size;

			auto size = transform.size;

			Math::Matrix mat = transform.GetGlobal();
			auto pos = Sprite::ToPixels(mat.Pos());

			Math::Matrix view;
			root.render.GetTransform(TransformStage::View, view);
			view.Inverse();

			auto camPos = Sprite::ToPixels(view.Pos());

			if (paralaxInEditor || GetScene()->IsPlaying())
			{
				pos.x = pos.x + (camPos.x - pos.x) * (1.0f - paralax.x);
				pos.y = pos.y + (camPos.y - pos.y) * (1.0f - paralax.y);
			}

			RenderTechniqueRef tech = Sprite::quadPrg;

			if (DefferedLight::hackStateEnabled && DefferedLight::gbufferTech)
			{
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "materialMap", tileSet->material ? tileSet->material.Get()->texture : nullptr);
				DefferedLight::gbufferTech->SetTexture(ShaderType::Pixel, "normalsMap", tileSet->normal ? tileSet->normal.Get()->texture : nullptr);

				Math::Matrix mat;
				DefferedLight::gbufferTech->SetMatrix(ShaderType::Pixel, "trans", &mat, 1);

				Math::Vector4 params;
				params.x = 0.0f;

				DefferedLight::gbufferTech->SetVector(ShaderType::Pixel, "params", &params, 1);

				DefferedLight::gbufferTech->SetMatrix(ShaderType::Pixel, "normalTrans", &mat, 1);

				tech = DefferedLight::gbufferTech;
			}

			for (auto& tile : tiles)
			{
				if (tile.index == -1)
				{
					continue;
				}

				trans.rotation = tile.rotation;
				trans.position = pos + mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y + Math::Vector3(0.5f, -0.5f, 0.0f) * transform.size;

				trans.size.x = size.x;
				trans.size.y = size.y;

				tile.texture.prg = tech;
				tile.texture.Draw(&trans, COLOR_WHITE, dt);
			}

			if (IsEditMode())
			{
				for (auto& tile : tilesSelected)
				{
					trans.rotation = tile.rotation;
					trans.position = pos + mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y + Math::Vector3(0.5f, -0.5f, 0.0f) * transform.size;

					trans.size.x = size.x;
					trans.size.y = size.y;

					tile.texture.prg = tech;
					tile.texture.Draw(&trans, COLOR_WHITE, dt);
				}

				for (auto& tile : tilesSelected)
				{
					Sprite::DebugRect({ tile.x * transform.size.x, tile.y * transform.size.y },
									  { (tile.x + 1) * transform.size.x, (tile.y - 1) * transform.size.y }, COLOR_WHITE);
				}

				if (tilesSelected.size() == 0 && editor.InSelectMode())
				{
					auto indices = tileSet->GetSelectedTileIndices();

					cornerX = 1000000;
					cornerY = -1000000;

					cornerWidth = -1000000;
					cornerHeight = 1000000;

					for (auto index : indices)
					{
						Math::Vector2 pos = tileSet->GetTilePos(index);

						if (pos.x < cornerX)
						{
							cornerX = (int)pos.x;
						}

						if (pos.y > cornerY)
						{
							cornerY = (int)pos.y;
						}

						if (pos.x > cornerWidth)
						{
							cornerWidth = (int)pos.x;
						}

						if (pos.y < cornerHeight)
						{
							cornerHeight = (int)pos.y;
						}
					}

					cornerWidth = cornerWidth - cornerX + 1;
					cornerHeight = cornerY - cornerHeight + 1;

					if (mode == Mode::Inactive && indices.size() > 0)
					{
						auto startPos = MouseToTile(prevMs);

						for (auto index : indices)
						{
							Math::Vector2 tilePos = tileSet->GetTilePos(index);

							trans.rotation = tileSet->GetTileRotation(index);

							float tileX = startPos.x + tilePos.x - (float)cornerX;
							float tileY = startPos.y + tilePos.y - (float)cornerY;

							trans.position = Math::Vector3(0.0f, 0.0f, -0.001f) + pos + mat.Vx() * tileX * transform.size.x + mat.Vy() * tileY * transform.size.y + Math::Vector3(0.5f, -0.5f, 0.0f) * transform.size;

							trans.size.x = size.x;
							trans.size.y = size.y;

							AssetTextureRef texRef = tileSet->GetTileTexture(index);
							texRef.prg = tech;
							texRef.Draw(&trans, COLOR_WHITE, dt);
						}

						Sprite::DebugRect({ startPos.x * transform.size.x, startPos.y * transform.size.y },
										  { (startPos.x + cornerWidth) * transform.size.x, (startPos.y - cornerHeight) * transform.size.y }, COLOR_YELLOW);
					}
				}
			}
		}
	}

	void TileMap::DrawOccluders(float dt)
	{
		auto trans = transform;

		Math::Matrix mat = trans.GetGlobal();
		auto pos = Sprite::ToPixels(mat.Pos());

		for (auto& tile : tiles)
		{
			if (!tile.texture.HasCollision() || tile.index == -1)
			{
				continue;
			}

			mat.Pos() = Sprite::ToUnits(pos + mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y);

			trans.SetGlobal(mat);
			auto sz = tile.texture.GetSize();
			trans.size.x = sz.x;
			trans.size.y = sz.y;

			tile.texture.prg = Sprite::quadPrgNoZ;
			tile.texture.Draw(&trans, COLOR_BLACK, dt);
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
			reader.Read("index", tile.index);

			if (tileSet)
			{
				tile.texture = tileSet->GetTileTexture(tile.index);
				tile.rotation = tileSet->GetTileRotation(tile.index);
			}

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
			writer.Write("index", tile.index);

			writer.FinishBlock();
		}

		writer.FinishArray();
	}

	void TileMap::Release()
	{
		for (auto* item : collition)
		{
			RELEASE(item);
		}

		SceneEntity::Release();
	}

#ifdef ORIN_EDITOR
	Math::Vector2 TileMap::MouseToWorldPos(Math::Vector2 ms)
	{
		Math::Vector3 mouseOrigin;
		Math::Vector3 mouseDirection;

		Math::GetMouseRay(ms, mouseOrigin, mouseDirection);
		auto trans = transform.GetGlobal();

		Math::Vector3 pos;
		Math::IntersectPlaneRay(trans.Pos(), trans.Vz(), mouseOrigin, mouseDirection, pos);

		pos = Sprite::ToPixels(pos - trans.Pos());

		return { pos.x, pos.y };
	}

	Math::Vector2 TileMap::MouseToTile(Math::Vector2 ms)
	{
		auto pos = MouseToWorldPos(ms);

		if (pos.x < 0.0f)
		{
			pos.x -= transform.size.x;
		}

		if (pos.y > 0.0f)
		{
			pos.y += transform.size.x;
		}

		int x = (int)(pos.x / transform.size.x);
		int y = (int)(pos.y / transform.size.y);

		return { (float)x , (float)y };
	}

	void TileMap::RectFill()
	{
		auto pos1 = MouseToTile(rectStart);
		auto pos2 = MouseToTile(rectEnd);

		int startX = (int)fmin(pos1.x, pos2.x);
		int endX = (int)fmax(pos1.x, pos2.x);

		int startY = (int)fmax(pos1.y, pos2.y);
		int endY = (int)fmin(pos1.y, pos2.y);
	
		if (mode == Mode::RectErase || mode == Mode::TilesSelection)
		{
			for (int y = startY; y >= endY; y--)
				for (int x = startX; x <= endX; x++)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						if (tiles[i].x == x && tiles[i].y == y)
						{
							if (mode == Mode::TilesSelection)
							{
								tilesSelected.push_back(tiles[i]);
							}

							tiles.erase(tiles.begin() + i);
							changed = true;

							break;
						}
					}
				}
		}
		else
		if (mode == Mode::RectPlace)
		{
			auto indices = tileSet->GetSelectedTileIndices();

			int offsetX = startX % cornerWidth;
			if (startX < 0)
			{
				offsetX = cornerWidth + offsetX;
			}

			int offsetY = cornerHeight - startY % cornerHeight;

			if (offsetY < 0)
			{
				offsetY = cornerHeight + offsetY;
			}

			for (int y = startY + offsetY; y >= endY; y -= cornerHeight)
				for (int x = startX - offsetX; x <= endX; x += cornerWidth)
				{											
					for (auto index : indices)
					{
						Math::Vector2 pos = tileSet->GetTilePos(index);

						int tileX = x + (int)pos.x - cornerX;
						int tileY = y + (int)pos.y - cornerY;

						if (tileX < startX || tileX > endX || tileY > startY || tileY < endY)
						{
							continue;
						}

						for (int i = 0; i < tiles.size(); i++)
						{
							if (tiles[i].x == tileX && tiles[i].y == tileY)
							{
								tiles.erase(tiles.begin() + i);
								changed = true;

								break;
							}
						}

						tiles.push_back({ tileX, tileY, 0, index, tileSet->GetTileRotation(index), tileSet->GetTileTexture(index) });

						changed = true;
					}
				}
		}
	}

	void TileMap::SetEditMode(bool ed)
	{
		SceneEntity::SetEditMode(ed);

		editor.ownGrid = ed;

		if (!ed)
		{
			tiles.insert(tiles.end(), tilesSelected.begin(), tilesSelected.end());
			tilesSelected.clear();

			mode = Mode::Inactive;
			TileSetWindow::StopEdit();
		}
		else
		{
			editor.gridOrigin = transform.GetGlobal().Pos();
			editor.gridStep = tileSet.GetSize();
		}
	}

	void TileMap::Copy(SceneEntity* source)
	{
		SceneEntity::Copy(source);

		tiles = ((TileMap*)source)->tiles;
	}

	void TileMap::OnMouseMove(Math::Vector2 ms)
	{
		if (TileSetWindow::tileSet)
		{
			if (mode == Mode::Erase || mode == Mode::Place)
			{
				auto pos = MouseToTile(ms);

				int x = (int)pos.x;
				int y = (int)pos.y;

				if (mode == Mode::Place && tileSet != nullptr && tileSet->IsTilesSelected())
				{
					auto indices = tileSet->GetSelectedTileIndices();

					for (auto index : indices)
					{
						Math::Vector2 pos = tileSet->GetTilePos(index);

						int tileX = x + (int)pos.x - cornerX;
						int tileY = y + (int)pos.y - cornerY;

						for (int i = 0; i < tiles.size(); i++)
						{
							if (tiles[i].x == tileX && tiles[i].y == tileY)
							{
								tiles.erase(tiles.begin() + i);
								changed = true;

								break;
							}
						}

						tiles.push_back({ tileX, tileY, 0, index, tileSet->GetTileRotation(index), tileSet->GetTileTexture(index) });
					}

					changed = true;
				}
				else
				if (mode == Mode::Erase)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						if (tiles[i].x == x && tiles[i].y == y)
						{
							tiles.erase(tiles.begin() + i);
							changed = true;

							break;
						}
					}
				}
			}
		}
		
		if (mode == Mode::TilesMove)
		{
			auto prevPos = MouseToWorldPos(prevMs);
			auto pos = MouseToWorldPos(ms);

			deltaMove += pos - prevPos;

			if (fabs(deltaMove.x) > transform.size.x || fabs(deltaMove.y) > transform.size.y)
			{
				int dx = (int)(deltaMove.x / transform.size.x);
				deltaMove.x -= dx * transform.size.x;

				int dy = (int)(deltaMove.y / transform.size.y);
				deltaMove.y -= dy * transform.size.y;

				for (auto& tile : tilesSelected)
				{
					tile.x += dx;
					tile.y += dy;

					for (int i = 0; i < tiles.size(); i++)
					{
						if (tiles[i].x == tile.x && tiles[i].y == tile.y)
						{
							tiles.erase(tiles.begin() + i);
							changed = true;

							break;
						}
					}
				}
			}
		}
		
		if (mode == Mode::RectPlace || mode == Mode::RectErase || mode == Mode::TilesSelection)
		{
			rectEnd = ms;
		}

		prevMs = ms;
	}

	void TileMap::OnLeftMouseDown()
	{
		if (!editor.InSelectMode())
		{
			return;
		}

		if (tileSet)
		{
			changed = false;
			savedTiles = tiles;
			savedSelectedTiles = tilesSelected;

			if (mode == Mode::TilesSelected)
			{
				mode = Mode::Inactive;

				auto pos = MouseToTile(prevMs);

				int x = (int)pos.x;
				int y = (int)pos.y;

				for (auto& tile : tilesSelected)
				{
					if (tile.x == x && tile.y == y)
					{
						mode = Mode::TilesMove;

						break;
					}
				}

				if (mode == Mode::Inactive)
				{
					tiles.insert(tiles.end(), tilesSelected.begin(), tilesSelected.end());
					tilesSelected.clear();
				}
			}
			else
			{
				if (root.controls.DebugKeyPressed("KEY_X", AliasAction::Pressed))
				{
					mode = Mode::TilesSelection;
					rectStart = prevMs;
				}
				else
				{
					if (tileSet->IsTilesSelected())
					{
						auto indices = tileSet->GetSelectedTileIndices();

						mode = Mode::Place;

						if (root.controls.DebugKeyPressed("KEY_Z", AliasAction::Pressed))
						{
							mode = Mode::RectPlace;
							rectStart = prevMs;
						}
					}
				}				
			}
		}
	}

	void TileMap::OnLeftMouseUp()
	{
		if (tileSet)
		{
			if (mode == Mode::RectPlace || mode == Mode::TilesSelection)
			{
				RectFill();
			}

			if (mode == Mode::TilesSelection)
			{
				mode = Mode::TilesSelected;
			}
			else
			if (mode == Mode::TilesMove)
			{
				mode = Mode::TilesSelected;
			}
			else
			{
				mode = Mode::Inactive;
			}

			AddEditorAction();
		}
	}

	bool TileMap::OnRightMouseDown()
	{
		if (!editor.InSelectMode())
		{
			return false;
		}

		changed = false;
		savedTiles = tiles;

		mode = Mode::Erase;

		if (root.controls.DebugKeyPressed("KEY_Z", AliasAction::Pressed))
		{
			mode = Mode::RectErase;
			rectStart = prevMs;
		}

		return true;
	}

	void TileMap::OnRightMouseUp()
	{
		if (mode == Mode::RectErase)
		{
			RectFill();
		}

		AddEditorAction();

		mode = Mode::Inactive;
	}

	void TileMap::AddEditorAction()
	{
		if (changed)
		{
			editor.AddAction(new TileMapAction(this, savedTiles, savedSelectedTiles, tiles, tilesSelected));
		}
	}

	bool TileMap::BlockMouseButtons()
	{
		return IsEditMode();
	}
#endif
}