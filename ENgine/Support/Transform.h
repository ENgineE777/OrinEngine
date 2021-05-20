
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
			\brief Size of object
		*/
		Math::Vector3 size = 1.0f;

		/**
			\brief Offset in absolute units, i.e. 0.5 equals to center of an object
		*/
		Math::Vector3 offset = 0.5f;

		/**
		\brief Calculate final matrix
		*/
		virtual void BuildMatrices()
		{
			global = parent ? (local * (*parent)) : local;
		}

		/**
		\brief Load data of transform

		\param[in] read JSON helper class for read JSON
		\param[in] name of block
		*/
		void Load(JsonReader& reader, const char* name)
		{
			if (reader.EnterBlock(name))
			{
				reader.Read("local", local);
				reader.Read("size", size);
				reader.Read("offset", offset);

				reader.LeaveBlock();
			}
		};

		/**
		\brief Save data of transform

		\param[in] writer JSON helper class for writing JSON
		\param[in] name of block
		*/
		void Save(JsonWriter& writer, const char* name)
		{
			writer.StartBlock(name);
			writer.Write("local", local);
			writer.Write("size", size);
			writer.Write("offset", offset);
			writer.FinishBlock();
		};
	};
}