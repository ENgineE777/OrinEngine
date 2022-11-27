
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <eastl/map.h>
#include "Support/ClassFactory.h"
#include "Support/PointerRef.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief Program

	Combination of vertex and pixel shaders into program with set up of render states.

	*/

	class CLASS_DECLSPEC RenderTechnique
	{
		friend class Render;
		friend class PointerRef<RenderTechnique>;

		#ifndef DOXYGEN_SKIP

		std::size_t hash = 0;
		int refCounter = 0;
		Shader* vshader = nullptr;
		Shader* pshader = nullptr;

		friend class DeviceDX11;
		friend class DeviceGLES;

	protected:

		static eastl::map<eastl::string, Math::Vector4> vectors;
		static eastl::map<eastl::string, Math::Matrix> matrixes;
		static eastl::map<eastl::string, Texture*> textures;

		virtual bool Init();
		virtual void ApplyStates() {};
		void Release();

		#endif

	public:

		/**
		\brief Initialize program

		\return result of initiazlization
		*/
		

		/**
		\brief Store a 4 componet vector in global pool

		\param[in] param Name of a parameter
		\param[in] v Pointer to a vector
		*/
		static void SetVector(const char* param, Math::Vector4* v);

		/**
		\brief Store a matrix in global pool

		\param[in] param Name of a parameter
		\param[in] mat Pointer to a matrix
		*/
		static void SetMatrix(const char* param, Math::Matrix* mat);

		/**
		\brief Store texture in global pool

		\param[in] param Name of a parameter
		\param[in] texture Pointer to a texture
		*/
		static void SetTexture(const char* param, Texture* texture);

		/**
		\brief Return name of vertex shader

		\return Name of vertex shader
		*/
		virtual const char* GetVsName() = 0;

		/**
		\brief Return name of pixel shader

		\return Name of pixel shader
		*/
		virtual const char* GetPsName() = 0;

		/**
		\brief Set 4 component vector for a parameter

		\param[in] shader_type Type of shader
		\param[in] param Name of a parameter
		\param[in] v Pointer to vectors
		\param[in] count Number of vectors needed to be set

		\return Result of an operation
		*/
		void SetVector(ShaderType shader_type, const char* param, Math::Vector4* v, int count);

		/**
		\brief Set matrices for a parameter

		\param[in] shader_type Type of shader
		\param[in] param Name of a parameter
		\param[in] mat Pointer to a matrices
		\param[in] count Number of matrices needed to be set

		\return Result of an operation
		*/
		void SetMatrix(ShaderType shader_type, const char* param, Math::Matrix* mat, int count);

		/**
		\brief Set texture for a parameter

		\param[in] shader_type Type of shader
		\param[in] param Name of a parameter
		\param[in] texture Pointer to a texture

		\return Result of an operation
		*/
		void SetTexture(ShaderType shader_type, const char* param, Texture* texture);

		/**
		\brief Reload shaders

		*/
		void ReloadShaders();
	};

	typedef PointerRef<RenderTechnique> RenderTechniqueRef;
}