#pragma once

#include <math.h>
#include <memory>
#include "Material.h"

class Sphere
{
public:
	Sphere(const RayMath::Vec3& center, float radius, std::shared_ptr<Material> m)
	{
		_center = center;
		_radius = radius;
		_matPtr = m;
	}

	// 判断射线是否与球相交
	// 射线上的点 f(t) = rayOrigin + t * rayDirection;
	// 判定一个点是否在球体表面 dot(p-c, p-c) = r * r;
	// dot(f(t)-c, f(t)-c) = r * r;
	// t*t*dot(rayDirection, rayDirection) + 2*t*dot(rayDirection, rayOrigin-center)
	// + dot(rayOrigin-center, rayOrigin-center) - r*r = 0;
	// ax^2 + 2bx + c = 0
	// 有没有根 b^2 - 4ac >= 0
	// a = dot(rayDirection, rayDirection)
	// b = dot(rayDirection, rayOrigin - center)
	// c = dot(rayOrigin-center, rayOrigin - center) - r * r
	bool hit(const Ray& r, float fMin, float fMax, HitRec& rec) const
	{
		RayMath::Vec3 oc = r.origin() - _center;
		RayMath::Vec3 direction = r.direction();

		float a = RayMath::dot(direction, direction);
		float b = 2.0f * RayMath::dot(direction, oc);
		float c = RayMath::dot(oc, oc) - _radius * _radius;
		float dis = b * b - 4.0f * a * c;
		if (dis > 0) {
			float tmp = (-b - static_cast<float>(sqrt(dis))) / (2.0f * a);
			if (fMin < tmp && tmp < fMax) {
				rec.t = tmp;
				rec.p = r.pointAtParameter(tmp);
				auto normal = rec.p - _center;
				normal.normalize();
				rec.setFaceNormal(r, normal);
				rec.matPtr = _matPtr;
				return true;
			}

			tmp = (-b + static_cast<float>(sqrt(dis))) / (2.0f * a);
			if (fMin < tmp && tmp < fMax) {
				rec.t = tmp;
				rec.p = r.pointAtParameter(tmp);
				auto normal = rec.p - _center;
				normal.normalize();
				rec.setFaceNormal(r, normal);
				rec.matPtr = _matPtr;
				return true;
			}
		}

		return false;
	}

private:
	RayMath::Vec3 _center;
	float _radius;
	std::shared_ptr<Material> _matPtr;
};