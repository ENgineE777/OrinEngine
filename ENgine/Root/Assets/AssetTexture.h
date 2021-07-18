#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetTexture : public Asset
	{
		friend class PointerRef<AssetTexture>;
		friend class AssetTextureRef;

		TextureFilter textureFilter = TextureFilter::Linear;
		TextureAddress texturMode = TextureAddress::Wrap;

		static void StartEditAssetTexture(void* owner);

	public:

		struct CLASS_DECLSPEC Slice
		{
			eastl::string name;
			bool isNineSliced = 0;
			Math::Vector2 pos = 0.0f;
			Math::Vector2 size = 0.0f;
			Math::Vector2 upLeftOffset = 10.0f;
			Math::Vector2 downRightOffset = 10.0f;
			Math::Vector2 offset = 0.5f;
		};

		struct CLASS_DECLSPEC Frame
		{
			int slice = -1;
			float frameLength = 1.0f;
			Math::Vector2 offset = 0.0f;
		};

		struct CLASS_DECLSPEC Animation
		{
			int fps = 15;
			eastl::string name;
			eastl::vector<Frame> frames;

			bool AdvanceFrame(float dt, int& currentFrame, float& currentTime, bool looped, bool reversed, eastl::function<void(int)> onFrameChange);
		};

		TextureRef texture;
		Math::Vector2 size;
		eastl::vector<Slice> slices;
		eastl::vector<Animation> animations;

		META_DATA_DECL_BASE(AssetTexture)

		TextureRef GetTexture();
		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif
	};

	class CLASS_DECLSPEC AssetTextureRef : public PointerRef<AssetTexture>
	{
		int animPlayFrame = 0;
		float animPlayTime = 0.0f;
		bool animLooped = true;
		bool animFinished = false;
		bool animReversed = false;
		eastl::function<void(int)> onFrameChange;

		#ifdef OAK_EDITOR
		char name[256];
		int previewAnimPlaySlice = 0;
		float previewAnimPlayTime = 0.0f;
		#endif

	public:

		AssetTextureRef() : PointerRef() {};
		AssetTextureRef(Asset* setPtr, const char* file, int line) : PointerRef(reinterpret_cast<AssetTexture*>(setPtr), _FL_) {};
		AssetTextureRef(AssetTexture* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		int sliceIndex = -1;
		int animIndex = -1;

		void Draw(Transform* trans, Color clr, float dt);

		const char* GetName();
		void SetupCreatedSceneEntity(SceneEntity* entity);

		Math::Vector2 GetSize();

		void ResetAnim(bool looped, bool reversed, eastl::function<void(int)> onFrameChange);
		bool IsAnimFinished();

		void LoadData(JsonReader& loader, const char* name);

	#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
		void ImGuiImage(float size);
	#endif

	};
}