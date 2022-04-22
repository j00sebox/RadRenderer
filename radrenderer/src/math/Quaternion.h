#pragma once

#include "Matrix.h"

namespace math
{
	class Quaternion
	{
	public:
		// initial rotation of 0 degrees around x-axis
		Quaternion() 
			: q(0.f), i(1.f), j(0.f), k(0.f) {}

		Quaternion(float _q, float _i, float _j, float _k)
			: q(_q), i(_i), j(_j), k(_k) {}

		Quaternion(float angle, Vec3<float> vec)
		{
			set_angle(angle);

			set_axis(vec);
		}

		void set_axis(const Vec3<float>& axis)
		{
			i = axis.x;
			j = axis.y;
			k = axis.z;
		}

		void set_angle(float angle)
		{
			q = cosf(angle / 2.f) + sinf(angle / 2.f);
		}

		void invert()
		{
			i = -i;
			j = -j;
			k = -k;
		}
		
		Mat4<float> convert_to_mat()
		{
			return Mat4<float>(
				2.f * (pow(q, 2) + pow(i, 2)) - 1.f, 2.f * (i * j - q * k), 2.f * (i * k + q * j), 0.f,
				2.f * (i * j + q * k), 2.f * (pow(q, 2) + pow(j, 2)) - 1.f, 2.f * (j * k - q * i), 0.f,
				2.f * (i * k - q * j), 2.f * (j * k + q * i), 2.f * (pow(q, 2) + pow(k, 2)) - 1.f, 0.f,
				0.f, 0.f, 0.f, 1.f
				);
		}

		Quaternion operator * (const Quaternion& other)
		{
			float new_q = q * other.q - i * other.i - j * other.j - k * other.k;
			float new_i = q * other.i + i * other.q + j * other.k - k * other.j;
			float new_j = q * other.j - i * other.k + j * other.q + k * other.i;
			float new_k = q * other.k + i * other.j - j * other.i + k * other.q;

			return Quaternion(new_q, new_i, new_j, new_k);
		}

		float q, i, j, k;
	};
}