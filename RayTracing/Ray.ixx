export module Ray;
import RayMath;

export class Ray {
public:
	Ray() = delete;
	Ray(const RayMath::Vec3& a, const RayMath::Vec3& b)
	{
		A = a;
		B = b;
		B.normalize();
	}

	auto origin() const { return A; }
	auto direction() const { return B; }
	auto pointAtParameter(float t) const { return A + t * B; }

private:
	RayMath::Vec3 A;
	RayMath::Vec3 B;
};