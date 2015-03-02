#ifndef MILK_GEOMETRY_H_
#define MILK_GEOMETRY_H_

#include "milk/math/math.h"
#include "milk/glhelper.h"
#include "milk/math/cvector.h"
#include <limits>

namespace milk
{
	/// class describing plane
	template<class F>
	class CPlane
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;

		template <class>
		friend class CFrustum;

	public:
		// a*x + b*y + c*z + d = 0
		_CVector4 equation;

		CPlane()
			: equation(1,0,0,0)
		{ }

		/// Create plane from equation
		CPlane(F A, F B, F C, F D)
			: equation(A, B, C, D)
		{ }

		/// Create plane from normal and distance
		explicit CPlane(const _CVector4& plane)
			: equation(plane)
		{ }

		/// Create plane from normal and distance
		CPlane(const _CVector3& normal, F d)
			: equation(normal.x, normal.y, normal.z, d)
		{ }

		/// Create plane from point and normal
		CPlane(const _CVector3& point, const _CVector3& normal)
			: equation(normal.x, normal.y, normal.z, -dot(normal, point))
		{ }

		/// Normalize the plane
		void normalize()
		{
			F magnitude = 1/math::sqrt(equation.x*equation.x + equation.y*equation.y + equation.z*equation.z);
			equation *= magnitude;
		}

		void set(const _CVector4& plane)
		{ equation = plane; }

		void set(const _CVector3& normal, F d)
		{ equation.set(normal.x, normal.y, normal.z, d); }

		void set(const _CVector3& point, const _CVector3& normal)
		{ equation.set(normal.x, normal.y, normal.z, -dot(normal, point)); }

		//////////////////////////////////////////////////////////////////////////

		void flip() // TODO: verify this really flips the plane...
		{ equation = -equation; }

		friend inline CPlane<F> flip(const CPlane<F>& p)
		{ return CPlane<F>(-p.equation); }

		void setNormal(const _CVector3& normal)
		{ equation.x = normal.x; equation.y = normal.y; equation.z = normal.z; }

		void setDistance(F d)
		{ equation.w = d; }

		const _CVector3& getNormal() const
		{ return equation.vec3part(); }

		_CVector3 getPointOnPlane() const
		{ return equation.vec3part()*equation.w; }

		F getDistance() const
		{ return equation.w; }

		F distance(const _CVector3& p)
		{ return (p.x*equation.x + p.y*equation.y + p.z*equation.z + equation.w); }

		bool isOn(const _CVector3& p)
		{ return (p.x*equation.x + p.y*equation.y + p.z*equation.z + equation.w) == 0; }

		bool isAbove(const _CVector3& p)
		{ return (p.x*equation.x + p.y*equation.y + p.z*equation.z + equation.w) > 0; }

		bool isBelow(const _CVector3& p)
		{ return (p.x*equation.x + p.y*equation.y + p.z*equation.z + equation.w) < 0; }
	};

	template<class F>
	class CBox
	{
	private:
		typedef CVector3<F> _CVector3;

	public:
		CBox(const _CVector3& min, const _CVector3& max)
			: m_min(math::vectorMin(min, max)), m_max(math::vectorMax(min, max))
		{ }

		F getVolume() const
		{
			_CVector3 delta = m_max - m_min;
			return delta.x*delta.y*delta.z;
		}

		void set(const _CVector3& min, const _CVector3& max)
		{
			m_min = math::vectorMin(min, max);
			m_max = math::vectorMax(min, max);
		}

		void setMin(const _CVector3& min)
		{
			m_min = min;
			m_max = math::vectorMax(min, m_max);
		}

		void setMax(const _CVector3& max)
		{
			m_max = max;
			m_min = math::vectorMin(m_min, max);
		}

		const _CVector3& getMin() const
		{ return m_min; }

		const _CVector3& getMax() const
		{ return m_max; }

	private:
		_CVector3 m_min;
		_CVector3 m_max;
	};

	/// class describing sphere
	template<class F>
	class CSphere
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;

	public:
		_CVector3 center;
		F radius;

		CSphere(const _CVector3& _center, F _radius)
			: center(_center), radius(_radius)
		{ }
	};

	/// class describing ray
	template<class F>
	class CRay
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;

	public:
		_CVector3 o, d;

		CRay() {}
		CRay(const _CVector3& origin, const _CVector3& direction) :
			o(origin), d(direction) { }

		_CVector3 calculate(F t) const
		{ return o+t*d; }
	};

	/// class describing triangle
	template<class F>
	class CTriangle
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;

	public:
		_CVector3 p0,p1,p2;

		_CVector3 barycentric(F u, F v)
		{ return ((1-u-v)*p0 + u*p1 + v*p2); }
	};

	/// class describing a view frustum
	template<class F>
	class CFrustum
	{
	public:
		CPlane<F> sides[6];

		enum SIDES
		{
			RIGHT	= 0,
			LEFT	= 1,
			BOTTOM	= 2,
			TOP		= 3,
			BACK	= 4,
			FRONT	= 5
		};


		bool isInside(const CVector3<F>& p)
		{
			for(int i=0 ; i<6 ; ++i)
				if(sides[i].distance(p) <= 0)
					return false;
			return true;
		}

		bool isInside(const CSphere<F>& sphere)
		{
			for(int i=0 ; i<6; ++i)
				if(sides[i].distance(sphere.center) <= -sphere.radius)
					return false;
			return true;
		}

		bool isInside(const CBox<F>& box)
		{
			const CVector3<F>& p1 = box.getMin();
			const CVector3<F>& p2 = box.getMax();

			for(int i = 0; i < 6; i++)
			{
				if(sides[i].isAbove(p1))
					continue;
				if(sides[i].isAbove(CVector3<F>(p2.x, p1.y, p1.z)))
					continue;
				if(sides[i].isAbove(CVector3<F>(p1.x, p2.y, p1.z)))
					continue;
				if(sides[i].isAbove(CVector3<F>(p2.x, p2.y, p1.z)))
					continue;
				if(sides[i].isAbove(CVector3<F>(p1.x, p1.y, p2.z)))
					continue;
				if(sides[i].isAbove(CVector3<F>(p2.x, p1.y, p2.z)))
					continue;
				if(sides[i].isAbove(CVector3<F>(p1.x, p2.y, p2.z)))
					continue;
				if(sides[i].isAbove(p2))
					continue;

				// If we get here, it isn't in the frustum
				return false;
			}

			return true;
		}

		void calculate(const CMatrix4<F>& view, const CMatrix4<F>& proj)
		{
			CMatrix4<F> clip = proj*view;

			// This will extract the RIGHT side of the frustum
			sides[RIGHT].equation.x = clip[ 3] - clip[ 0];
			sides[RIGHT].equation.y = clip[ 7] - clip[ 4];
			sides[RIGHT].equation.z = clip[11] - clip[ 8];
			sides[RIGHT].equation.w = clip[15] - clip[12];
			sides[RIGHT].normalize();

			// This will extract the LEFT side of the frustum
			sides[LEFT].equation.x = clip[ 3] + clip[ 0];
			sides[LEFT].equation.y = clip[ 7] + clip[ 4];
			sides[LEFT].equation.z = clip[11] + clip[ 8];
			sides[LEFT].equation.w = clip[15] + clip[12];
			sides[LEFT].normalize();

			// This will extract the BOTTOM side of the frustum
			sides[BOTTOM].equation.x = clip[ 3] + clip[ 1];
			sides[BOTTOM].equation.y = clip[ 7] + clip[ 5];
			sides[BOTTOM].equation.z = clip[11] + clip[ 9];
			sides[BOTTOM].equation.w = clip[15] + clip[13];
			sides[BOTTOM].normalize();

			// This will extract the TOP side of the frustum
			sides[TOP].equation.x = clip[ 3] - clip[ 1];
			sides[TOP].equation.y = clip[ 7] - clip[ 5];
			sides[TOP].equation.z = clip[11] - clip[ 9];
			sides[TOP].equation.w = clip[15] - clip[13];
			sides[TOP].normalize();

			// This will extract the BACK side of the frustum
			sides[BACK].equation.x = clip[ 3] - clip[ 2];
			sides[BACK].equation.y = clip[ 7] - clip[ 6];
			sides[BACK].equation.z = clip[11] - clip[10];
			sides[BACK].equation.w = clip[15] - clip[14];
			sides[BACK].normalize();

			// This will extract the FRONT side of the frustum
			sides[FRONT].equation.x = clip[ 3] + clip[ 2];
			sides[FRONT].equation.y = clip[ 7] + clip[ 6];
			sides[FRONT].equation.z = clip[11] + clip[10];
			sides[FRONT].equation.w = clip[15] + clip[14];
			sides[FRONT].normalize();
		}
	};
}

#endif
