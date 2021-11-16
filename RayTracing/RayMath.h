#pragma once

#include <math.h>
#include <random>

namespace RayMath
{
	class Vec3
	{
	public:
		Vec3() = default;
		Vec3(float e1, float e2, float e3)
		{
			e[0] = e1;
			e[1] = e2;
			e[2] = e3;
		}

		inline float x() const { return e[0]; }
		inline float y() const { return e[1]; }
		inline float z() const { return e[2]; }

		inline float squaredLength() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
		inline float length() const { return static_cast<float>(sqrt(squaredLength())); }
		void normalize()
		{
			auto len = length();
			e[0] /= len;
			e[1] /= len;
			e[2] /= len;
		}

		bool nearZero() const
		{
			// Return true if the vector is close to zero in all dimensions.
			const auto s = 1e-8;
			return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
		}

		float e[3];
	};

	inline Vec3 operator*(float t, const Vec3& v) { return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]); }
	inline Vec3 operator+(const Vec3& v1, const Vec3& v2) { return Vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]); }
	inline Vec3 operator+=(Vec3& v1, const Vec3& v2) { v1 = v1 + v2;  return v1; }
	inline Vec3 operator-(const Vec3& v1, const Vec3& v2) { return Vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]); }
	inline Vec3 operator/=(Vec3& v1, float t) { v1.e[0] /= t; v1.e[1] /= t; v1.e[2] /= t; return v1; }

	inline Vec3 operator-(const Vec3& v1) { return Vec3(-v1.e[0], -v1.e[1], -v1.e[2]); }
	inline Vec3 operator*(const Vec3& u, const Vec3& v)
	{
		return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}
	inline Vec3 operator/(Vec3& v1, float t)
	{
		return Vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
	}

	inline float dot(const Vec3& v1, const Vec3& v2)
	{
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
	}

	inline Vec3 cross(const Vec3& u, const Vec3& v)
	{
		return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
					u.e[2] * v.e[0] - u.e[0] * v.e[2],
					u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	Vec3 reflect(const Vec3& v, const Vec3& n)
	{
		return v - 2 * dot(v, n) * n;
	}

	Vec3 refract(const Vec3& uv, const Vec3& n, float e)
	{
		auto t = fmin(dot(-uv, n), 1.0f);
		auto rOutPerp = e * (uv + t * n);
		auto rOutParallel = -sqrt(fabs(1.0f - rOutPerp.squaredLength())) * n;
		return rOutPerp + rOutParallel;
	}

	float randF()
	{
		return static_cast<float>(rand()) / RAND_MAX;
	}

	float randF(float min, float max)
	{
		// Returns a random real in [min,max).
		return min + (max - min) * randF();
	}

	Vec3 random()
	{
		return Vec3(randF(), randF(), randF());
	}

	Vec3 random(float min, float max)
	{
		return Vec3(randF(min, max), randF(min, max), randF(min, max));
	}

	Vec3 randomInUnitSphere()
	{
		// 为了防止找到的点进入了内部
		RayMath::Vec3 p;
		do {
			p = 2.0f * Vec3(randF(), randF(), randF()) - Vec3(1.0f, 1.0f, 1.0f);
		} while (p.squaredLength() >= 1.0f);

		return p;
	}

	Vec3 randomUnitVector()
	{
		auto p = randomInUnitSphere();
		p.normalize();
		return p;
	}

	Vec3 randomInHemisphere(const Vec3& normal)
	{
		Vec3 p = randomInUnitSphere();
		if (dot(p, normal) > 0.0) {
			return p;
		} else {
			return -p;
		}
	}

	Vec3 randomInUnitDisk()
	{
		while (true) {
			auto p = Vec3(randF(-1.0f, 1.0f), randF(-1.0f, 1.0f), 0.0f);
			if (p.squaredLength() >= 1) continue;
			return p;
		}
	}
}