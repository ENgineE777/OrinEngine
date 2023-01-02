
#pragma once

#include "Root/Root.h"

namespace Orin::DebugTechiques
{
	class LineWithDepth : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "debug_line_vs.shd"; };
		virtual const char* GetPsName() { return "debug_line_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
		}
	};

	class Line : public LineWithDepth
	{
	public:

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	class Triangle : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "debug_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "debug_triangle_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
		}
	};

	class Triangle2D : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "debug_triangle2D_vs.shd"; };
		virtual const char* GetPsName() { return "debug_triangle2D_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	class Sprite : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "debug_sprite_vs.shd"; };
		virtual const char* GetPsName() { return "debug_sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};
}