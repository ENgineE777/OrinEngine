
#pragma once

#include "Root/Render/Device.h"
#include "Root/Render/Program.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include <vector>
#include <map>

namespace Oak
{
	enum class ScreenCorner
	{
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom
	};

	class Render
	{
		friend class TextureDX11;
		friend class TextureGLES;

		Device* device;
		Math::Matrix trans[4];
		bool   need_calc_trans;

		struct TextureRef
		{
			int count;
			Texture* texture;
		};

		std::map<std::string, TextureRef> textures;

		std::map<std::string, Program*> programs;

		class DebugLines*       lines;
		class DebugSpheres*     spheres;
		class DebugBoxes*       boxes;
		class DebugTriangles*   triangles;
		class DebugFont*        font;
		class DebugSprites*     sprites;
		class DebugTriangles2D* triangles2D;

		TaskExecutor::GroupTaskPool* groupTaskPool;
		TaskExecutor::SingleTaskPool* debugTaskPool;

		Texture* white_tex = nullptr;

	public:

		enum Transform
		{
			World = 0,
			View,
			Projection,
			WrldViewProj
		};

		Render();

		bool Init(const char* device, void* external_device);
		void Execute(float dt);
		void Release();

		Device* GetDevice();

		void SetTransform(Transform trans, Math::Matrix mat);

		void GetTransform(Transform trans, Math::Matrix& mat);

		Program* GetProgram(const char* name);

		Texture* LoadTexture(const char* name);

		TaskExecutor::SingleTaskPool* AddTaskPool();

		void DelTaskPool(TaskExecutor::SingleTaskPool* pool);
		void AddExecutedLevelPool(int level);
		void ExecutePool(int level, float dt);
		void DebugLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth = true);
		void DebugLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr);
		void DebugRect2D(Math::Vector2 from, Math::Vector2 to, Color color);
		void DebugSphere(Math::Vector3 pos, Color color, float radius, bool full_shade = true);
		void DebugBox(Math::Matrix pos, Color color, Math::Vector3 scale);
		void DebugTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color);
		void DebugTriangle2D(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color);
		void DebugPrintText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text, ...);
		void DebugPrintText(Math::Vector3 pos, float dist, Color color, const char* text, ...);
		void DebugSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Color color = COLOR_WHITE, Math::Vector2 offset = 0.0f, float angle = 0.0f);

		inline Texture* GetWhiteTexture()
		{
			return white_tex;
		}

		Math::Vector3 TransformToScreen(Math::Vector3 pos, int type);

	protected:
		void CalcTrans();
		bool TexRefIsEmpty(Texture* texture);
	};
}