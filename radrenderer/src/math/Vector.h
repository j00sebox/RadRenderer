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
	template <typename T>
	class Vec3
	{
	public:
		Vec3() : x(0), y(0), z(0) {}

		Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

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
			float normalize = sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));

			x /= normalize;
			y /= normalize;
			z /= normalize;
		}

		inline void subtract(const Vec3& inputVec, Vec3& outVec) const
		{
			outVec.x = x - inputVec.x;
			outVec.y = y - inputVec.y;
			outVec.z = z - inputVec.z;
		}

		inline Vec3<T> operator + (const Vec3<float>& v)
		{
			Vec3<T> r;

			r.x = x + v.x;
			r.y = y + v.y;
			r.z = z + v.z;

			return r;
		}

		inline Vec3<T> operator - (const Vec3<float>& v)
		{
			Vec3<T> r;

			r.x = x - v.x;
			r.y = y - v.y;
			r.z = z - v.z;

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

		T x, y, z;
	};

}
