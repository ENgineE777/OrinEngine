
#pragma once

#include "Root/Fonts/FontRef.h"

namespace Orin
{
	class DebugFont : public Object
	{
		FontRef font;
		int font_size = 11;

		struct Text
		{
			Math::Vector3 pos;
			ScreenCorner corner;
			float dist;
			Color color;
			char text[256];
		};

		eastl::vector<Text> texts;
		eastl::vector<Text> textsIn3d;

	public:
	
		DebugFont();
		virtual ~DebugFont();

		virtual bool Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddText(eastl::vector<Text>& texts, Math::Vector3 pos, ScreenCorner corner, float dist, Color color, const char* text);
		void AddText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text);
		void AddText(Math::Vector3 pos, float dist, Color color, const char* text);
		void Draw(float dt);
		void Release();
	};
}