
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
		__declspec(property(get = GetPosition, put = SetPosition)) Math::Vector3 position;

		/**
			\brief Local rotation position
		*/
		__declspec(property(get = GetRotation, put = SetRotation)) Math::Vector3 rotation;

		/**
			\brief Local rotation position
		*/
		__declspec(property(get = GetScale, put = SetScale)) Math::Vector3 scale;

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
		__declspec(property(get = GetLocal, put = SetLocal)) Math::Matrix local;

		/**
		\brief Final matrix
		*/
		__declspec(property(get = GetGlobal, put = SetGlobal)) Math::Matrix global;

		/**
		\brief Matrix of a parent
		*/
		Transform* parent = nullptr;

		/**
			\brief Matrix of a parent
		*/
		eastl::vector<Transform*> childs;

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
		\brief Load data of transform

		\param[in] read JSON helper class for read JSON
		\param[in] name of block
		*/
		void Load(JsonReader& reader, const char* name)
		{
			if (reader.EnterBlock(name))
			{
				reader.Read("position", positionValue);
				reader.Read("rotation", rotationValue);
				reader.Read("scale", scaleValue);

				reader.Read("size", size);
				reader.Read("offset", offset);

				SetDirty();

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
			writer.Write("position", positionValue);
			writer.Write("rotation", rotationValue);
			writer.Write("scale", scaleValue);
			writer.Write("size", size);
			writer.Write("offset", offset);
			writer.FinishBlock();
		};

		Math::Vector3 GetPosition()
		{
			return positionValue;
		}

		void SetPosition(Math::Vector3 set)
		{
			positionValue = set;

			SetDirty();
		}

		Math::Vector3 GetRotation()
		{
			return rotationValue;
		}

		void SetRotation(Math::Vector3 set)
		{
			rotationValue = set;

			SetDirty();
		}

		Math::Vector3 GetScale()
		{
			return scaleValue;
		}

		void SetScale(Math::Vector3 set)
		{
			scaleValue = set;

			SetDirty();
		}

		Math::Matrix GetLocal()
		{
			if (dirty)
			{
				BuildMatrices();
			}

			return localValue;
		}


		/**
			\brief Set local transform
		*/
		void SetLocal(Math::Matrix set)
		{
			positionValue = set.Pos();
			scaleValue = set.GetScale();
			rotationValue = set.GetRotation() / Math::Radian;

			SetDirty();
		}

		Math::Matrix GetGlobal()
		{
			if (dirty)
			{
				BuildMatrices();
			}

			return globalValue;
		}

		/**
			\brief Set local transform
		*/
		void SetGlobal(Math::Matrix set)
		{
			globalValue = set;

			if (parent)
			{
				Math::Matrix inverse = parent->globalValue;
				inverse.Inverse();

				localValue = globalValue * inverse;
			}
			else
			{
				localValue = set;
			}

			SetLocal(localValue);
			dirty = false;
		}

		private:

		bool dirty = true;

		Math::Vector3 positionValue;
		Math::Vector3 rotationValue;
		Math::Vector3 scaleValue = 1.0f;
		Math::Matrix localValue;
		Math::Matrix globalValue;

		void SetDirty()
		{
			dirty = true;

			for (auto child : childs)
			{
				child->SetDirty();
			}
		}

		void BuildMatrices()
		{
			localValue.Identity();
			localValue.Rotate(rotation * Math::Radian);
			localValue.Scale(scale);
			localValue.Pos() = position;

			if (parent)
			{
				if (parent->dirty)
				{
					parent->BuildMatrices();
				}

				globalValue = localValue * parent->globalValue;
			}
			else
			{
				globalValue = localValue;
			}

			dirty = false;
		}
	};
}