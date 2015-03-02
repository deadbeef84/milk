#ifndef MILK_CVECTOR4_H_
#define MILK_CVECTOR4_H_

#include "milk/math/math.h"
#include "milk/math/cvector2.h"
#include "milk/math/cvector3.h"

namespace milk
{
#	include "milk/pack8enable.h"
	/// 4D-vector
	template<class F=float>
	class CVector4
	{
	private:
		typedef CVector2<F> _CVector2;
		typedef CVector3<F> _CVector3;

	public:
		F x, y, z, w;

		F& operator[](int i)
		{ return (&x)[i]; }

		const F& operator[](int i) const
		{ return (&x)[i]; }

		//////////////////////////////////////////////////////////////////

		CVector4()
			: x(0), y(0), z(0), w(0)
		{ }

		CVector4(F X, F Y, F Z, F W)
			: x(X), y(Y), z(Z), w(W)
		{ }

		template<class F2>
		explicit CVector4(const CVector4<F2>& v)
			: x((F)v.x), y((F)v.y), z((F)v.z), w((F)v.w)
		{ }

		CVector4(const CVector3<F>& v, F W)
			: x(v.x), y(v.y), z(v.z), w(W)
		{ }

		explicit CVector4(const F *v)
			: x(v[0]), y(v[1]), z(v[2]), w(v[3])
		{ }

		CVector4& operator=(const F *v)
		{ x=v[0]; y=v[1]; z=v[2]; w=v[3]; return *this; }

		const F* ptr() const
		{ return &x; }

		F* ptr()
		{ return &x; }

		void set(F X, F Y, F Z, F W)
		{ x = X; y = Y; z = Z; w = W; }

		//////////////////////////////////////////////////////////////////

		_CVector2& vec2part()
		{ return *reinterpret_cast<_CVector2*>(&x); }

		const _CVector2& vec2part() const
		{ return *reinterpret_cast<const _CVector2*>(&x); }

		_CVector3& vec3part()
		{ return *reinterpret_cast<_CVector3*>(&x); }

		const _CVector3& vec3part() const
		{ return *reinterpret_cast<const _CVector3*>(&x); }

		//////////////////////////////////////////////////////////////////

		CVector4 operator-() const
		{ return CVector4(-x, -y, -z, -w); }

		const CVector4& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		CVector4& operator*=(F s)
		{ x*=s; y*=s; z*=s; w*=s; return *this; }

		CVector4& operator/=(F s)
		{ x/=s; y/=s; z/=s; w/=s; return *this; }

		friend inline const CVector4 operator*(const CVector4& v, const F s)
		{ return CVector4(v.x*s, v.y*s, v.z*s, v.w*s); }

		friend inline const CVector4 operator*(const F s, const CVector4& v)
		{ return CVector4(v.x*s, v.y*s, v.z*s, v.w*s); }

		friend inline const CVector4 operator/(const CVector4& v, const F s)
		{ return CVector4(v.x/s, v.y/s, v.z/s, v.w*s/s); }

		//////////////////////////////////////////////////////////////////

		CVector4& operator+=(const CVector4& v)
		{ x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }

		CVector4& operator-=(const CVector4& v)
		{ x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }

		friend inline const CVector4 operator+(const CVector4& v1, const CVector4& v2)
		{ return CVector4(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w); }

		friend inline const CVector4 operator-(const CVector4& v1, const CVector4& v2)
		{ return CVector4(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w); }

		//////////////////////////////////////////////////////////////////

		friend inline const F dot(const CVector4& v1, const CVector4& v2)
		{ return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w); }

		//////////////////////////////////////////////////////////////////

		F abs2() const
		{ return (dot(*this, *this)); }

		F abs() const
		{ return math::sqrt(this->abs2()); }

		F length() const
		{ return abs(); }

		friend inline F abs2(const CVector4& v)
		{ return abs2(v); }

		friend inline F abs(CVector4 v)
		{ return v.abs(); }

		friend inline F length(CVector4 v)
		{ return v.length(); }

		CVector4& normalize()
		{ return ((*this) *= 1/abs()); }

		friend inline CVector4 normalize(const CVector4& v)
		{ return v * (1 / v.abs()); }

		CVector4& fast_normalize()
		{ return ((*this) *= math::invSqrt(dot(*this,*this))); }

		friend inline CVector4 fast_normalize(const CVector4& v)
		{ return v * math::invSqrt(dot(v,v)); }

		//////////////////////////////////////////////////////////////////

		friend inline bool orthogonal(const CVector4& v1, const CVector4& v2)
		{ return (dot(v1,v2)==0); }

		friend inline bool perpendicular(const CVector4& v1, const CVector4& v2)
		{ return orthogonal(v1,v2); }

		friend inline bool operator == (const CVector4& v1, const CVector4& v2)
		{ return (v1.x==v2.x && v1.y==v2.y && v1.z==v2.z); }

		friend inline bool operator != (const CVector4& v1, const CVector4& v2)
		{ return !(v1==v2); }
	} MILK_PACK_STRUCT;
	typedef CVector4<float> CVector4f;
	typedef CVector4<double> CVector4d;
#	include "milk/pack8disable.h"
}

#endif
