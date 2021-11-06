export module World;

import <vector>;
export import Sphere;
export import Ray;
export import RayMath;
export import RayCamera;
export import LambertianMaterial;
export import MetalMaterial;

export class World {
public:
	World() = default;

	void addObject(const Sphere& obj)
	{
		_vec.push_back(obj);
	}

	bool hit(const Ray& r, float fMin, float fMax, HitRec& rec) const
	{
		HitRec hitRecTemp = { 0 };
		bool isHit = false;
		float fMaxTmp = fMax;

		for (const auto& obj : _vec) {
			if (obj.hit(r, fMin, fMaxTmp, hitRecTemp)) {
				isHit = true;
				fMaxTmp = hitRecTemp.t;
				rec = hitRecTemp;
			}
		}

		return isHit;
	}

private:
	std::vector<Sphere> _vec;
};