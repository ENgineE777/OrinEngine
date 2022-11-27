
#pragma once

#include "Support/Support.h"
#include "Texture.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC ShaderType
	{
		Vertex = 0 /*!< Vertex shader */,
		Pixel = 1 /*!< Pixel shader */
	};

	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief Shader

	Class that holds ready for use shader and allows set up constants.

	*/

	class CLASS_DECLSPEC Shader
	{
		friend class RenderTechnique;
		friend class DeviceDX11;

	public:

		Shader(ShaderType tp) { shaderType = tp; };

		/**
		\brief Set 4 component vector for a parameter

		\param[in] param Name of a parameter
		\param[in] v Pointer to vectors
		\param[in] count Number of vectors needed to be set

		\return Result of an operation
		*/
		virtual bool SetVector(const char* param, Math::Vector4* v, int count) = 0;

		/**
		\brief Set matrices for a parameter

		\param[in] param Name of a parameter
		\param[in] m Pointer to a matrices
		\param[in] count Number of matrices needed to be set

		\return Result of an operation
		*/
		virtual bool SetMatrix(const char* param, Math::Matrix* m, int count) = 0;

		/**
		\brief Set texture for a parameter

		\param[in] param Name of a parameter
		\param[in] tex Pointer to a texture

		\return Result of an operation
		*/
		virtual bool SetTexture(const char* param, Texture* tex) = 0;

	protected:

		#ifndef DOXYGEN_SKIP

		virtual void UpdateConstants() = 0;
		virtual void Apply() = 0;
		ShaderType shaderType;
		virtual void Release() = 0;

		#endif
	};
}