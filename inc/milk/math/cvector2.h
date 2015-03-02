#ifndef MILK_CVECTOR2_H_
#define MILK_CVECTOR2_H_

#include "milk/math/math.h"

namespace milk
{
#	include "milk/pack8enable.h"
	/// 2D-vector
	template<class F=float>
	class CVector2
	{
	public:
		F x, y;

		F& operator[](int i)
		{ return (&x)[i]; }

		const F& operator[](int i) const
		{ return (&x)[i]; }

		F& operator()(int i)
		{ return (*this)[i]; }

		F operator()(int i) const
		{ return (*this)[i]; }

		//////////////////////////////////////////////////////////////////

		CVector2(F X=F(), F Y=F()) : 
		x(X), y(Y) { }

		template<class F2>
		explicit CVector2(const CVector2<F2>& v) :
		x((F)v.x), y((F)v.y) { }
	
		explicit CVector2(const F *v) :
		x(v[0]), y(v[1]) { }

		CVector2& operator=(const F *v)
		{ x=v[0]; y=v[1]; return *this; }

		/*
		operator const F*() const
		{ return &x; }

		operator F*()
		{ return &x; }
		*/

		const F* ptr() const
		{ return &x; }

		F* ptr()
		{ return &x; }

		void set(F X, F Y)
		{ x = X; y = Y; }

		//////////////////////////////////////////////////////////////////

		CVector2 operator-() const
		{ return CVector2(-x, -y); }

		const CVector2& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		CVector2& operator*=(F s)
		{ x*=s; y*=s; return *this; }

		CVector2& operator/=(F s)
		{ x=F(x/s); y=F(y/s); return *this; }

		friend inline const CVector2 operator*(const CVector2& v, const F s)
		{ return CVector2(v.x*s, v.y*s); }

		friend inline const CVector2 operator*(const F s, const CVector2& v)
		{ return CVector2(v.x*s, v.y*s); }

		friend inline const CVector2 operator/(const CVector2& v, const F s)
		{ return CVector2(v.x/s, v.y/s); }

		//////////////////////////////////////////////////////////////////

		CVector2& operator+=(const CVector2& v)
		{ x+=v.x; y+=v.y; return *this; }

		CVector2& operator-=(const CVector2& v)
		{ x-=v.x; y-=v.y; return *this; }

		friend inline const CVector2 operator+(const CVector2& v1, const CVector2& v2)
		{ return CVector2(v1.x+v2.x, v1.y+v2.y); }

		friend inline const CVector2 operator-(const CVector2& v1, const CVector2& v2)
		{ return CVector2(v1.x-v2.x, v1.y-v2.y); }

		//////////////////////////////////////////////////////////////////

		friend inline const F dot(const CVector2& v1, const CVector2& v2)
		{ return (v1.x*v2.x + v1.y*v2.y); }

		//////////////////////////////////////////////////////////////////
		
		F abs2() const
		{ return (dot(*this, *this)); }
		
		F abs() const
		{ return math::sqrt<F>(this->abs2()); }
		
		F length() const
		{ return abs(); }

		friend inline F abs2(const CVector2& v)
		{ return v.abs2(); }

		friend inline F abs(const CVector2& v)
		{ return v.abs(); }

		friend inline F length(const CVector2& v)
		{ return v.length(); }

		CVector2& normalize()
		{ return ((*this) *= 1/abs()); }

		friend inline CVector2 normalize(const CVector2& v)
		{ return v * (1 / v.abs()); }

		CVector2& fast_normalize()
		{ return ((*this) *= math::invSqrt(dot(*this,*this))); }

		friend inline CVector2 fast_normalize(const CVector2& v)
		{ return v * math::invSqrt(dot(v,v)); }

		F angle() const
		{ return math::atan2(x,y); }

		friend inline F angle(const CVector2& v)
		{ return v.angle(); }

		//////////////////////////////////////////////////////////////////
		
		friend inline const bool orthogonal(const CVector2& v1, const CVector2& v2)
		{ return (dot(v1,v2)==0); }
		
		friend inline const bool perpendicular(const CVector2& v1, const CVector2& v2)
		{ return orthogonal(v1,v2); }

		friend inline bool operator == (const CVector2& v1, const CVector2& v2)
		{ return (v1.x==v2.x && v1.y==v2.y); }

		friend inline bool operator != (const CVector2& v1, const CVector2& v2)
		{ return !(v1==v2); }
	} MILK_PACK_STRUCT;
	typedef CVector2<float> CVector2f;
	typedef CVector2<double> CVector2d;
#	include "milk/pack8disable.h"
}

#endif
