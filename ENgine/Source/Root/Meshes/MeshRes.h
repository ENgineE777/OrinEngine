
#pragma once

#include "Root/Render/Render.h"
#include "Root/TaskExecutor/TaskExecutor.h"

namespace Oak
{
	class CLASS_DECLSPEC Mesh
	{
	public:

		class CLASS_DECLSPEC Instance : public Object
		{
			Mesh* res = nullptr;
			TaskExecutor::SingleTaskPool* taskPool;

		public:

			Math::Matrix transform;
			eastl::vector<Math::Matrix> transforms;
			Color color;
			int visible = 1;

			void Init(Mesh* mesh, TaskExecutor::SingleTaskPool* taskPool);

			Math::Vector3 GetBBMin();
			Math::Vector3 GetBBMax();

			int GetSubMeshesCount();

			Math::Vector3 GetBBMin(int sub_mesh_index);
			Math::Vector3 GetBBMax(int sub_mesh_index);

			void Render(float dt);
			void ShRender(float dt);
			void Render(RenderTechnique* prg);

			void GetLocatorTransform(const char* name, Math::Matrix& loc_transform);

			void Release();
		};

		struct MeshVertex
		{
			Math::Vector3 pos;
			Math::Vector2 uv;
			Math::Vector3 normal;
		};

		struct SubMesh
		{
			int texture = -1;
			int num_triangles;
			DataBufferRef indices;
			DataBufferRef vertices;

			Math::Vector3 bb_max = -FLT_MAX;
			Math::Vector3 bb_min = FLT_MAX;
		};

		VertexDeclRef vdecl;

		Math::Vector3 bb_max = -FLT_MAX;
		Math::Vector3 bb_min = FLT_MAX;

		eastl::vector<TextureRef> textures;
		eastl::vector<SubMesh> meshes;

		eastl::map<eastl::string, Math::Matrix> locators;

		bool Load(const char* filename);
		bool LoadFBX(const char* filename);

		void Release();
	};
}