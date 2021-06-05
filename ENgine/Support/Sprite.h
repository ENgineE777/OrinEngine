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

	void Init();
	void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv);
	void Release();
}
#endif