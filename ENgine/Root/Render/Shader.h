
#pragma once

#include "Support/Support.h"
#include "Texture.h"

namespace Oak
{
	class Shader
	{
	public:
		enum Type
		{
			Vertex = 0,
			Pixel  = 1
		};

		Shader(Type tp) { sdr_type = tp; };

		virtual void UpdateConstants() = 0;
		virtual void Apply() = 0;

		virtual bool SetVector(const char* param, Math::Vector4* v, int count) = 0;
		virtual bool SetMatrix(const char* param, Math::Matrix* m, int count) = 0;

		virtual bool SetTexture(const char* param, Texture* tex) = 0;

		virtual void Release() = 0;

	protected:
		Type sdr_type;
	};
}