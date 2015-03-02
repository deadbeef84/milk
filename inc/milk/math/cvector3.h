#ifndef MILK_CVECTOR3_H_
#define MILK_CVECTOR3_H_

#include "milk/math/math.h"

namespace milk
{
#	include "milk/pack8enable.h"
	/// 3D-vector
	template<class F=float>
	class CVector3
	{
	private:
		typedef CVector2<F> _CVector2;

	public:
		F x, y, z;

		// constants
		static const CVector3 XAXIS;
		static const CVector3 YAXIS;
		static const CVector3 ZAXIS;
		static const CVector3 ZERO;

		F& operator[](int i)
		{ return (&x)[i]; }

		const F& operator[](int i) const
		{ return (&x)[i]; }

		//////////////////////////////////////////////////////////////////

		CVector3()
			: x(0), y(0), z(0)
		{ }

		CVector3(F X, F Y, F Z)
			: x(X), y(Y), z(Z)
		{ }

		template<class F2>
		explicit CVector3(const CVector3<F2>& v)
			: x((F)v.x), y((F)v.y), z((F)v.z)
		{ }
	
		explicit CVector3(const F *v)
			: x(v[0]), y(v[1]), z(v[2])
		{ }

		CVector3& operator=(const F *v)
		{ x=v[0]; y=v[1]; z=v[2]; return *this; }

		const F* ptr() const
		{ return &x; }

		F* ptr()
		{ return &x; }

		void set(F X, F Y, F Z)
		{ x = X; y = Y; z = Z; }

		//////////////////////////////////////////////////////////////////

		_CVector2& vec2part()
		{ return *reinterpret_cast<_CVector2*>(&x); }

		const _CVector2& vec2part() const
		{ return *reinterpret_cast<const _CVector2*>(&x); }

		//////////////////////////////////////////////////////////////////

		CVector3 operator-() const
		{ return CVector3(-x, -y, -z); }

		const CVector3& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		CVector3& operator*=(F s)
		{ x*=s; y*=s; z*=s; return *this; }

		CVector3& operator/=(F s)
		{ x/=s; y/=s; z/=s; return *this; }

		friend inline const CVector3 operator*(const CVector3& v, const F s)
		{ return CVector3(v.x*s, v.y*s, v.z*s); }

		friend inline const CVector3 operator*(const F s, const CVector3& v)
		{ return CVector3(v.x*s, v.y*s, v.z*s); }

		friend inline const CVector3 operator/(const CVector3& v, const F s)
		{ return CVector3(v.x/s, v.y/s, v.z/s); }

		//////////////////////////////////////////////////////////////////

		CVector3& operator+=(const CVector3& v)
		{ x+=v.x; y+=v.y; z+=v.z; return *this; }

		CVector3& operator-=(const CVector3& v)
		{ x-=v.x; y-=v.y; z-=v.z; return *this; }

		friend inline const CVector3 operator+(const CVector3& v1, const CVector3& v2)
		{ return CVector3(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z); }

		friend inline const CVector3 operator-(const CVector3& v1, const CVector3& v2)
		{ return CVector3(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z); }

		//////////////////////////////////////////////////////////////////

		friend inline const F dot(const CVector3& v1, const CVector3& v2)
		{ return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); }

		friend inline const CVector3 cross(const CVector3& v1, const CVector3& v2)
		{ return CVector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x); }

		//////////////////////////////////////////////////////////////////
		
		F abs2() const
		{ return (dot(*this, *this)); }
		
		F abs() const
		{ return math::sqrt(abs2()); }

		F length() const
		{ return abs(); }

		friend inline F abs2(const CVector3& v)
		{ return v.abs2(); }

		friend inline F abs(CVector3 v)
		{ return v.abs(); }

		friend inline F length(CVector3 v)
		{ return v.length(); }
		
		CVector3& normalize()
		{ return ((*this) *= 1/abs()); }

		friend inline CVector3 normalize(const CVector3& v)
		{ return v * (1 / v.abs()); }

		CVector3& fast_normalize()
		{ return ((*this) *= math::invSqrt(dot(*this,*this))); }

		friend inline CVector3 fast_normalize(const CVector3& v)
		{ return v * math::invSqrt(dot(v,v)); }

		//////////////////////////////////////////////////////////////////
		
		friend inline bool orthogonal(const CVector3& v1, const CVector3& v2)
		{ return (dot(v1,v2)==0); }
		
		friend inline bool perpendicular(const CVector3& v1, const CVector3& v2)
		{ return orthogonal(v1,v2); }

		friend inline bool operator == (const CVector3& v1, const CVector3& v2)
		{ return (v1.x==v2.x && v1.y==v2.y && v1.z==v2.z); }

		friend inline bool operator != (const CVector3& v1, const CVector3& v2)
		{ return !(v1==v2); }
	} MILK_PACK_STRUCT;

	template<class F>
	const CVector3<F> CVector3<F>::ZERO(0,0,0);
	template<class F>
	const CVector3<F> CVector3<F>::XAXIS(1,0,0);
	template<class F>
	const CVector3<F> CVector3<F>::YAXIS(0,1,0);
	template<class F>
	const CVector3<F> CVector3<F>::ZAXIS(0,0,1);

	typedef CVector3<float> CVector3f;
	typedef CVector3<double> CVector3d;
#	include "milk/pack8disable.h"
}

#endif
