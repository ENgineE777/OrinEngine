
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
		\brief Calulated local matrix
		*/
		Math::Matrix local;

		/**
		\brief Matrix of a parent
		*/
		Math::Matrix* parent = nullptr;

		/**
		\brief Final matrix
		*/
		Math::Matrix global;

		/**
		\brief Calculate final matrix
		*/
		virtual void BuildMatrices()
		{
			global = parent ? (local * (*parent)) : local;
		}

		/**
		\brief Is transform for 2D object
		*/
		virtual bool Is2D() = 0;

		/**
		\brief Load data of transform

		\param[in] read JSON helper class for read JSON
		\param[in] name of block
		*/
		virtual void Load(JsonReader& reader, const char* name) {};

		/**
		\brief Save data of transform

		\param[in] writer JSON helper class for writing JSON
		\param[in] name of block
		*/
		virtual void Save(JsonWriter& writer, const char* name) {};
	};

	/**
	\brief Transform3D

	This class used for storing a transform of 3D objects.

	*/

	struct Transform3D : Transform
	{
		/**
		\brief Calculate final matrix
		*/
		void BuildMatrices() override
		{
			Transform::BuildMatrices();
		}

		/**
		\brief Is transform for 2D object
		*/
		virtual bool Is2D()
		{
			return false;
		}

		/**
		\brief Load data of transform

		\param[in] read JSON helper class for read JSON
		\param[in] name of block
		*/
		virtual void Load(JsonReader& reader, const char* name)
		{
			reader.Read(name, local);
		}

		/**
		\brief Save data of transform

		\param[in] writer JSON helper class for writing JSON
		\param[in] name of block
		*/
		virtual void Save(JsonWriter& writer, const char* name)
		{
			writer.Write(name, local);
		}
	};

	/**
	\brief Transform2D

	This class used for storing a transform of 2D objects.

	*/

	struct Transform2D : Transform
	{
		/**
		\brief Position of a object
		*/
		Math::Vector2 pos = 0.0f;

		/**
		\brief Axis direction
		*/
		Math::Vector2 axis = 1.0f;

		/**
		\brief Z depth
		*/
		float depth = 0.5f;

		/**
		\brief Width and height
		*/
		Math::Vector2 size = 10.0f;

		/**
		\brief Rotation around anchor
		*/
		float rotation = 0.0f;

		/**
		\brief Offset of anchorn in absolute units, i.e. 1 equals to width/height
		*/
		Math::Vector2 offset = 0.5f;

		/**
		\brief Calculate final matrix
		*/
		void BuildMatrices() override
		{
			local.Identity();
			local.RotateZ(rotation);
			local.Pos() = Math::Vector3(axis.x > 0.0f ? pos.x : -pos.x - size.x, axis.y > 0.0f ? pos.y : -pos.y - size.y, 0.0f);

			Transform::BuildMatrices();
		}

		/**
		\brief Is transform for 2D object
		*/
		virtual bool Is2D()
		{
			return true;
		}
	};
}