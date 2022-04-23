#pragma once

namespace math {

	template <typename T>
	class Vec2
	{
	public:
		Vec2() : x(0), y(0) {}

		Vec2(T _x, T _y) : x(_x), y(_y) {}

		Vec2(std::pair<T, T> pair)
		{
			x = pair.first;
			y = pair.second;
		}

		float cross(Vec2& line)
		{
			return (x * line.y) - (y * line.x);
		}

		float dot(Vec2& vec)
		{
			return x * vec.x + y * vec.y;
		}

		void normalize()
		{
			float normalize = sqrtf(pow(x, 2) + pow(y, 2));

			x /= normalize;
			y /= normalize;
		}

		void subtract(Vec2& inputVec, Vec2& outVec)
		{
			outVec.x = x - inputVec.x;
			outVec.y = y - inputVec.y;
		}

		void scalar_mul(Vec2& outVec, float scalar)
		{
			outVec.x = scalar * x;
			outVec.y = scalar * y;
		}

		void add(Vec2& a, Vec2& outVec)
		{
			outVec.x = x + a.x;
			outVec.y = y + a.y;
		}

		T x, y;
	};

	/**********************************************
	class Vec3
	@brief Class used to represent a vector in 3D
			space.
	************************************************/
	class Vec3
	{
	public:
		Vec3() : x(0), y(0), z(0) {}

		Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

		inline void cross(Vec3& line, Vec3& normal)
		{
			normal.x = y * line.z - z * line.y;
			normal.y = z * line.x - x * line.z;
			normal.z = x * line.y - y * line.x;
		}

		inline float dot(Vec3& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		inline void normalize()
		{
			float normalize = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

			x /= normalize;
			y /= normalize;
			z /= normalize;
		}

		inline Vec3 operator + (const Vec3& v)
		{
			Vec3 r;

			r.x = x + v.x;
			r.y = y + v.y;
			r.z = z + v.z;

			return r;
		}

		inline Vec3 operator - (const Vec3& v)
		{
			Vec3 r;

			r.x = x - v.x;
			r.y = y - v.y;
			r.z = z - v.z;

			return r;
		}
        
        inline Vec3 operator * (float a) const 
		{
			Vec3 r;

			r.x = x * a;
			r.y = y * a;
			r.z = z * a;

			return r;
		}

		inline void scalar_mul(Vec3& outVec, float scalar) const
		{
			outVec.x = scalar * x;
			outVec.y = scalar * y;
			outVec.z = scalar * z;
		}

		inline void add(const Vec3& a, Vec3& outVec) const
		{
			outVec.x = x + a.x;
			outVec.y = y + a.y;
			outVec.z = z + a.z;
		}

		float x, y, z;
	};


	template<typename T>
	class Vec4
	{
	public:
		Vec4() : x(0), y(0), z(0), w(0) {}

		Vec4(const Vec3& vec, int w)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			this->w = w;
		}

		inline void normalize()
		{
			float normalize = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

			x /= normalize;
			y /= normalize;
			z /= normalize;
			w /= normalize;
		}

		inline void normalize_xyz()
		{
			float normalize = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

			x /= normalize;
			y /= normalize;
			z /= normalize;
		}

		inline Vec4<T> operator + (const Vec4<T>& v)
		{
			Vec4<T> r;

			r.x = x + v.x;
			r.y = y + v.y;
			r.z = z + v.z;
			r.w = w + v.w;

			return r;
		}

		inline Vec4<T> operator - (const Vec4<T>& v)
		{
			Vec4<T> r;

			r.x = x - v.x;
			r.y = y - v.y;
			r.z = z - v.z;
			r.w = w - v.w;

			return r;
		}

		inline Vec4<T> operator * (float a) const
		{
			Vec4<T> r;

			r.x = x * a;
			r.y = y * a;
			r.z = z * a;
			r.w = w * a;

			return r;
		}

		T x, y, z, w;
	};

}
