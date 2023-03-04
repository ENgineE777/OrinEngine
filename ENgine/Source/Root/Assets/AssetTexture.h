#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"
#include "support/Transform.h"

namespace Orin
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
			bool hasCollision = false;
			int physGroup = 1;
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

		void LoadMetaData(JsonReader& loader) override;

		#ifdef ORIN_EDITOR
		void SaveMetaData(JsonWriter& saver) override;
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

		#ifdef ORIN_EDITOR
		char name[256];
		int previewAnimPlaySlice = 0;
		float previewAnimPlayTime = 0.0f;
		#endif

	public:

		AssetTextureRef() : PointerRef() {};
		AssetTextureRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetTexture*>(setPtr), _FL_) {};
		AssetTextureRef(AssetTexture* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		int sliceIndex = -1;
		int animIndex = -1;
		RenderTechniqueRef prg = Sprite::quadPrg;

		AssetTextureRef& operator=(const AssetTextureRef& ref)
		{
			Copy(ref);

			sliceIndex = ref.sliceIndex;
			animIndex = ref.animIndex;
			prg = ref.prg;

			return *this;
		}

		bool operator==(const AssetTextureRef& rhs) const
		{
			return Get() == rhs.Get() && sliceIndex == rhs.sliceIndex && animIndex == rhs.animIndex;
		}

		void Draw(Transform* trans, Color clr, float dt);

		const char* GetName();
		void SetupCreatedSceneEntity(SceneEntity* entity);

		Math::Vector2 GetSize();
		Math::Vector2 GetFrameOffset();
		bool HasCollision();
		int  GetPhysGroup();

		void ResetAnim(bool looped, bool reversed, eastl::function<void(int)> onFrameChange);
		bool IsAnimFinished();

		void LoadData(JsonReader& loader, const char* name);

	#ifdef ORIN_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
		void ImGuiImage(float size);
	#endif

	};
}