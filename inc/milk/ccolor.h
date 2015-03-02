#ifndef MILK_CCOLOR_H_
#define MILK_CCOLOR_H_

#include <ostream>
#include <istream>

namespace milk
{
#	include "milk/pack8enable.h"
	/// A very simple color-class
	template<class F>
	class CColor4
	{
	public:
		static const CColor4 BLACK;
		static const CColor4 WHITE;
		static const CColor4 RED;
		static const CColor4 GREEN;
		static const CColor4 BLUE;
		static const CColor4 CYAN;
		static const CColor4 MAGENTA;
		static const CColor4 YELLOW;

		CColor4()
			: r(F(0)), g(F(0)), b(F(0)), a(F(1))
		{ }

		CColor4(F _r, F _g, F _b, F _a=F(1))
			: r(_r), g(_g), b(_b), a(_a)
		{ }

		template<class F2>
		explicit CColor4(const CColor4<F2>& c)
			: r((F)c.r), g((F)c.g), b((F)c.b), a((F)c.a)
		{ }

		// these operators are needed so we can easily blend colors
		friend inline const CColor4<F> operator*(const CColor4<F>& c1, F s)
		{ return CColor4f(c1.r*s, c1.g*s, c1.b*s, c1.a*s); }

		friend inline const CColor4<F> operator*(F s, const CColor4<F>& c1)
		{ return CColor4f(c1.r*s, c1.g*s, c1.b*s, c1.a*s); }

		friend inline const CColor4<F> operator+(const CColor4<F>& c1, const CColor4<F>& c2)
		{ return CColor4f(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b, c1.a+c2.a); }

		friend inline const CColor4<F> operator-(const CColor4<F>& c1, const CColor4<F>& c2)
		{ return CColor4f(c1.r-c2.r, c1.g-c2.g, c1.b-c2.b, c1.a-c2.a); }

		friend inline bool operator == (const CColor4<F>& c1, const CColor4<F>& c2)
		{ return (c1.r==c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a); }

		F r, g, b, a;

		operator const F*() const
		{ return &r; }
		operator F*()
		{ return &r; }

	} MILK_PACK_STRUCT;

	template<class F>
	const CColor4<F> CColor4<F>::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
	template<class F>
	const CColor4<F> CColor4<F>::WHITE(1.0f, 1.0f, 1.0f, 1.0f);

	template<class F>
	const CColor4<F> CColor4<F>::RED(1.0f, 0.0f, 0.0f, 1.0f);
	template<class F>
	const CColor4<F> CColor4<F>::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
	template<class F>
	const CColor4<F> CColor4<F>::BLUE(0.0f, 0.0f, 1.0f, 1.0f);

	template<class F>
	const CColor4<F> CColor4<F>::CYAN(0.0f, 1.0f, 1.0f, 1.0f);
	template<class F>
	const CColor4<F> CColor4<F>::MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);
	template<class F>
	const CColor4<F> CColor4<F>::YELLOW(1.0f, 1.0f, 0.0f, 1.0f);

	typedef CColor4<float> CColor4f;
	typedef CColor4<double> CColor4d;
	typedef CColor4<unsigned char> CColor4ub;

	/// A very simple color-class
	template<class F>
	class CColor3
	{
	public:
		static const CColor3 BLACK;
		static const CColor3 WHITE;
		static const CColor3 RED;
		static const CColor3 GREEN;
		static const CColor3 BLUE;
		static const CColor3 CYAN;
		static const CColor3 MAGENTA;
		static const CColor3 YELLOW;

		CColor3(F _r, F _g, F _b)
			: r(_r), g(_g), b(_b)
		{ }

		CColor3()
			: r(F(0)), g(F(0)), b(F(0))
		{ }

		CColor3& set(F _r, F _g, F _b)
		{ r = _r, g = _g, b = _b; return *this; }

		template<class F2>
		explicit CColor3(const CColor3<F2>& c)
			: r((F)c.r), g((F)c.g), b((F)c.b)
		{ }

		// these operators are needed so we can easily blend colors
		friend inline const CColor3<F> operator*(const CColor3<F>& c1, F s)
		{ return CColor4f(c1.r*s, c1.g*s, c1.b*s); }

		friend inline const CColor3<F> operator*(F s, const CColor3<F>& c1)
		{ return CColor4f(c1.r*s, c1.g*s, c1.b*s); }

		friend inline const CColor3<F> operator+(const CColor3<F>& c1, const CColor3<F>& c2)
		{ return CColor4f(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b); }

		friend inline const CColor3<F> operator-(const CColor3<F>& c1, const CColor3<F>& c2)
		{ return CColor4f(c1.r-c2.r, c1.g-c2.g, c1.b-c2.b); }

		F r, g, b;

		operator const F*() const
		{ return &r; }
		operator F*()
		{ return &r; }

	} MILK_PACK_STRUCT;

	template<class F>
	const CColor3<F> CColor3<F>::BLACK(0.0f, 0.0f, 0.0f);
	template<class F>
	const CColor3<F> CColor3<F>::WHITE(1.0f, 1.0f, 1.0f);

	template<class F>
	const CColor3<F> CColor3<F>::RED(1.0f, 0.0f, 0.0f);
	template<class F>
	const CColor3<F> CColor3<F>::GREEN(0.0f, 1.0f, 0.0f);
	template<class F>
	const CColor3<F> CColor3<F>::BLUE(0.0f, 0.0f, 1.0f);

	template<class F>
	const CColor3<F> CColor3<F>::CYAN(0.0f, 1.0f, 1.0f);
	template<class F>
	const CColor3<F> CColor3<F>::MAGENTA(1.0f, 0.0f, 1.0f);
	template<class F>
	const CColor3<F> CColor3<F>::YELLOW(1.0f, 1.0f, 0.0f);

	typedef CColor3<float> CColor3f;
	typedef CColor3<double> CColor3d;
	typedef CColor3<unsigned char> CColor3ub;

#	include "milk/pack8disable.h"
}

#endif
