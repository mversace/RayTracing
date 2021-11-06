export module LambertianMaterial;

export import Material;
import Ray;
import RayMath;

export class LambertianMaterial : public Material {
public:
	LambertianMaterial(const RayMath::Vec3& a) : albedo(a)
	{

	}

	virtual bool scatter(const Ray& rIn, const HitRec& rec, RayMath::Vec3& attenuation, Ray& scattered) const override
	{
		auto scatter_direction = rec.normal + RayMath::randomUnitVector();

		if (scatter_direction.nearZero())
			scatter_direction = rec.normal;

		scattered = Ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

public:
	RayMath::Vec3 albedo;
};