
#pragma once

#include <math.h>
#include "Support/Defines.h"
#include "Vector2.h"

namespace Orin::Math
{
	/**
	\ingroup gr_code_common_math
	*/

	struct CLASS_DECLSPEC Vector3
	{
	public:

		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
		
			float v[3];
		};

		Vector3()
		{
			Set(0.0f);
		}

		Vector3(float f)
		{
			Set(f);
		}

		Vector3(float x, float y, float z)
		{
			Set(x, y, z);
		}

		Vector3(const Vector2& v)
		{
			Set(v);
		}

		void Set(float f)
		{
			x = y = z= f;
		}

		void Set(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		void Set(const Vector3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

		void Set(const Vector2& v)
		{
			x = v.x;
			y = v.y;
			z = 0.0f;
		}

		Vector3 operator - () const
		{
			Vector3 v(*this);
			v.x = -v.x;
			v.y = -v.y;
			v.z = -v.z;
			return v;
		}

		bool operator == (const Vector3& v)
		{
			return ((fabsf(x - v.x) < 0.001f) && (fabsf(y - v.y) < 0.001f) && (fabsf(z - v.z) < 0.001f));
		}

		Vector3& operator = (float f)
		{
			x = f;
			y = f;
			z = f;
			return *this;
		}

		Vector3& operator = (const Vector3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		Vector3& operator += (float f)
		{
			x += f;
			y += f;
			z += f;
			return *this;
		}

		Vector3& operator += (const Vector3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Vector3& operator -= (float f)
		{
			x -= f;
			y -= f;
			z -= f;
			return *this;
		}

		Vector3& operator -= (const Vector3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		Vector3& operator *= (float f)
		{
			x *= f;
			y *= f;
			z *= f;
			return *this;
		}

		Vector3& operator *= (const Vector3& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		Vector3& operator /= (float f)
		{
			float d = 1.0f / f;
			x = float(x * d);
			y = float(y * d);
			z = float(z * d);
			return *this;
		}

		Vector3& operator /= (const Vector3& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}

		friend Vector3 operator + (const Vector3& v, float f)
		{
			Vector3 tv(v);
			tv += f;
			return tv;
		}

		friend Vector3 operator + (float f, const Vector3& v)
		{
			Vector3 tv(f);
			tv += v;
			return tv;
		}

		friend Vector3 operator + (const Vector3& v1, const Vector3& v2)
		{
			Vector3 tv(v1);
			tv += v2;
			return tv;
		}

		friend Vector3 operator - (const Vector3& v, float f)
		{
			Vector3 tv(v);
			tv -= f;
			return tv;
		}

		friend Vector3 operator - (float f, const Vector3& v)
		{
			Vector3 tv(f);
			tv -= v;
			return tv;
		}

		friend Vector3 operator - (const Vector3& v1, const Vector3& v2)
		{
			Vector3 tv(v1);
			tv -= v2;
			return tv;
		}

		friend Vector3 operator * (const Vector3& v, float f)
		{
			Vector3 tv(v);
			tv *= f;
			return tv;
		}

		friend Vector3 operator * (float f, const Vector3& v)
		{
			Vector3 tv(v);
			tv *= f;
			return tv;
		}

		friend Vector3 operator * (const Vector3& v1, const Vector3& v2)
		{
			Vector3 tv(v1);
			tv *= v2;
			return tv;
		}

		friend Vector3 operator / (const Vector3& v, float f)
		{
			Vector3 tv(v);
			tv /= f;
			return tv;
		}

		friend Vector3 operator / (float f, const Vector3& v)
		{
			Vector3 tv;
			tv.Set(f);
			tv /= v;
			return tv;
		}

		friend Vector3 operator / (const Vector3& v1, const Vector3& v2)
		{
			Vector3 tv;
			tv.Set(v1);
			tv /= v2;
			return tv;
		}

		bool IsEqual(const Vector3& v);

		float Normalize()
		{
			float len = sqrtf(x * x + y * y + z * z);
			if (len > 0.0001f)
			{
				float k = 1.0f / len;
				x = float(x * k);
				y = float(y * k);
				z = float(z * k);
			}

			return float(len);
		}

		void Min(const Vector3& v)
		{
			if (x > v.x) x = v.x;
			if (y > v.y) y = v.y;
			if (z > v.z) z = v.z;
		}

		void Max(const Vector3& v)
		{
			if (x < v.x) x = v.x;
			if (y < v.y) y = v.y;
			if (z < v.z) z = v.z;
		}

		float Length() const
		{
			return sqrtf(x*x + y * y + z * z);
		}

		float Length(const Vector3& v) const
		{
			float dx = x - v.x;
			float dy = y - v.y;
			float dz = z - v.z;
			return sqrtf(dx * dx + dy * dy + dz * dz);
		}

		float Length2() const
		{
			return x * x + y * y + z * z;
		}

		float Length2(const Vector3& v) const
		{
			float dx = x - v.x;
			float dy = y - v.y;
			float dz = z - v.z;
			return dx * dx + dy * dy + dz * dz;
		}

		void Lerp(const Vector3& from, const Vector3& to, float k)
		{
			x = from.x + (to.x - from.x) * k;
			y = from.y + (to.y - from.y) * k;
			z = from.z + (to.z - from.z) * k;
		}

		void SLerp(const Vector3& from, const Vector3& to, float kBlend)
		{
			double cosomega = from.x * to.x + from.y * to.y + from.z * to.z;
			double k = 1.0f;

			if (cosomega < 0.0f)
			{
				cosomega = -cosomega;
				k = -1.0f;
			}

			if (cosomega > 1.0)
			{
				cosomega = 1.0;
			}

			double k0, k1;

			if (1.0 - cosomega > 0.005)
			{

				double omega = acos(cosomega);
				double sinomega = 1.0 / sin(omega);
				k0 = sin(omega*(1.0 - kBlend))*sinomega;
				k1 = sin(omega*kBlend)*sinomega;
			}
			else
			{
				k0 = 1.0 - kBlend;
				k1 = kBlend;
			}

			k0 *= k;

			x = float(from.x * k0 + to.x * k1);
			y = float(from.y * k0 + to.y * k1);
			z = float(from.z * k0 + to.z * k1);
		}

		float Dot(const Vector3& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3 Cross(const Vector3& v) const
		{
			Vector3 v_res;
			v_res.x = y * v.z - z * v.y;
			v_res.y = z * v.x - x * v.z;
			v_res.z = x * v.y - y * v.x;
			return v_res;
		}
	};
}