#pragma once

#include "Ray.h"

class Camera
{
public:
	Camera()
	{

	}

	void init(float fov, float aspectRadio)
	{
		auto theta = fov * 3.141592653f / 180.0f;
		auto h = tan(theta / 2.0f);
		auto viewHeight = 2.0f * h;
		auto viewWidth = aspectRadio * viewHeight;

		auto focalLen = 1.0f;
		origin = RayMath::Vec3(0.0f, 0.0f, 0.0f);
		horizontal = RayMath::Vec3(viewWidth, 0.0f, 0.0f);
		vertical = RayMath::Vec3(0.0f, viewHeight, 0.0f);
		lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f + RayMath::Vec3(0.0f, 0.0f, focalLen);
	}

	Ray getRay(float u, float v)
	{
		return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
	}

private:
	RayMath::Vec3 origin;
	RayMath::Vec3 lowerLeftCorner;
	RayMath::Vec3 horizontal;
	RayMath::Vec3 vertical;
};