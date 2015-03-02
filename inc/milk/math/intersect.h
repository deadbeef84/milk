#ifndef MILK_INTERSECT_H_
#define MILK_INTERSECT_H_

#include <limits>
#include "milk/geometry.h"

namespace milk
{
	/// intersect sphere with ray
	inline bool isIntersecting(const CSphere& sphere, const CRay& ray)
	{
		CVector3f temp = ray.o-sphere.getCenter();
		float p = dot(ray.d, temp);
		float q = dot(temp, temp) - math::sqr(sphere.getRadius());
		return ( math::sqr(p) - q >= 0 );
	}

	/// intersect sphere with ray
	/** returns where on the ray the two intersect, the shorterst first
	use CRay::calculate(F) to get the precise point of intersection */
	inline std::pair<float, float> intersect(const CSphere& sphere, const CRay& ray)
	{
		CVector3f temp = ray.o-sphere.getCenter();
		float p = dot(ray.d, temp);
		float q = dot(temp, temp) - math::sqr(sphere.getRadius());
		float temp2 = math::sqrt(math::sqr(p)-q);
		return std::pair<float, float>(-p - temp2, -p + temp2);
	}

	/// intersect sphere with sphere
	inline bool isIntersecting(const CSphere& s1, const CSphere& s2)
	{
		CVector3f temp = s1.getCenter() - s2.getCenter();
		return (dot(temp,temp) <= math::sqr(s1.getRadius() + s2.getRadius()));
	}

	/// intersect triangle with ray
	/** returns if there was intersection, and where on ray it was
	use CRay::calculate(F) to get the precise point of intersection */
	inline std::pair<bool, float> intersect(const CTriangle& tri, const CRay& ray)
	{
		CVector3f e1 = tri.p1 - tri.p0;
		CVector3f e2 = tri.p2 - tri.p0;
		CVector3f p = cross(ray.d, e2);
		float a = dot(e1, p);
		if (a >= -2*std::numeric_limits<float>::epsilon() && a <= 2*std::numeric_limits<float>::epsilon())
			return std::pair<bool, float>(false, 0);
		float f = 1/a;
		CVector3f s = ray.o-tri.p1;
		float u = f*dot(s,p);
		//if (u<0 || u>1) return std::pair<bool, t>(false, 0);
		CVector3f q = cross(s,e1);
		float v = f*dot(ray.d,q);
		if (v<0 || u+v>1)
			return std::pair<bool, float>(false, 0);
		else
			return std::pair<bool, float>(true, f*dot(e2,q));
	}

}

#endif
