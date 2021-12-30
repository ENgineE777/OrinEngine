#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetAnimGraph2D : public Asset
	{
		friend class PointerRef<AssetAnimGraph2D>;
		friend class AssetAnimGraph2DRef;

		enum class DragMode
		{
			None,
			Field,
			MoveNode,
			MoveField,
			AddLink
		};

	public:

		enum class CLASS_DECLSPEC NodeType
		{
			AnimNode,
			LogicNode,
			GroupNode
		};

		class CLASS_DECLSPEC Link
		{
		public:
			META_DATA_DECL_BASE(Link)

			int index;
			eastl::string  name;
			bool defLink = false;
			Math::Vector2 arrowPos;
			float angle = 0.0f;
		};

		class CLASS_DECLSPEC Event
		{
		public:
			META_DATA_DECL_BASE(Event)

			int frameNumber = 0;
			eastl::string name;
			eastl::string param;
		};

		class CLASS_DECLSPEC Node
		{
		public:
			META_DATA_DECL_BASE(Node)

			NodeType type = NodeType::AnimNode;
			AssetTextureRef texture;
			bool looped = true;
			bool reversed = false;
			Math::Vector2 pos = 0.0f;
			eastl::string name;
			uint32_t object_uid = 0;
			int defLink = -1;
			eastl::vector<Link> links;
			eastl::vector<Event> events;
		};

		int defNode = -1;
		eastl::vector<Node> nodes;

		META_DATA_DECL_BASE(AssetAnimGraph2D)

		void Init() override;

		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		Math::Vector2 nodeSize = Math::Vector2(150.0f, 80.0f);
		Math::Vector2 linkSize = Math::Vector2(15.0f, 15.0f);
		int selNode = -1;
		int targetNode = -1;
		int selLink = -1;
		DragMode drag = DragMode::None;
		float camZoom = 1.0f;
		Math::Vector2 prevMs;
		Math::Vector2 camPos;
		Math::Vector2 mousePos;

		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		void Draw(float dt);
		int GetNodeIndex(Math::Vector2& ms);
		void CreateNode(NodeType type);
		void Delete();
		void MakeNodeAsDefault();

		void ImGuiProperties() override;
		void ImGui(bool viewportFocused) override;

		void OnMouseMove(Math::Vector2 ms) override;
		void OnLeftMouseDown() override;
		void OnLeftMouseUp() override;
		void OnMiddleMouseDown() override;
		void OnMiddleMouseUp() override;
		#endif
	};

	class CLASS_DECLSPEC AssetAnimGraph2DRef : public PointerRef<AssetAnimGraph2D>
	{
		AssetTextureRef curTexture;
		
		AssetAnimGraph2D::Node* gotoNode = nullptr;
		AssetAnimGraph2D::Node* curNode = nullptr;
		eastl::function<void(int, eastl::string&, eastl::string&)> onFrameChange;

	public:

		AssetAnimGraph2DRef() : PointerRef() {};
		AssetAnimGraph2DRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetAnimGraph2D*>(setPtr), _FL_) {};
		AssetAnimGraph2DRef(AssetAnimGraph2D* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};
		
		AssetAnimGraph2DRef& operator=(const AssetAnimGraph2DRef& ref)
		{
			Copy(ref);

			onFrameChange = ref.onFrameChange;

			return *this;
		}

		void SetOnFrameChangeCallback(eastl::function<void(int, eastl::string&, eastl::string&)> onFrameChange);

		void Reset();
		bool ActivateLink(const char* link);
		void GotoNode(int index);
		bool GotoNode(const char* node);

		void Draw(Transform* trans, Color clr, float dt);

		void SetupCreatedSceneEntity(SceneEntity* entity);

		Math::Vector2 GetSize();

		void LoadData(JsonReader& loader, const char* name);

	#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
	#endif
	};
}