#pragma once

#include "Support/Support.h"
#include "Root/Files/Files.h"
#include "Root/Render/Render.h"

/**
\ingroup gr_code_common
*/

namespace Oak
{
	class CLASS_DECLSPEC Sprite
	{
		friend class SceneManager;
		friend class Project;
		friend class Editor;

#ifndef DOXYGEN_SKIP
		static float _pixelsPerUnit;
		static float _pixelsPerUnitInvert;
		static float _pixelsHeight;

		static VertexDeclRef _vdecl;
		static DataBufferRef _buffer;

		static void SetData(float pixelsHeight, float pixelsPerUnit);

	public:

		static ProgramRef quadPrg;
		static ProgramRef quadPrgNoZ;

		static void Init();
		static void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, ProgramRef prg = quadPrg);

		static inline float GetPixelsHeight() { return _pixelsHeight; };

		template<typename T>
		static inline T ToUnits(const T& val) { return val * _pixelsPerUnitInvert; }

		template<typename T>
		static inline T ToPixels(const T& val) { return val * _pixelsPerUnit; }

		static void DebugLine(const Math::Vector3& from, const Math::Vector3& to, const Color& color);
		static void DebugSphere(const Math::Vector3& pos, float radius, const Color& color);
		static void DebugRect(const Math::Vector2& p1, const Math::Vector2& p2, Color color);

		static void Release();
#endif
	};
}