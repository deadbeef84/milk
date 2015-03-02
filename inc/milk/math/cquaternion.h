#ifndef MILK_CQUATERNION_H_
#define MILK_CQUATERNION_H_

#include "milk/math/math.h"
#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"

namespace milk
{
	/// Quaternion class
	template<class F=float>
	class CQuaternion
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CMatrix3<F> _CMatrix3;
		typedef CMatrix4<F> _CMatrix4;

	public:
		F w, x, y, z;

		F& operator[](int i)
		{ return (&w)[i]; }

		F operator[](int i) const
		{ return (&w)[i]; }

		const _CVector3& v() const
		{ return *(_CVector3*)&x; }
		_CVector3& v()
		{ return *(_CVector3*)&x; }

		F s() const
		{ return w; }
		F& s()
		{ return w; }

		//////////////////////////////////////////////////////////////////

		/// Base constructor
		CQuaternion()
		: w(1), x(0), y(0), z(0)
		{ }

		/// Create from 4 components
		CQuaternion(F _w, F _x, F _y, F _z)
			: w(_w), x(_x), y(_y), z(_z)
		{ }

		/// Create from scalar and vector
		CQuaternion(const F s, const _CVector3& v)
			: w(s), x(v.x), y(v.y), z(v.z)
		{ }

		/// Create from pointer
		explicit CQuaternion(const F* qv)
			: w(qv[0]), x(qv[1]), y(qv[2]), z(qv[3])
		{  }

		/// Create from Matrix3
		explicit CQuaternion(const _CMatrix3& m)
		{ (*this) = quatFromMatrix(m); }

		/// Create from Matrix4
		explicit CQuaternion(const _CMatrix4& m)
		{ (*this) = quatFromMatrix(m); }

		/// Create from Vector
		explicit CQuaternion(const _CVector3& v)
		{ (*this) = quatFromEuler(v); }

		//////////////////////////////////////////////////////////////////

		/*
		operator F*()
		{ return &w; }

		operator const F*() const
		{ return &w; }
		*/

		//////////////////////////////////////////////////////////////////

		CQuaternion operator-() const
		{ return CQuaternion(-w, -x, -y, -z); }

		const CQuaternion& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		CQuaternion& operator*=(const F s)
		{ w *= s; x *= s; y *= s; z *= s; return *this; }

		CQuaternion& operator/=(const F s)
		{ *this *= 1/s; return *this; }

		CQuaternion& operator*=(const CQuaternion& q)
		{
			const F a = (w+x) * (q.w+q.x),
					b = (z-y) * (q.y-q.z),
					c = (w-x) * (q.y+q.z),
					d = (y+z) * (q.w-q.x),
					e = (x+z) * (q.x+q.y),
					f = (x-z) * (q.x-q.y),
					g = (w+y) * (q.w-q.z),
					h = (w-y) * (q.w+q.z);

			w = b + (-e - f + g + h)/2;
			x = a - (e + f + g + h)/2;
			y = c + (e - f + g - h)/2;
			z = d + (e - f - g + h)/2;
			return (*this);
		}

		friend CQuaternion operator*(const CQuaternion& q1, const CQuaternion& q2)
		{
			const F a = (q1.w+q1.x) * (q2.w+q2.x),
					b = (q1.z-q1.y) * (q2.y-q2.z),
					c = (q1.w-q1.x) * (q2.y+q2.z),
					d = (q1.y+q1.z) * (q2.w-q2.x),
					e = (q1.x+q1.z) * (q2.x+q2.y),
					f = (q1.x-q1.z) * (q2.x-q2.y),
					g = (q1.w+q1.y) * (q2.w-q2.z),
					h = (q1.w-q1.y) * (q2.w+q2.z);

			return CQuaternion(
				b + (-e - f + g + h)/2,
				a - (e + f + g + h)/2,
				c + (e - f + g - h)/2, 
				d + (e - f - g + h)/2);
		}

		friend CQuaternion operator*(const CQuaternion& q, F s)
		{ return CQuaternion(q.w*s, q.x*s, q.y*s, q.z*s); }

		friend CQuaternion operator*(const F s, const CQuaternion& q)
		{ return CQuaternion(q.w*s, q.x*s, q.y*s, q.z*s); }

		friend CQuaternion operator/(const CQuaternion& q, const F s)
		{ return q * (1 / s); }

		friend CQuaternion operator/(const F s, const CQuaternion& q)
		{ return q * (1 / s); }

		//////////////////////////////////////////////////////////////////

		CQuaternion& operator+=(const CQuaternion& q)
		{
			w += q.w;
			x += q.x;
			y += q.y;
			z += q.z;
			return *this;
		}

		CQuaternion& operator-=(const CQuaternion& q)
		{
			w -= q.w;
			x -= q.x;
			y -= q.y;
			z -= q.z;
			return *this;
		}

		friend CQuaternion operator+(const CQuaternion& q1, const CQuaternion& q2)
		{ return CQuaternion(q1.w+q2.w, q1.x+q2.x, q1.y+q2.y, q1.z+q2.z); }

		friend CQuaternion operator-(const CQuaternion& q1, const CQuaternion& q2)
		{ return CQuaternion(q1.w-q2.w, q1.x-q2.x, q1.y-q2.y, q1.z-q2.z); }

		//////////////////////////////////////////////////////////////////

		friend F dot(const CQuaternion& q1, const CQuaternion& q2)
		{
			return q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;
		}

		F norm() const
		{
			return w*w + x*x + y*y + z*z;
		}

		CQuaternion& normalize()
		{
			F n = 1 / abs();
			*this *= n;
			return *this;
		}

		F abs() const
		{
			return math::sqrt(norm());
		}

		CQuaternion& conj()
		{
			x = -x;
			y = -y;
			z = -z;
			return (*this);
		}

		CQuaternion& inverse()
		{
			(*this) = milk::conj(*this) / milk::norm(*this);
			return (*this);
		}

		////////////////////////////////////////////////////////////

		CQuaternion& fromAxis(const _CVector3& axis, F radians)
		{
			w = math::cos(radians/2);
			v() = axis * math::sin(radians/2);
			return (*this);
		}

		CQuaternion& fromEuler(const F _x, const F _y, const F _z)
		{
			F angle;
			F sr, sp, sy, cr, cp, cy;
			angle = _x*0.5f;
			sy = math::sin(angle);
			cy = math::cos(angle);
			angle = _y*0.5f;
			sp = math::sin(angle);
			cp = math::cos(angle);
			angle = _z*0.5f;
			sr = math::sin(angle);
			cr = math::cos(angle);

			F crcp = cr*cp;
			F srsp = sr*sp;

			w = crcp*cy+srsp*sy;
			x = sr*cp*cy-cr*sp*sy;
			y = cr*sp*cy+sr*cp*sy;
			z = crcp*sy-srsp*cy;
			return (*this);
		}

		CQuaternion& fromEuler(const _CVector3& euler)
		{
			return fromEuler(euler.x, euler.y, euler.z);
		}

		CQuaternion& fromMatrix(const _CMatrix4& m)
		{
			*this = quatFromMatrix(m);
			return (*this);
		}

		CQuaternion& fromMatrix(const _CMatrix3& m)
		{
			*this = quatFromMatrix(m);
			return (*this);
		}

		_CVector3 getEuler() const
		{
			F sqw = w*w;
			F sqx = x*x;
			F sqy = y*y;
			F sqz = z*z;
			return _CVector3(
				math::atan2(F(2) * (y*z + x*w),(-sqx - sqy + sqz + sqw)),
				math::asin(F(-2) * (x*z - y*w)),
				math::atan2(F(2) * (x*y + z*w),(sqx - sqy - sqz + sqw)));
		}

		void getAxis(_CVector3& axis, F& angle) const
		{
			angle = F(2) * math::acos(w);
			F d = math::sqrt(1 - w * w);
			if(math::abs(d) < F(0.001))
				d = 1;
			axis.x = x / d;
			axis.y = y / d;
			axis.z = z / d;
		}

		_CMatrix3 getMatrix3() const
		{
			const F x2 = x + x;
			const F y2 = y + y; 
			const F z2 = z + z;
			const F xx = x * x2;
			const F xy = x * y2;
			const F xz = x * z2;
			const F yy = y * y2;
			const F yz = y * z2;
			const F zz = z * z2;
			const F wx = w * x2;
			const F wy = w * y2;
			const F wz = w * z2;

			return _CMatrix3(
				F(1) - (yy + zz),	(xy + wz),			(xz - wy),
				(xy - wz),			F(1) - (xx + zz),	(yz + wx),
				(xz + wy),			(yz - wx),			F(1) - (xx + yy));
		}

		_CMatrix4 getMatrix4() const
		{
			const F x2 = x + x;
			const F y2 = y + y; 
			const F z2 = z + z;
			const F xx = x * x2;
			const F xy = x * y2;
			const F xz = x * z2;
			const F yy = y * y2;
			const F yz = y * z2;
			const F zz = z * z2;
			const F wx = w * x2;
			const F wy = w * y2;
			const F wz = w * z2;
			
			return _CMatrix4(
				F(1) - (yy + zz),	xy + wz,			xz - wy,			0,
				xy - wz,			F(1) - (xx + zz),	yz + wx,			0,
				xz + wy,			yz - wx,			F(1) - (xx + yy),	0,
				0,					0,					0,					1);
		}

		//////////////////////////////////////////////////////////////////

		friend CQuaternion quatFromAxis(const _CVector3& axis, F radians)
		{
			return CQuaternion(math::cos(radians/2), axis * math::sin(radians/2));
		}

		friend CQuaternion quatFromMatrix(const _CMatrix4& m)
		{
			// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
			// article "Quaternion Calculus and Fast Animation".

			CQuaternion q;
			F trace = m.m00+m.m11+m.m22;
			F root;

			if(trace > F(0))
			{
				root = math::sqrt(trace + F(1));
				q.w = F(0.5)*root;
				root = F(0.5)/root;
				q.x = (m.m12-m.m21)*root;
				q.y = (m.m20-m.m02)*root;
				q.z = (m.m01-m.m10)*root;
			}
			else
			{
				static int next[3] = {1, 2, 0};
				int i = 0;
				if(m.m11 > m.m00)
					i = 1;
				if(m.m22 > m(i,i))
					i = 2;
				int j = next[i];
				int k = next[j];

				root = math::sqrt(m(i,i)-m(j,j)-m(k,k) + F(1));
				q[1+i] = F(0.5)*root;
				root = F(0.5)/root;
				q.w = (m(j,k)-m(k,j))*root;
				q[1+j] = (m(i,j)+m(j,i))*root;
				q[1+k] = (m(i,k)+m(k,i))*root;
			}

			if(q.w < F(0))
				q = -q;

			return q;
		}

		friend CQuaternion quatFromMatrix(const _CMatrix3& m)
		{
			// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
			// article "Quaternion Calculus and Fast Animation".

			CQuaternion q;
			F trace = m.m00+m.m11+m.m22;
			F root;

			if(trace > F(0))
			{
				root = math::sqrt(trace + F(1));
				q.w = F(0.5)*root;
				root = F(0.5)/root;
				q.x = (m.m12-m.m21)*root;
				q.y = (m.m20-m.m02)*root;
				q.z = (m.m01-m.m10)*root;
			}
			else
			{
				static int next[3] = {1, 2, 0};
				int i = 0;
				if(m.m11 > m.m00)
					i = 1;
				if(m.m22 > m(i,i))
					i = 2;
				int j = next[i];
				int k = next[j];

				root = math::sqrt(m(i,i)-m(j,j)-m(k,k) + F(1));
				q[1+i] = F(0.5)*root;
				root = F(0.5)/root;
				q.w = (m(j,k)-m(k,j))*root;
				q[1+j] = (m(i,j)+m(j,i))*root;
				q[1+k] = (m(i,k)+m(k,i))*root;
			}

			if(q.w < F(0))
				q = -q;

			return q;
		}

		////////////////////////////////////////////////////////////

		friend _CVector3 operator*(const CQuaternion& q, const _CVector3& v)
		{
			return ((q * CQuaternion(0, v)) * milk::inverse(q)).v();
		}
	};

	template<class F>
	inline F abs(const CQuaternion<F>& q)
	{
		return math::sqrt(q.norm());
	}

	template<class F>
	inline CQuaternion<F> conj(const CQuaternion<F>& q)
	{
		return CQuaternion<F>(q.w, -q.x, -q.y, -q.z);
	}
	
	template<class F>
	inline F norm(const CQuaternion<F>& q)
	{
		return dot(q, q);
	}
    
	template<class F>
	CQuaternion<F> normalize(const CQuaternion<F>& q)
	{
		F n = 1 / q.abs();
		return q * n;
	}

	template<class F>
	const CQuaternion<F> quatFromEuler(const F x, const F y, const F z)
	{
		F angle;
		F sr, sp, sy, cr, cp, cy;
		angle = x*0.5f;
		sy = math::sin(angle);
		cy = math::cos(angle);
		angle = y*0.5f;
		sp = math::sin(angle);
		cp = math::cos(angle);
		angle = z*0.5f;
		sr = math::sin(angle);
		cr = math::cos(angle);

		F crcp = cr*cp;
		F srsp = sr*sp;

		return CQuaternion<F>(
			sr*cp*cy-cr*sp*sy,
			cr*sp*cy+sr*cp*sy,
			crcp*sy-srsp*cy,
			crcp*cy+srsp*sy);
	}

	template<class F>
	inline CQuaternion<F> quatFromEuler(const CVector3<F>& euler)
	{
		return quatFromEuler<F>(euler.x, euler.y, euler.z);
	}

	template<class F>
	inline CQuaternion<F> inverse(const CQuaternion<F>& q)
	{
		return (milk::conj(q) / milk::norm(q));
	}

	/// Find rotation arc
	/**
	Gets the rotation quaternion for rotating
	from v1 to v2. v1 and v2 should be normalized!
	*/
	template<class F>
	CQuaternion<F> roataionArc(const CVector3<F>& v0, const CVector3<F>& v1)
	{
		F s = 1/math::sqrt(2+2*dot(v0,v1));
		return CQuaternion<F>(1/(2*s), cross(v0,v1)*s);
	}

	/// spherical linear interpolation
	/**
	Spherical linear interpolation of quaternions
	*/
	template<class F>
	CQuaternion<F> slerp(F t, const CQuaternion<F>& q0, CQuaternion<F> q1)
	{
		F dot = q0[0]*q1[0] + q0[1]*q1[1] + q0[2]*q1[2] + q0[3]*q1[3];

		// dot = cos(theta)
		// if (dot < 0), q0 and q1 are more than 90 degrees apart,
		// so we can invert one to reduce spinning

		//remove this for squad
		if(dot < 0)
		{
			dot = -dot;
			q1 *= F(-1);
		}

		if(dot < F(0.95))
		{
			F angle		= math::acos(dot);
			F sinat		= math::sin(angle*t);
			F sinaomt	= math::sin(angle*(1-t));
			return (q0*sinaomt + q1*sinat) / math::sin(angle);
		}

		// use linear interpolation on small angles
		return math::lerp(t, q0, q1);
	}

	/// Simple spherical linear interpolation
	/**
	Simpler version of the splerp, this will not rotate the
	shortest way.
	*/
	template<class F>
	CQuaternion<F> simpleSlerp(F t, const CQuaternion<F>& q0, const CQuaternion<F>& q1)
	{
		F dot = dot(q0, q1);

		if((dot > -0.95f) && (dot < 0.95f))
		{
			F angle		= math::acos(dot);
			F sinat		= math::sin(angle*t); 
			F sinaomt	= math::sin(angle*(1-t));
			return (q0*sinaomt + q1*sinat) / math::sin(angle);
		}
		// use linear interpolation on small angles
		return math::lerp(t, q0, q1);
	}
	
	/**
	Spherical cubic spline quaternion interpolation
	q0->q1 is the distance to travel, a and b are control-
	quaternions, so in sequence: a, q1, q2, b
	*/
	template<class F>
	inline CQuaternion<F> splerp(F t, const CQuaternion<F>& q0, const CQuaternion<F>& q1,
								const CQuaternion<F>& a, const CQuaternion<F>& b)
	{
		return simpleSlerp(2*t*(1-t), simpleSlerp(t, q0, q1), simpleSlerp(t, a, b));
	}

	/// Linear interpolation between 2 matrices
	/**
	Linear interpolation between matrices,
	from m0 to m1. Uses lerp for the position, and slerp for the rotation.
	*/
	template<class F>
	inline CMatrix4<F> matrixLerp(F t, const CMatrix4<F>& m0, const CMatrix4<F>& m1)
	{
		CMatrix4<F> rm = matrixTranslation(math::lerp(t, m0.position(), m1.position()));
		CQuaternion<F> q = slerp(t, quatFromMatrix(m0), quatFromMatrix(m1));
		matrixSetRotation(rm, q.getMatrix3() );
		return rm;
	}

	template<class F>
	inline CMatrix4<F> matrixLerp(F t, const CMatrix3<F>& m0, const CMatrix3<F>& m1)
	{
		return slerp(t, quatFromMatrix(m0), quatFromMatrix(m1)).getMatrix3();
	}

	// Typedef Quaternion-float
	typedef CQuaternion<float> CQuaternionf;
	typedef CQuaternion<double> CQuaterniond;
	// explicit initialization...
	template class CQuaternion<float>;
	template class CQuaternion<double>;
}

#endif
