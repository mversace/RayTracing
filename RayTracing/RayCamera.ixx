export module RayCamera;
import Ray;
import RayMath;

export class Camera{
public:
	Camera() = default;

	Ray getRay(float u, float v)
	{
		return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
	}

private:
	RayMath::Vec3 lowerLeftCorner{ -2.0f, -1.5f, 1.0f };
	RayMath::Vec3 horizontal{ 4.0f, 0.0f, 0.0f };
	RayMath::Vec3 vertical{ 0.0f, 3.0f, 0.0f };
	RayMath::Vec3 origin{ 0.0f, 0.0f, 0.0f };
};