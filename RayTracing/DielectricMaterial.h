#pragma once

#include "Material.h"

class DielectricMaterial : public Material
{
public:
	DielectricMaterial(float t) : ir(t)
	{

	}

	bool scatter(const Ray& rIn, const HitRec& rec, RayMath::Vec3& attenuation, Ray& scattered) const override
	{
		attenuation = RayMath::Vec3(1.0f, 1.0f, 1.0f);
		auto refractionRatio = rec.frontFace ? (1.0f / ir) : ir;
		auto dir = rIn.direction();
		dir.normalize();

		auto cosTheta = fmin(dot(-dir, rec.normal), 1.0f);
		auto sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		bool canRefract = refractionRatio * sinTheta > 1.0f;

		RayMath::Vec3 direction;
		if (canRefract || reflectance(cosTheta, refractionRatio) > RayMath::randF()) {
			direction = RayMath::reflect(dir, rec.normal);
		} else {
			direction = RayMath::refract(dir, rec.normal, refractionRatio);
		}

		scattered = Ray(rec.p, direction);
		return true;
	}

public:
	float ir;

private:
	static double reflectance(float cosine, float ref_idx)
	{
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};