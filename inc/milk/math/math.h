#ifndef MILK_MATH_H_
#define MILK_MATH_H_

#include <cmath>
#include <complex>
#include <map>
#include <cstdlib>	//abs(int)

namespace milk
{
	/// namespace containing useful mathematical functions, such as templated sin, pow etc
	namespace math
	{

#if defined(WIN32) && defined(_MSC_VER)

		#define	_MATH_OVERLOAD_FLOAT(name)\
			template<class T> inline T name(T x)\
			{ return std::name(x); }\
			template<> inline float name(float x)\
			{ return std::name##f(x); }\
			template<> inline long double name(long double x)\
			{ return std::name##l(x); }

		#define _MATH_OVERLOAD_2FLOAT(name)\
			template<class T> inline T name(T x, T y)\
			{ return std::name(x, y); }\
			template<> inline float name(float x, float y)\
			{ return std::name##f(x, y); }\
			template<> inline long double name(long double x, long double y)\
			{ return std::name##l(x, y); }

		template<class T>
		inline T abs(T x)
		{ return std::abs(x); }
		template<>
		inline long abs(long x)
		{ return std::labs(x); }
		template<>
		inline float abs(float x)
		{ return std::fabsf(x); }
		template<>
		inline double abs(double x)
		{ return std::fabs(x); }
		template<>
		inline long double abs(long double x)
		{ return std::fabsl(x); }

#else

		#define	_MATH_OVERLOAD_FLOAT(name)\
			template<class T> inline T name(T x)\
			{ return std::name(x); }

		#define _MATH_OVERLOAD_2FLOAT(name)\
			template<class T> inline T name(T x, T y)\
			{ return std::name(x, y); }

		template<class T>
		inline T abs(T x)
		{ return std::abs(x); }

#endif

		_MATH_OVERLOAD_FLOAT(cos)
		_MATH_OVERLOAD_FLOAT(sin)
		_MATH_OVERLOAD_FLOAT(tan)

		_MATH_OVERLOAD_FLOAT(acos)
		_MATH_OVERLOAD_FLOAT(asin)
		_MATH_OVERLOAD_FLOAT(atan)

		_MATH_OVERLOAD_FLOAT(cosh)
		_MATH_OVERLOAD_FLOAT(sinh)
		_MATH_OVERLOAD_FLOAT(tanh)

		_MATH_OVERLOAD_FLOAT(exp)
		_MATH_OVERLOAD_FLOAT(log)
		_MATH_OVERLOAD_FLOAT(log10)
		_MATH_OVERLOAD_FLOAT(sqrt)

		_MATH_OVERLOAD_2FLOAT(atan2)
		_MATH_OVERLOAD_2FLOAT(fmod)
		_MATH_OVERLOAD_2FLOAT(pow)

		#undef _MATH_OVERLOAD_FLOAT
		#undef _MATH_OVERLOAD_2FLOAT

		/// The well-known costant Pi
		const float PI_float	= 3.1415926535f;
		const double PI			=  3.1415926535897932384626433;
		const double TO_DEGREES	= 180.0/PI; //57.2957795130823208767981548l;
		const double TO_RADIANS	= PI/180.0; //0.0174532925199432957692369l;

		/// convert  to degrees
		template<class F>
		inline const F toDegrees(const F& radians)
		{ return (radians * F(TO_DEGREES)); }

		/// convert degrees to radians
		template<class F>
		inline F toRadians(const F& degrees)
		{ return (degrees * F(TO_RADIANS)); }

		/// square the argument using x*x
		template<class T>
		inline T sqr(const T& x)
		{ return x*x; }

		/// round a number to the neares integer
		template<class F>
		inline int roundOff(const F& x)
		{ return ( x<0 ? static_cast<int>(x-F(0.5)) : static_cast<int>(x+F(0.5)) ); }

		/// Wrap v around w
		template<class C>
		inline C wrap(C v, const C& w)
		{
			while(v < 0) v += w;
			while(v >= w)v -= w;
			return v;
		}

		/// clamps a value to the given range
		template<class C>
		inline const C& clamp(const C& x, const C& a, const C& b)
		{
			return (x < a ? a : (x > b ? b : x));
		}

		/// [min..max]
		inline int irand(int min, int max) 
		{
			return min + rand()%(max-min+1);
		}

		/// Returns a random number between [0..1). That is 0 inclusive and 1 exclusive.
		inline float frand()
		{
			return float(rand())/float(RAND_MAX-1);
		}

		/// [min..max)
		inline float frand(float min, float max)
		{
			return (max-min)*frand() + min;
		}

		/// step
		/**
		(from glsl)
		Returns 0.0 if x <= edge, otherwise it returns 1.0
		*/
		template<class C>
		inline C step(const C& edge, const C& x)
		{
			return (x <= edge) ? C(0) : C(1);
		}

		/// smoothstep
		/**
		(from glsl)
		Returns 0.0 if x <= edge0 and 1.0 if x >= edge1
		and performs smooth Hermite interpolation
		between 0 and 1 when edge0 < x < edge1. This is
		useful in cases where you would want a threshold
		function with a smooth transition. This is
		equivalent to:

		t = clamp((x-edge0)/(edge1-edge0), 0, 1);
		return t*t*(3-2*t);
		*/
		template<class C>
		inline C smoothstep(const C& edge0, const C& edge1, const C& x)
		{
			C t = clamp((x-edge0)/(edge1-edge0), C(0), C(1));
			return t*t*(3-2*t);
		}

		/// standard linear interpolation, t = [0,1]
		/**
		note that t should be clamped to [0,1]
		*/
		template<class F, class X>
		X lerp(const F& t, const X& x0, const X& x1)
		{
			return static_cast<X>(t*(x1-x0) + x0);
		}

		template<class F, class X>
		X map_lerp(const F& t, const std::map<F, X>& values)
		{
			typedef typename map<F, X>::const_iterator IT;
			if(!values.size())
				return X();
			IT n = values.lower_bound(t);
			if(n == values.begin())
				return n->second;
			else if(n == values.end())
				return (--n)->second;
			else
			{
				IT p = n;
				--p;
				return lerp<F,X>((t - p->first)/(n->first - p->first), p->second, n->second);
			}
		}

		/// Check if an integer is a power of two. x = 2^n, n=integer
		template<class I>
		bool isPowerOfTwo(I n)
		{
			return (n&(n-1))==0;
		}

		/// Return the closest power-of-two number
		template<class I>
		I closestPowerOfTwo(I n, int snap=0)
		{
			static const double ln2 = math::log(2.0);
			double exp = math::log((double)n)/ln2;
			exp = (snap < 0) ? floor(exp) :
			      (snap > 0) ? ceil(exp) :
				  math::roundOff(exp);
			return (I)math::pow(2.0, exp);
		}
	}
}

#endif
