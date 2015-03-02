#ifndef MILK_CRECT_H_
#define MILK_CRECT_H_

#include <algorithm>
#include "milk/math/cvector.h"

namespace milk
{
	/// Simple class to describe a rectangle
	template<class F>
	class CRect
	{
	public:
		F x1, y1;
		F x2, y2;

		CRect() :
			x1(0), y1(0), x2(0), y2(0)
		{ }

		CRect(F _x1, F _y1, F _x2, F _y2) :
			x1(_x1), y1(_y1), x2(_x2), y2(_y2)
		{ makeStraight(); }

		CRect(const CVector2<F>& p1, const CVector2<F>& p2) :
			x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y)
		{ makeStraight(); }

		CRect(const CRect& r) :
			x1(r.x1), y1(r.y1), x2(r.x2), y2(r.y2)
		{ makeStraight(); }

		template<class F2>
		explicit CRect(const CRect<F2>& r) :
			x1(static_cast<F>(r.x1)), y1(static_cast<F>(r.y1)),
			x2(static_cast<F>(r.x2)), y2(static_cast<F>(r.y2))
		{ makeStraight(); }

		CRect& operator=(const CRect& rhs)
		{
			x1 = rhs.x1;
			x2 = rhs.x2;
			y1 = rhs.y1;
			y2 = rhs.y2;
			return *this;
		}

		friend inline bool operator==(const CRect& r1, const CRect& r2)
		{
			return (r1.x1==r2.x1 && r1.x2==r2.x2 &&
					r1.y1==r2.y1 && r1.y2==r2.y2);
		}

		friend inline bool operator!=(const CRect& r1, const CRect& r2)
		{
			return !(r1==r2);
		}

		bool intersects(const CRect& rhs)
		{
			if (rhs.x2<x1 || rhs.x1>x2 || rhs.y2<y1 || rhs.y1>y2)
				return false;
			else
				return true;
		}

		void makeStraight()
		{
			if (x2<x1) std::swap(x1,x2);
			if (y2<y1) std::swap(y1,y2);
		}

		F getX1() const { return x1; }
		F getX2() const { return x2; }
		F getY1() const { return y1; }
		F getY2() const { return y2; }

		F getW() const { return x2-x1; }
		F getH() const { return y2-y1; }
		
		F getCenterX() const { return (x1+x2)/2; }
		F getCenterY() const { return (y1+y2)/2; }
		
		CVector2<F> getCenter() const { return CVector2<F>(getCenterX(), getCenterY()); }

		F getArea() const { return getW()*getH(); }

		bool contains(const CVector2<F>& v)
		{ return (v.x>=x1 && v.x<x2 && v.y>=y1 && v.y<y2); }
		
		CRect& operator += (const CVector2<F>& v)
		{
			x1 += v.x;
			x2 += v.x;
			y1 += v.y;
			y2 += v.y;
			return *this;
		}
		
		CRect& operator -= (const CVector2<F>& v)
		{
			x1 -= v.x;
			x2 -= v.x;
			y1 -= v.y;
			y2 -= v.y;
			return *this;
		}
    
		friend CRect operator + (CRect r, const CVector2<F>& v)
		{ return r+=v; }
    
		friend CRect operator + (const CVector2<F>& v, CRect r)
		{ return r+=v; }

		friend CRect operator - (CRect r, const CVector2<F>& v)
		{ return r-=v; }
	};
}

#endif
