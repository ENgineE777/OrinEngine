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

	extern CLASS_DECLSPEC float pixelsPerUnit;
	extern CLASS_DECLSPEC float pixelsPerUnitInvert;
	extern CLASS_DECLSPEC float pixelsHeight;

	extern CLASS_DECLSPEC ProgramRef quadPrg;
	extern CLASS_DECLSPEC ProgramRef quadPrgNoZ;

	CLASS_DECLSPEC void Init();
	CLASS_DECLSPEC void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, ProgramRef prg = quadPrg);
	CLASS_DECLSPEC void Release();
}
#endif