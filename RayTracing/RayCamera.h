#pragma once

#include "Ray.h"

class Camera
{
public:
	Camera()
	{

	}

	void init(RayMath::Vec3 lookFrom, RayMath::Vec3 lookAt, RayMath::Vec3 up, float fov, float aspectRadio)
	{
		auto theta = fov * 3.141592653f / 180.0f;
		auto h = tan(theta / 2.0f);
		auto viewHeight = 2.0f * h;
		auto viewWidth = aspectRadio * viewHeight;

		auto w = lookFrom - lookAt;
		w.normalize();
		auto u = RayMath::cross(w, up);
		u.normalize();
		auto v = RayMath::cross(u, w);
		v.normalize();

		origin = lookFrom;
		horizontal = viewWidth * u;
		vertical = viewHeight * v;
		lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - w;
	}

	Ray getRay(float u, float v)
	{
		return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin );
	}

private:
	RayMath::Vec3 origin;
	RayMath::Vec3 lowerLeftCorner;
	RayMath::Vec3 horizontal;
	RayMath::Vec3 vertical;
};