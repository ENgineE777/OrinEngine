
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

	enum CLASS_DECLSPEC TransformFlag : uint32_t
	{
		MoveX = (1 << 0),
		MoveY = (1 << 1),
		MoveZ = (1 << 2),
		RotateX = (1 << 3),
		RotateY = (1 << 4),
		RotateZ = (1 << 5),
		ScaleX = (1 << 6),
		ScaleY = (1 << 7),
		ScaleZ = (1 << 8),
		SizeX = (1 << 9),
		SizeY = (1 << 10),
		SizeZ = (1 << 11),
		RectMoveX = (1 << 12),
		RectMoveY = (1 << 13),
		RectSizeX = (1 << 14),
		RectSizeY = (1 << 15),
		RectAnchorn = (1 << 16),
		MoveXYZ = MoveX | MoveY | MoveZ,
		RotateXYZ = RotateX | RotateY | RotateZ,
		ScaleXYZ = ScaleX | ScaleY | ScaleZ,
		SizeXYZ = SizeX | SizeY | SizeZ,
		RectMoveXY = RectMoveX | RectMoveY,
		RectSizeXY = RectSizeX | RectSizeY,
		RectFull = RectMoveXY | RectSizeXY | RectAnchorn,
		MoveRotateScaleFull = MoveXYZ | RotateXYZ | ScaleXYZ,
		SpriteTransformFlags = MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY | TransformFlag::RectFull
	};

	constexpr enum TransformFlag operator |(const enum TransformFlag selfValue, const enum TransformFlag inValue)
	{
		return (enum TransformFlag)(uint32_t(selfValue) | uint32_t(inValue));
	}

	constexpr enum TransformFlag operator &(const enum TransformFlag selfValue, const enum TransformFlag inValue)
	{
		return (enum TransformFlag)(uint32_t(selfValue) & uint32_t(inValue));
	}

	struct CLASS_DECLSPEC Transform
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
			\brief Units scale
		*/
		float* unitsScale = nullptr;

		/**
			\brief Units invert scale
		*/
		float* unitsInvScale = nullptr;

		/**
			\brief Transform flag
		*/
		TransformFlag transformFlag = TransformFlag::MoveRotateScaleFull;

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

		/**
		\brief Load data of transform

		\param[in] read JSON helper class for read JSON
		\param[in] name of block
		*/
		void Load(JsonReader& reader, const char* name)
		{
			if (reader.EnterBlock(name))
			{
				reader.Read("position", position);
				reader.Read("rotation", rotation);
				reader.Read("scale", scale);

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
			writer.Write("position", position);
			writer.Write("rotation", rotation);
			writer.Write("scale", scale);
			writer.Write("size", size);
			writer.Write("offset", offset);
			writer.FinishBlock();
		};
	};
}