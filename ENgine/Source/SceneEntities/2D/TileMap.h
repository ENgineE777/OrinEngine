
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "Root/Assets/AssetTileSet.h"
#include "Root/Assets/AssetTexture.h"
#include "Root/Physics/PhysObject.h"
#include "Root/Physics/PhysScene.h"
#include "PortalMask.h"

#ifdef ORIN_EDITOR
#include "Editor/EditorAction.h"
#endif

namespace Orin
{
	class CLASS_DECLSPEC TileMap : public SceneEntity
	{
		Math::Vector2 zoneSize;
		Math::Vector2 zoneCenter;

	public:

		struct Tile
		{
			int x = 0;
			int y = 0;
			int data = 0;
			int index = -1;
			float rotation = 0.0f;
			bool flipH = false;
			bool flipV = false;
			Color emmisive = COLOR_WHITE;
			float emmisiveIntencity = 1.0f;
			AssetTextureRef texture;
		};

		bool paralaxInEditor = false;
		Math::Vector2 paralax = 1.0f;

		eastl::vector<Tile> tiles;
		eastl::vector<PhysObject*> collition;

		PhysScene::BodyUserData body;

		int cornerX = 0;
		int cornerY = 0;
		int cornerWidth = 0;
		int cornerHeight = 0;

		Color color;
		int drawLevel = 0;
		AssetTileSetRef tileSet;

		int lightGroup = 1;

		bool autoTileH = false;
		bool autoTileV = false;
		bool autoCalcTileZone = true;

		bool usePortlas = false;
		SceneEntityRef<PortalMask> portlaMask;

		META_DATA_DECL(TileMap)

		void CalcAutoTileData();

	#ifndef DOXYGEN_SKIP

		virtual ~TileMap() = default;

		void Init() override;

		void Play() override;

		void OnVisiblityChange(bool set) override;

		void ApplyProperties() override;
		void Draw(float dt);
		void DrawOccluders(float dt);
		void DrawMask(float dt);
		void DrawTiles(float dt, Math::Vector3 pos);

		void Load(JsonReader& reader) override;
		void Save(JsonWriter& writer) override;

		void Release() override;

	#ifdef ORIN_EDITOR

		class TileMapAction : public IEditorAction
		{
			eastl::vector<Tile> savedData;
			eastl::vector<Tile> savedSelectedData;
			eastl::vector<Tile> data;
			eastl::vector<Tile> selectedData;

			void ApplyTileSet(eastl::vector<Tile>& data, eastl::vector<Tile>& selectedData);

		public:

			TileMapAction(void* owner, eastl::vector<Tile>& savedTiles, eastl::vector<Tile>& savedSelectedTiles, 
							eastl::vector<Tile>& tiles, eastl::vector<Tile>& selectedTiles);
			void Apply() override;
			void Undo() override;

			void Release() override;
		};

		bool changed = false;
		eastl::vector<Tile> savedTiles;
		eastl::vector<Tile> savedSelectedTiles;

		enum class Mode
		{
			Inactive,
			Place,
			RectPlace,
			Erase,
			RectErase,
			TilesSelection,
			TilesSelected,
			TilesMove
		};

		Mode mode = Mode::Inactive;

		Math::Vector2 prevMs;
		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

		Math::Vector2 rectStart;
		Math::Vector2 rectEnd;

		Math::Vector2 deltaMove;

		eastl::vector<Tile> tilesSelected;

		Math::Vector2 MouseToWorldPos(Math::Vector2 ms);
		Math::Vector2 MouseToTile(Math::Vector2 ms);
		void RectFill();

		void AddEditorAction();

		void SetEditMode(bool ed) override;

		void Copy(SceneEntity* source) override;

		void OnMouseMove(Math::Vector2 ms) override;
		void OnLeftMouseDown() override;
		void OnLeftMouseUp() override;
		bool OnRightMouseDown() override;
		void OnRightMouseUp() override;

		bool BlockMouseButtons() override;
	#endif

	#endif
	};
}
