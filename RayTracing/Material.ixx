export module Material;

import <memory>;
import Ray;
import RayMath;

class Material;
export struct HitRec {
	float t;
	RayMath::Vec3 p;
	RayMath::Vec3 normal;

	std::shared_ptr<Material> matPtr;
	bool frontFace;

	inline void setFaceNormal(const Ray& r, const RayMath::Vec3& outWardNormal) {
		frontFace = RayMath::dot(r.direction(), outWardNormal) < 0;
		normal = frontFace ? outWardNormal : -outWardNormal;
	}
};

export class Material {
public:
	virtual bool scatter(
		const Ray& rIn, const HitRec& rec, RayMath::Vec3& attenuation, Ray& scattered
	) const = 0;
};

