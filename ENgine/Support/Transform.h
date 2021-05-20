
#pragma once

#include "Root/Files/JsonReader.h"
#include "Root/Files/JsonWriter.h"

/**
\ingroup gr_code_common
*/

namespace Oak
{
	/**
	\brief Transform

	This base class of transformation

	*/

	struct Transform
	{
		/**
			\brief Local position
		*/
		Math::Vector3 position;

		/**
			\brief Local rotation position
		*/
		Math::Vector3 rotation;

		/**
			\brief Local rotation position
		*/
		Math::Vector3 scale = 1.0f;

		/**
			\brief Size of object
		*/
		Math::Vector3 size = 1.0f;

		/**
			\brief Offset in absolute units, i.e. 0.5 equals to center of an object
		*/
		Math::Vector3 offset = 0.5f;

		/**
		\brief Calulated local matrix
		*/
		Math::Matrix local;

		/**
		\brief Final matrix
		*/
		Math::Matrix global;

		/**
		\brief Matrix of a parent
		*/
		Math::Matrix* parent = nullptr;

		/**
		\brief Calculate final matrix
		*/
		virtual void BuildMatrices()
		{
			local.Identity();
			local.Rotate(rotation * Math::Radian);
			local.Scale(scale);
			local.Pos() = position;

			global = parent ? (local * (*parent)) : local;
		}

		/**
			\brief Calculate final matrix
		*/
		virtual void SetData(Math::Matrix& matrix)
		{
			position = matrix.Pos();
			scale = matrix.GetScale();
			rotation = matrix.GetRotation() / Math::Radian;
		}
	};
}