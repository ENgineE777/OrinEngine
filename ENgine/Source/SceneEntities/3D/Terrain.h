
#pragma once

#include "Root/Render/Render.h"
#include "Root/Scenes/SceneEntity.h"
#include "Root/Physics/PhysScene.h"

namespace Orin
{

	/**
	\page scene_object_3D_Terrain Terrain

	Terrain costracted from height map and textured by one big texture. Size is taken from
	dimention of a texture. Vertical and horizontal scales can be adjusted.

	This class ::Terrain is a representation on C++ side.

	Parameters
	----------

	Name              | Description
	------------------| -------------
	ScaleH            | Horizontal scale factor
	ScaleV            | Vertical scale factor
	Texture           | Filename of a texture
	Heightmap         | Filename of a high map
	Color             | Overlay color of a terrain

	*/


	/**
	\ingroup gr_code_scene_objects_3D
	*/

	/**
	\brief Representation of terrain in 3D space

	Terrain costracted from height map and textured by one big texture. Size is taken from
	dimention of a texture. Vertical and horizontal scales can be adjusted.

	*/

	class Terrain : public SceneEntity
	{
	public:
		META_DATA_DECL(Terrain)

		/**
		\brief Filename of a texture.
		*/

		eastl::string tex_name;

		/**
		\brief Filename of a high map.
		*/
		eastl::string hgt_name;

		/**
		\brief Horizontal scale factor.
		*/

		float scaleh;

		/**
		\brief Vertical scale factor.
		*/

		float scalev;

		/**
		\brief Overlay color of a terrain.
		*/

		Color color;

	#ifndef DOXYGEN_SKIP
		TextureRef    texture;
		VertexDeclRef vdecl;
		DataBufferRef buffer;
		int      hwidth;
		int      hheight;
		uint8_t* hmap = nullptr;

		struct VertexTri
		{
			Math::Vector3 position;
			Math::Vector2 texCoord;
			Math::Vector3 normal;
		};

		int sz;

		PhysScene::BodyUserData body;

		Terrain();
		virtual ~Terrain();

		void Init() override;
		void ApplyProperties() override;
		float GetHeight(int i, int j);
		Math::Vector3 GetVecHeight(int i, int j);
		void LoadHMap(const char* hgt_name);
		void Render(float dt);
		void ShRender(float dt);
		void Render(RenderTechnique* prg);

		void Play() override;

		void Release() override;
	#endif
	};
}
