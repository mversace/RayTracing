#pragma once

#include "Material.h"

class MetalMaterial : public Material
{
public:
	MetalMaterial(const RayMath::Vec3& a, float f) : albedo(a), fuzz(f < 1 ? f : 1)
	{

	}

	virtual bool scatter( const Ray& rIn, const HitRec& rec, RayMath::Vec3& attenuation, Ray& scattered ) const override
	{
		auto t = rIn.direction();
		t.normalize();
		auto reflected = RayMath::reflect(t, rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * RayMath::randomInUnitSphere());
		attenuation = albedo;
		return (RayMath::dot(scattered.direction(), rec.normal) > 0);
	}

public:
	RayMath::Vec3 albedo;
	float fuzz;
};