#ifndef MILK_CVECTOR_H_
#define MILK_CVECTOR_H_

#include "milk/math/math.h"
#include "milk/math/cvector2.h"
#include "milk/math/cvector3.h"
#include "milk/math/cvector4.h"

namespace milk
{
	namespace math
	{
		template<class VectorType>
		inline VectorType vectorMax(const VectorType& v1, const VectorType& v2)
		{
			return VectorType(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
		}

		template<class VectorType>
		inline VectorType vectorMin(const VectorType& v1, const VectorType& v2)
		{
			return VectorType(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
		}

		/// [min..max)
		inline CVector2f v2rand(CVector2f min, CVector2f max)
		{
			return CVector2f(frand(min.x, max.x), frand(min.y, max.y));
		}

		/// [min..max)
		inline CVector3f v3rand(CVector3f min, CVector3f max)
		{
			return CVector3f(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z));
		}

		/// [min..max)
		inline CVector4f v4rand(CVector4f min, CVector4f max)
		{
			return CVector4f(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z), frand(min.w, max.w));
		}
	}
}

#endif
