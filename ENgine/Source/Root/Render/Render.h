
#pragma once

#include "IRender.h"
#include "Root/Render/Device.h"
#include "Root/Render/RenderTechnique.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include <eastl/vector.h>
#include <eastl/map.h>
#include <typeinfo>

namespace Oak
{
	class Render : public IRender
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

		eastl::map<std::size_t, RenderTechnique*> renderTechniques;

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

		Device* GetDevice() override;

		void SetTransform(TransformStage trans, Math::Matrix mat) override;

		void GetTransform(TransformStage trans, Math::Matrix& mat) override;

		template<class T>
		RenderTechniqueRef GetRenderTechnique(const char* file, int line)
		{
			auto hash = typeid(T).hash_code();

			auto iter = renderTechniques.find(hash);

			if (iter == renderTechniques.end())
			{
				T* renderTechnique = new T();
				renderTechnique->Init();
				renderTechnique->hash = hash;

				renderTechniques[hash] = renderTechnique;

				return RenderTechniqueRef(renderTechnique, _FL_);
			}

			return RenderTechniqueRef(iter->second, _FL_);
		}

		TextureRef LoadTexture(const char* name, const char* file, int line) override;

		void LoadTexture(TextureRef& texture, const char* name) override;

		TaskExecutor::SingleTaskPool* AddTaskPool(const char* file, int line) override;

		void DelTaskPool(TaskExecutor::SingleTaskPool* pool) override;

		void AddExecutedLevelPool(int level) override;

		void ExecutePool(int level, float dt) override;

		void DebugLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth = true) override;

		void DebugLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr) override;

		void DebugRect2D(Math::Vector2 from, Math::Vector2 to, Color color) override;

		void DebugSphere(Math::Vector3 pos, Color color, float radius, bool full_shade = true) override;

		void DebugBox(Math::Matrix transform, Color color, Math::Vector3 scale) override;

		void DebugTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color) override;

		void DebugTriangle2D(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color) override;

		void DebugPrintText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text, ...) override;

		void DebugPrintText(Math::Vector3 pos, float dist, Color color, const char* text, ...) override;

		void DebugSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Color color = COLOR_WHITE, Math::Vector2 offset = 0.0f, float angle = 0.0f) override;

		TextureRef GetWhiteTexture() override;

		Math::Vector3 TransformToScreen(Math::Vector3 pos, int type) override;

	protected:

		#ifndef DOXYGEN_SKIP

		void CalcTrans();

		#endif
	};
}