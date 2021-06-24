#include "Root/Root.h"
#include "SceneEntities/2D/SpriteEntity.h"


#ifdef OAK_EDITOR
#include "imgui.h"
#endif

namespace Oak
{
#ifdef OAK_EDITOR
	extern void ShowSpriteWindow(AssetTexture* texture);
#endif

	CLASSREG(Asset, AssetTexture, "AssetTexture")

	META_DATA_DESC(AssetTexture)
		ENUM_PROP(AssetTexture, textureFilter, TextureFilter::Linear, "Properties", "Texture filter", "Texture filter")
			ENUM_ELEM("Point", TextureFilter::Point)
			ENUM_ELEM("Linear", TextureFilter::Linear)
		ENUM_END
		ENUM_PROP(AssetTexture, texturMode, TextureAddress::Wrap, "Properties", "Texture filter", "Texture filter")
			ENUM_ELEM("Wrap", TextureAddress::Wrap)
			ENUM_ELEM("Clamp", TextureAddress::Clamp)
			ENUM_ELEM("Mirror", TextureAddress::Mirror)
		ENUM_END
		CALLBACK_PROP(AssetTexture, AssetTexture::StartEditAssetTexture, "Properties", "Sprite Editor")
	META_DATA_DESC_END()

	bool AssetTexture::Animation::AdvanceFrame(float dt, int& currentFrame, float& currentTime, bool looped, bool reversed, eastl::function<void(int)> onFrameChange)
	{
		if (frames.size() > 1)
		{
			if (currentFrame >= frames.size())
			{
				currentFrame = 0;
			}

			currentTime += dt;

			while (currentTime > frames[currentFrame].frameLength / (float)fps)
			{
				currentTime -= frames[currentFrame].frameLength / (float)fps;

				if (reversed)
				{
					currentFrame--;

					if (currentFrame < 0)
					{
						if (looped)
						{
							currentFrame = (int)(frames.size()) - 1;
						}
						else
						{
							currentFrame = 0;

							if (onFrameChange)
							{
								onFrameChange(currentFrame);
							}
							return true;
						}
					}
				}
				else
				{
					currentFrame++;
	
					if (currentFrame >= frames.size())
					{
						if (looped)
						{
							currentFrame = 0;
						}
						else
						{
							currentFrame = (int)frames.size() - 1;

							if (onFrameChange)
							{
								onFrameChange(currentFrame);
							}

							return true;
						}
					}
				}

				if (onFrameChange)
				{
					onFrameChange(currentFrame);
				}
			}
		}
		else
		{
			currentFrame = 0;
		}

		return false;
	}

	void AssetTexture::StartEditAssetTexture(void* owner)
	{
#ifdef OAK_EDITOR
		ShowSpriteWindow((AssetTexture*)owner);
#endif
	}

	TextureRef AssetTexture::GetTexture()
	{
		return texture;
	}

	void AssetTexture::Reload()
	{
		if (texture.Get())
		{
			root.render.LoadTexture(texture, path.c_str());
		}
		else
		{
			texture = root.render.LoadTexture(path.c_str(), _FL_);
		}

		if (texture.Get())
		{
			texture->SetFilters(textureFilter, textureFilter);
			texture->SetAdress(texturMode);
			size = Math::Vector2((float)texture->GetWidth(), (float)texture->GetHeight());
		}
	}

	void AssetTexture::LoadData(JsonReader& loader)
	{
		if (loader.EnterBlock("spriteSheet"))
		{
			Math::Vector2 size = 0.0f;

			loader.Read("sizeX", size.x);
			loader.Read("sizeY", size.y);

			int count = 0;
			loader.Read("count", count);
			slices.resize(count);

			for (int i = 0; i < count; i++)
			{
				Slice& slice = slices[i];

				loader.EnterBlock("Sheet");

				loader.Read("name", slice.name);
				loader.Read("isNineSliced", slice.isNineSliced);
				loader.Read("pos", slice.pos);
				loader.Read("size", slice.size);
				loader.Read("upLeftOffset", slice.upLeftOffset);
				loader.Read("downRightOffset", slice.downRightOffset);
				loader.Read("offset", slice.offset);

				loader.LeaveBlock();
			}

			count = 0;
			loader.Read("anim_count", count);
			animations.resize(count);

			for (int i = 0; i < count; i++)
			{
				Animation& anim = animations[i];

				loader.EnterBlock("Animations");

				loader.Read("name", anim.name);
				loader.Read("fps", anim.fps);

				int frames_count = 0;
				loader.Read("frames_count", frames_count);
				anim.frames.resize(frames_count);

				for (int j = 0; j < frames_count; j++)
				{
					loader.EnterBlock("frames");

					Frame& frame = anim.frames[j];

					loader.Read("slice", frame.slice);
					loader.Read("frame_length", frame.frameLength);
					loader.Read("offset", frame.offset);

					loader.LeaveBlock();
				}

				loader.LeaveBlock();
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetTexture::SaveData(JsonWriter& saver)
	{
		saver.StartBlock("spriteSheet");

		saver.Write("sizeX", size.x);
		saver.Write("sizeY", size.y);

		int count = (int)slices.size();
		saver.Write("count", count);

		saver.StartArray("Sheet");

		for (int i = 0; i < count; i++)
		{
			Slice& slice = slices[i];

			saver.StartBlock(nullptr);

			saver.Write("name", slice.name);
			saver.Write("isNineSliced", slice.isNineSliced);
			saver.Write("pos", slice.pos);
			saver.Write("size", slice.size);
			saver.Write("upLeftOffset", slice.upLeftOffset);
			saver.Write("downRightOffset", slice.downRightOffset);
			saver.Write("offset", slice.offset);

			saver.FinishBlock();
		}

		saver.FinishArray();

		count = (int)animations.size();
		saver.Write("anim_count", count);

		saver.StartArray("Animations");

		for (int i = 0; i < count; i++)
		{
			Animation& anim = animations[i];

			saver.StartBlock(nullptr);

			saver.Write("name", anim.name);
			saver.Write("fps", anim.fps);
			
			int frames_count = (int)anim.frames.size();
			saver.Write("frames_count", frames_count);

			if (frames_count > 0)
			{
				saver.StartArray("frames");

				for (int j = 0; j < frames_count; j++)
				{
					saver.StartBlock(nullptr);

					Frame& frame = anim.frames[j];

					saver.Write("slice", frame.slice);
					saver.Write("frame_length", frame.frameLength);
					saver.Write("offset", frame.offset);

					saver.FinishBlock();
				}

				saver.FinishArray();
			}

			saver.FinishBlock();
		}

		saver.FinishArray();

		saver.FinishBlock();
	}

	const char* AssetTexture::GetSceneEntityType()
	{
		return "Sprite";
	}
	#endif

	void AssetTextureRef::Draw(Transform* trans, Color clr, float dt)
	{
		if (!Get())
		{
			return;
		}

		Math::Matrix local_trans = trans->global;
		Math::Vector3 pos3d = Math::Vector3(trans->offset.x, trans->offset.y, trans->offset.z) * trans->size * Math::Vector3(-1.0f, 1.0f, -1.0f);
		Math::Vector2 pos = Math::Vector2(pos3d.x, pos3d.y);
		Math::Vector2 size = Math::Vector2(trans->size.x, trans->size.y);

		if (sliceIndex != -1 && sliceIndex < Get()->slices.size())
		{
			AssetTexture::Slice& slice = Get()->slices[sliceIndex];

			if (slice.isNineSliced)
			{
				float x[] = { 0, slice.upLeftOffset.x, size.x - slice.downRightOffset.x, size.x };
				float y[] = { 0, slice.upLeftOffset.y, size.y - slice.downRightOffset.y, size.y };

				float u[] = { 0, slice.upLeftOffset.x, slice.size.x - slice.downRightOffset.x, slice.size.x };
				float v[] = { 0, slice.upLeftOffset.y, slice.size.y - slice.downRightOffset.y, slice.size.y };

				int index = 0;
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
					{
						Math::Vector2 slicePos = pos + Math::Vector2(x[j], -y[i]);
						Math::Vector2 sliceSize = Math::Vector2(x[j + 1] - x[j], y[i + 1] - y[i]);

						Math::Vector2 sliceUVPos = slice.pos + Math::Vector2(u[j], v[i]);
						Math::Vector2 sliceUVSize = Math::Vector2(u[j + 1] - u[j], v[i + 1] - v[i]);

						Sprite::Draw(Get()->texture, clr, local_trans, slicePos, sliceSize, sliceUVPos / Get()->size, sliceUVSize / Get()->size, true);
						index++;
					}
			}
			else
			{
				Sprite::Draw(Get()->texture, clr, local_trans, pos, size, Math::Vector2(slice.pos.x, slice.pos.y) / Get()->size, slice.size / Get()->size, true);
			}
		}
		else
		if (animIndex != -1 && animIndex < Get()->animations.size())
		{
			auto& anim = Get()->animations[animIndex];

			if (!animFinished)
			{
				animFinished = anim.AdvanceFrame(dt, animPlayFrame, animPlayTime, animLooped, animReversed, onFrameChange);
			}

			auto& frame = anim.frames[animPlayFrame];
			AssetTexture::Slice& slice = Get()->slices[frame.slice];

			trans->size.x = slice.size.x;
			trans->size.y = slice.size.y;

			pos3d = Math::Vector3(trans->offset.x, trans->offset.y, trans->offset.z) * trans->size * Math::Vector3(-1.0f, 1.0f, -1.0f);
			pos = Math::Vector2(pos3d.x, pos3d.y);
			size = Math::Vector2(trans->size.x, trans->size.y);

			Sprite::Draw(Get()->texture, clr, local_trans, pos + Math::Vector2(frame.offset.x, -frame.offset.y), slice.size, Math::Vector2(slice.pos.x, slice.pos.y) / Get()->size, slice.size / Get()->size, true);
		}
		else
		{
			Sprite::Draw(Get()->texture, clr, local_trans, pos, size, 0.0f, 1.0f, true);
		}
	}

	const char* AssetTextureRef::GetName()
	{
		if (!Get())
		{
			return "null";
		}

		StringUtils::Copy(name, 256, Get()->name.c_str());
		StringUtils::RemoveExtension(name);

		if (sliceIndex != -1 && sliceIndex < Get()->slices.size())
		{
			StringUtils::Cat(name, 256, "_");
			StringUtils::Cat(name, 256, Get()->slices[sliceIndex].name.c_str());
		}
		else
		if (animIndex != -1 && animIndex < Get()->animations.size())
		{
			StringUtils::Cat(name, 256, "_");
			StringUtils::Cat(name, 256, Get()->animations[animIndex].name.c_str());
		}

		return name;
	}

	void AssetTextureRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		SpriteEntity* sprite = reinterpret_cast<SpriteEntity*>(entity);

		if (sprite)
		{
			sprite->texture = *this;

			entity->SetName(GetName());
		}
	}

	Math::Vector2 AssetTextureRef::GetSize()
	{
		if (!Get())
		{
			return 50.0f;
		}

		if (animIndex != -1 && animIndex < Get()->animations.size())
		{
			auto& anim = Get()->animations[animIndex];

			if (anim.frames.size() > 0)
			{
				return Get()->slices[anim.frames[animPlayFrame].slice].size;
			}
			else
			{
				return 50.0f;
			}
		}
		else
		if (sliceIndex == -1 || sliceIndex >= Get()->slices.size())
		{
			return Get()->size;
		}

		AssetTexture::Slice& slice = Get()->slices[sliceIndex];
		return slice.size;
	}

	void AssetTextureRef::ResetAnim(bool looped, bool reversed, eastl::function<void(int)> setOnFrameChange)
	{
		if (!Get())
		{
			return;
		}

		animPlayFrame = reversed ? 0 : ((int)Get()->animations[animIndex].frames.size() - 1);
		animPlayTime = 0.0f;
		animLooped = looped;
		animReversed = reversed;
		animFinished = false;

		onFrameChange = setOnFrameChange;

		if (onFrameChange)
		{
			onFrameChange(animPlayFrame);
		}
	}

	bool AssetTextureRef::IsAnimFinished()
	{
		return animFinished;
	}

	void AssetTextureRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = Oak::root.assets.GetAssetRef<AssetTextureRef>(path);

				loader.Read("sliceIndex", sliceIndex);
				loader.Read("animIndex", animIndex);
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetTextureRef::SaveData(JsonWriter& saver, const char* name)
	{
		if (Get())
		{
			saver.StartBlock(name);
			saver.Write("Path", Get()->GetPath().c_str());
			saver.Write("sliceIndex", sliceIndex);
			saver.Write("animIndex", animIndex);
			saver.FinishBlock();
		}
	}

	void AssetTextureRef::ImGuiImage(float size)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		float k = 1.0f;
		Math::Vector2 uv = 0.0f;
		Math::Vector2 duv = 1.0f;
		float sliceSizeX = 1.0f;
		Math::Vector2 offset = 0.0f;

		int curSlice = -1;

		if (sliceIndex != -1 && sliceIndex < Get()->slices.size())
		{
			curSlice = sliceIndex;
		}
		else
		if (animIndex != -1 && animIndex < Get()->animations.size())
		{
			auto& anim = Get()->animations[animIndex];

			anim.AdvanceFrame(root.GetDeltaTime(), previewAnimPlaySlice, previewAnimPlayTime, true, animReversed, nullptr);

			auto& frame = anim.frames[previewAnimPlaySlice];

			curSlice = frame.slice;

			offset = frame.offset;
		}

		if (curSlice != -1)
		{
			auto& slice = Get()->slices[curSlice];

			k = slice.size.x / slice.size.y;
			uv = Math::Vector2(slice.pos.x, slice.pos.y) / Get()->size;
			duv = slice.size / Get()->size;
			sliceSizeX = slice.size.x;
		}
		else
		{
			k = GetSize().x / GetSize().y;
		}

		ImVec2 p = ImGui::GetCursorScreenPos();

		ImVec2 sz = k > 1.0f ? ImVec2(size, size / k) : ImVec2(size * k, size);
		float scale = sz.x / sliceSizeX;
		ImVec2 pos = ImVec2(p.x + (size - sz.x) * 0.5f + offset.x * scale, p.y + (size - sz.y) * 0.5f + offset.y * scale);

		drawList->AddImage(Get()->GetTexture()->GetNativeResource(), pos, ImVec2(pos.x + sz.x, pos.y + sz.y), ImVec2(uv.x, uv.y), ImVec2(uv.x + duv.x, uv.y + duv.y));

		ImGui::Image(nullptr, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

		drawList->AddText(ImVec2(p.x + 4, p.y + 1), IM_COL32_WHITE, GetName());
	}
	#endif
};