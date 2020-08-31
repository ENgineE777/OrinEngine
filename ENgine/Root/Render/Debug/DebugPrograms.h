
#pragma once

#include "Root/Root.h"

namespace Oak
{
	class DebugPrograms
	{
	public:

		class DbgLineWithDepth : public Program
		{
		public:
			virtual const char* GetVsName() { return "debug_line_vs.shd"; };
			virtual const char* GetPsName() { return "debug_line_ps.shd"; };
		};

		class DbgLine : public DbgLineWithDepth
		{
		public:

			virtual void ApplyStates()
			{
				root.render.GetDevice()->SetDepthTest(false);
				root.render.GetDevice()->SetDepthWriting(false);
			};
		};

		class DbgTriangle : public Program
		{
		public:
			virtual const char* GetVsName() { return "debug_triangle_vs.shd"; };
			virtual const char* GetPsName() { return "debug_triangle_ps.shd"; };
		};

		class DbgTriangle2D : public Program
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

		class DbgSprite : public Program
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
	};
}