#pragma once

#include "Ray.h"

class Camera
{
public:
	Camera()
	{

	}

	void init(RayMath::Vec3 lookFrom, RayMath::Vec3 lookAt, RayMath::Vec3 up,
			  float fov, float aspectRadio, float aperture, float focusDist)
	{
		auto theta = fov * 3.141592653f / 180.0f;
		auto h = tan(theta / 2.0f);
		auto viewHeight = 2.0f * h;
		auto viewWidth = aspectRadio * viewHeight;

		w = lookFrom - lookAt;
		w.normalize();
		u = RayMath::cross(w, up);
		u.normalize();
		v = RayMath::cross(u, w);
		v.normalize();

		origin = lookFrom;
		horizontal = focusDist * viewWidth * u;
		vertical = focusDist * viewHeight * v;
		lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - focusDist * w;

		lensRadius = aperture / 2.0f;
	}

	Ray getRay(float s, float t)
	{
		auto rd = lensRadius * RayMath::randomInUnitDisk();
		auto offset = rd.x() * u + rd.y() * v;

		return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset );
	}

private:
	RayMath::Vec3 origin;
	RayMath::Vec3 lowerLeftCorner;
	RayMath::Vec3 horizontal;
	RayMath::Vec3 vertical;
	RayMath::Vec3 w, u, v;
	float lensRadius;
};