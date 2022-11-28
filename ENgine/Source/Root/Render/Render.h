
#pragma once

#include "Root/Render/Device.h"
#include "Root/Render/RenderTechnique.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include <eastl/vector.h>
#include <eastl/map.h>
#include <typeinfo>

namespace Oak
{
	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC ScreenCorner
	{
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC TransformStage
	{
		World = 0 /*!< World trasformation */,
		View /*!< View trasformation */,
		Projection /*!< Projection trasformation */,
		WrldViewProj /*!< Final transformation */
	};

	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief Render

	This is main class of render system.

	*/

	class CLASS_DECLSPEC Render
	{
		friend class Root;
		friend class RenderTechnique;
		friend class Texture;
		friend class TextureDX11;
		friend class TextureGLES;

#ifndef DOXYGEN_SKIP

		Device* device = nullptr;
		Math::Matrix trans[4];
		bool needCalcTrans = true;

		eastl::map<eastl::string, Texture*> textures;

		struct RenderTechniqueHolder
		{
			RenderTechnique* renderTechnique = nullptr;
			bool createdFromEngine = false;
			bool dirty = false;
		};

		eastl::map<std::size_t, RenderTechniqueHolder> renderTechniques;

		class DebugLines* lines;
		class DebugSpheres* spheres;
		class DebugBoxes* boxes;
		class DebugTriangles* triangles;
		class DebugFont* font;
		class DebugSprites* sprites;
		class DebugTriangles2D* triangles2D;

		TaskExecutor::GroupTaskPool* groupTaskPool;
		TaskExecutor::SingleTaskPool* debugTaskPool;

		TextureRef whiteTex;

		bool Init(const char* device, void* external_device);
		void Release();
		void Execute(float dt);

#endif

	public:

		Device* GetDevice();

		void SetTransform(TransformStage trans, Math::Matrix mat);

		void GetTransform(TransformStage trans, Math::Matrix& mat);

		template<class T>
		RenderTechniqueRef GetRenderTechnique(const char* file, int line)
		{
			auto hash = typeid(T).hash_code();

			auto iter = renderTechniques.find(hash);

			if (iter == renderTechniques.end())
			{
				RenderTechniqueHolder holder;

				T* renderTechnique = new T();
				renderTechnique->Init();
				renderTechnique->hash = hash;

				holder.renderTechnique = renderTechnique;
				holder.createdFromEngine = strstr(file, "Code\\") == nullptr;

				renderTechniques[hash] = holder;

				return RenderTechniqueRef(renderTechnique, file, line);
			}

			RenderTechniqueHolder& holder = iter->second;

			if (holder.dirty)
			{
				int refCounter = holder.renderTechnique->refCounter;
				delete holder.renderTechnique;

				T* renderTechnique = new T();
				renderTechnique->Init();
				renderTechnique->hash = hash;
				renderTechnique->refCounter = refCounter;

				holder.renderTechnique = renderTechnique;
				holder.dirty = false;
			}

			return RenderTechniqueRef(holder.renderTechnique, file, line);
		}

		void InvalidateNonEngineTechiques();

		TextureRef LoadTexture(const char* name, const char* file, int line);

		void LoadTexture(TextureRef& texture, const char* name);

		TaskExecutor::SingleTaskPool* AddTaskPool(const char* file, int line);

		void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

		void AddExecutedLevelPool(int level);

		void ExecutePool(int level, float dt);

		void DebugLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth = true);

		void DebugLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr);

		void DebugRect2D(Math::Vector2 from, Math::Vector2 to, Color color);

		void DebugSphere(Math::Vector3 pos, Color color, float radius, bool full_shade = true);

		void DebugBox(Math::Matrix transform, Color color, Math::Vector3 scale);

		void DebugTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color);

		void DebugTriangle2D(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color);

		void DebugPrintText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text, ...);

		void DebugPrintText(Math::Vector3 pos, float dist, Color color, const char* text, ...);

		void DebugSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Color color = COLOR_WHITE, Math::Vector2 offset = 0.0f, float angle = 0.0f);

		TextureRef GetWhiteTexture();

		Math::Vector3 TransformToScreen(Math::Vector3 pos, int type);

	protected:

		#ifndef DOXYGEN_SKIP

		void CalcTrans();

		#endif
	};
}