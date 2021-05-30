#pragma once

#include "Support/Support.h"
#include "Root/Files/Files.h"
#include "Root/Render/Render.h"
#include "Support/Transform.h"

/**
\ingroup gr_code_common
*/

namespace Oak::Sprite
{
#ifndef DOXYGEN_SKIP

	extern float pixelsPerUnit;
	extern float pixelsPerUnitInvert;
	extern float pixelsHeight;

	struct Slice
	{
		eastl::string name;
		bool isNineSliced = 0;
		Math::Vector2 pos = 0.0f;
		Math::Vector2 size = 0.0f;
		Math::Vector2 upLeftOffset = 0.0f;
		Math::Vector2 downRightOffset = 0.0f;
		Math::Vector2 offset = 0.5f;
	};

	struct Sheet
	{
		TextureRef texture;
		Math::Vector2 size = 0.0f;

		eastl::vector<Slice> slices;
	};

	/*struct FrameState
	{
		bool  looped = true;
		bool  finished = false;
		bool  reversed = false;
		bool  horz_flipped = false;
		int   curFrame = 0;
		float curTime = -1.0f;
		float frameStartTime = 0.0f;
	};*/

	void Load(JsonReader& loader, Sprite::Sheet* sheet, const char* name);
	void Save(JsonWriter& saver, Sprite::Sheet* sheet, const char* name);
 
	/*template<typename Func>
	static void UpdateFrame(Sprite::Data* data, FrameState* state, float dt, Func callback)
	{
		if (data->type != Type::Frames)
		{
			return;
		}

		if (state->curTime < -0.5f || state->curFrame >= data->frames.size())
		{
			state->curTime = 0.0f;
			state->finished = false;
			state->frameStartTime = 0.0f;

			if (state->reversed)
			{
				state->curFrame = (int)(data->frames.size() - 1);
			}
			else
			{
				state->curFrame = 0;
			}
		}

		if (state->finished)
		{
			return;
		}

		float frame_tm = (data->frames[state->curFrame].time < 0.01f) ? data->frameTime : data->frames[state->curFrame].time;

		while (state->curTime + dt > state->frameStartTime + frame_tm)
		{
			state->frameStartTime += frame_tm;

			dt -= (state->frameStartTime - state->curTime);

			callback(state->curTime, state->frameStartTime);

			state->curTime = state->frameStartTime;

			if (state->reversed)
			{
				state->curFrame--;

				if (state->curFrame < 0)
				{
					if (state->looped)
					{
						state->curFrame = (int)(data->frames.size() - 1);
						state->curTime = 0.0f;
						state->frameStartTime = 0.0f;
					}
					else
					{
						state->finished = true;
						state->curFrame++;
						return;
					}
				}
			}
			else
			{
				state->curFrame++;
				if (state->curFrame >= data->frames.size())
				{
					if (state->looped)
					{
						state->curFrame = 0;
						state->curTime = 0.0f;
						state->frameStartTime = 0.0f;
					}
					else
					{
						state->finished = true;
						state->curFrame--;
						return;
					}
				}
			}

			frame_tm = (data->frames[state->curFrame].time < 0.01f) ? data->frameTime : data->frames[state->curFrame].time;
		}

		callback(state->curTime, state->curTime + dt);

		state->curTime += dt;
	}*/

	void Init();
	//void UpdateFrame(Sprite::Data* data, FrameState* state, float dt);

	void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv);
	void Draw(Transform* trans, Color clr, Sprite::Sheet* sprite, int sheetIndex = -1);

	void Release();
}
#endif