#ifndef MILK_CMATRIX4_H_
#define MILK_CMATRIX4_H_

#include <algorithm>
#include "milk/error.h"
#include "milk/math/math.h"
#include "milk/math/cvector.h"
#include "milk/math/cmatrix3.h"

namespace milk
{
	template<class> class CPlane;

	/// A 4x4 matrix class
	/**
		This is a 4x4 matrix written for flexibility, reliability and speed.
		It's template based so you can use whatever type you would like.
	*/
	template<class F=float>
	class CMatrix4
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;
		typedef CMatrix3<F> _CMatrix3;

	public:
		typedef F _F;

		/// elements are named m{row}{column}
		//row1,row2,row3,row4
		F m00, m10, m20, m30, // <- column 1
		  m01, m11, m21, m31, // <- column 2
		  m02, m12, m22, m32, // <- column 3
		  m03, m13, m23, m33; // <- column 4

		// constants
		static const CMatrix4 IDENTITY;
		static const CMatrix4 ZERO;

		/// Access element using operator[]
		F& operator[](int i)
		{ return (&m00)[i]; }

		/// Access element using operator[] (read-only)
		F operator[](int i) const
		{ return (&m00)[i]; }

		/// Access element by specifying row and column
		F& operator()(int row, int column)
		{ return (&m00)[(column<<2)+row]; }

		/// Access element by specifying row and column (read-only)
		F operator()(int row, int column) const
		{ return (&m00)[(column<<2)+row]; }

		/// Access element by specifying row and column
		F& m(int row, int column)
		{ return (&m00)[(column<<2)+row]; }

		/// Access element by specifying row and column (read-only)
		F m(int row, int column) const
		{ return (&m00)[(column<<2)+row]; }

		/// Access column
		_CVector4& column(int _column)
		{ return *reinterpret_cast<_CVector4*>(&m00 + (_column<<2)); }

		/// Access column (read-only)
		const _CVector4& column(int _column) const
		{ return *reinterpret_cast<const _CVector4*>(&m00 + (_column<<2)); }

		/// Access column (3-component)
		_CVector3& column3(int _column)
		{ return *reinterpret_cast<_CVector3*>(&m00 + (_column<<2)); }

		/// Access column (3-component) (read-only)
		const _CVector3& column3(int _column) const
		{ return *reinterpret_cast<const _CVector3*>(&m00 + (_column<<2)); }

		/// Access row (read-only)
		_CVector4 row(int _row) const
		{ return _CVector4(m(_row,0), m(_row,1), m(_row,2), m(_row,3)); }

		/// Access row (3-component) (read-only)
		_CVector3 row3(int _row) const
		{ return _CVector3(m(_row,0), m(_row,1), m(_row,2)); }

		//////////////////////////////////////////////////////////////////

		/// Default constructor, will create an identity matrix.
		CMatrix4()
		:	m00(1), m10(0), m20(0), m30(0),
			m01(0), m11(1), m21(0), m31(0),
			m02(0), m12(0), m22(1), m32(0),
			m03(0), m13(0), m23(0), m33(1)
		{ }

		/// Create from vectors
		CMatrix4(const _CVector3& r, const _CVector3& u, const _CVector3& a, const _CVector3& p)
		:	m00(r.x), m10(r.y), m20(r.z), m30(0),
			m01(u.x), m11(u.y), m21(u.z), m31(0),
			m02(a.x), m12(a.y), m22(a.z), m32(0),
			m03(p.x), m13(p.y), m23(p.z), m33(1)
		{ }

		/// Create from 3x3 matrix (rotation) and vector (translation)
		CMatrix4(const _CMatrix3& r, const _CVector3& t)
		:	m00(r.m00), m10(r.m10), m20(r.m20), m30(0),
			m01(r.m01), m11(r.m11), m21(r.m21), m31(0),
			m02(r.m02), m12(r.m12), m22(r.m22), m32(0),
			m03( t.x ), m13( t.y ), m23( t.z ), m33(1)
		{ }

		/// Copy constructor
		template<class V>
		explicit CMatrix4(const V& m)
		:	m00((F)m.m00), m10((F)m.m10), m20((F)m.m20), m30((F)m.m30),
			m01((F)m.m01), m11((F)m.m11), m21((F)m.m21), m31((F)m.m31),
			m02((F)m.m02), m12((F)m.m12), m22((F)m.m22), m32((F)m.m32),
			m03((F)m.m03), m13((F)m.m13), m23((F)m.m23), m33((F)m.m33)
		{ }

		/// create from array
		template<class V>
		explicit CMatrix4(const V* mv)
		{ for(int i = 0; i < 16; ++i) m[i] = F(mv[i]); }

		/// Create from parameters
		CMatrix4(F a00, F a01, F a02, F a03,
				 F a10, F a11, F a12, F a13,
				 F a20, F a21, F a22, F a23,
				 F a30, F a31, F a32, F a33)
		:	m00(a00), m10(a10), m20(a20), m30(a30),
			m01(a01), m11(a11), m21(a21), m31(a31),
			m02(a02), m12(a12), m22(a22), m32(a32),
			m03(a03), m13(a13), m23(a23), m33(a33)
		{ }

		//////////////////////////////////////////////////////////////////

		/*
		/// Convert to arbitrary matrix-class
		template<class V>
		operator V() const
		{ return V(*this); }
		*/

		/*
		/// Array-conversion
		operator F*()
		{ return &m00; }

		/// Array-conversion (const)
		operator const F*() const
		{ return &m00; }
		*/

		const F* ptr() const
		{ return &m00; }

		F* ptr()
		{ return &m00; }

		//////////////////////////////////////////////////////////////////

		/// Negation, will negate all elements
		CMatrix4 operator-() const
		{ CMatrix4 m; for(int i = 0; i < 16; ++i) m[i] = -(*this)[i]; return m; }

		/// Does nothing really, just return itself
		const CMatrix4& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		/// Multiply with scalar. Will multiply each element with the specified scalar.
		CMatrix4& operator*=(const F& s)
		{ for(int i = 0; i < 16; ++i) (*this)[i] *= s; return *this; }

		/// Multiplication with scalar.
		CMatrix4 operator*(const F s) const
		{ CMatrix4 r; for(int i = 0; i < 16; ++i) r[i] = (*this)[i] * s; return r; }

		/// Assignment multiplication with matrix.
		CMatrix4& operator*=(const CMatrix4& s)
		{
			CMatrix4 pre(*this);
			m00 = pre.m00*s.m00 + pre.m01*s.m10 + pre.m02*s.m20 + pre.m03*s.m30;
			m01 = pre.m00*s.m01 + pre.m01*s.m11 + pre.m02*s.m21 + pre.m03*s.m31;
			m02 = pre.m00*s.m02 + pre.m01*s.m12 + pre.m02*s.m22 + pre.m03*s.m32;
			m03 = pre.m00*s.m03 + pre.m01*s.m13 + pre.m02*s.m23 + pre.m03*s.m33;

			m10 = pre.m10*s.m00 + pre.m11*s.m10 + pre.m12*s.m20 + pre.m13*s.m30;
			m11 = pre.m10*s.m01 + pre.m11*s.m11 + pre.m12*s.m21 + pre.m13*s.m31;
			m12 = pre.m10*s.m02 + pre.m11*s.m12 + pre.m12*s.m22 + pre.m13*s.m32;
			m13 = pre.m10*s.m03 + pre.m11*s.m13 + pre.m12*s.m23 + pre.m13*s.m33;

			m20 = pre.m20*s.m00 + pre.m21*s.m10 + pre.m22*s.m20 + pre.m23*s.m30;
			m21 = pre.m20*s.m01 + pre.m21*s.m11 + pre.m22*s.m21 + pre.m23*s.m31;
			m22 = pre.m20*s.m02 + pre.m21*s.m12 + pre.m22*s.m22 + pre.m23*s.m32;
			m23 = pre.m20*s.m03 + pre.m21*s.m13 + pre.m22*s.m23 + pre.m23*s.m33;

			m30 = pre.m30*s.m00 + pre.m31*s.m10 + pre.m32*s.m20 + pre.m33*s.m30;
			m31 = pre.m30*s.m01 + pre.m31*s.m11 + pre.m32*s.m21 + pre.m33*s.m31;
			m32 = pre.m30*s.m02 + pre.m31*s.m12 + pre.m32*s.m22 + pre.m33*s.m32;
			m33 = pre.m30*s.m03 + pre.m31*s.m13 + pre.m32*s.m23 + pre.m33*s.m33;
			return *this;
		}

		/// Multiplication with matrix
		CMatrix4 operator*(const CMatrix4& m2) const
		{
			return CMatrix4(
				m00*m2.m00 + m01*m2.m10 + m02*m2.m20 + m03*m2.m30,
				m00*m2.m01 + m01*m2.m11 + m02*m2.m21 + m03*m2.m31,
				m00*m2.m02 + m01*m2.m12 + m02*m2.m22 + m03*m2.m32,
				m00*m2.m03 + m01*m2.m13 + m02*m2.m23 + m03*m2.m33,

				m10*m2.m00 + m11*m2.m10 + m12*m2.m20 + m13*m2.m30,
				m10*m2.m01 + m11*m2.m11 + m12*m2.m21 + m13*m2.m31,
				m10*m2.m02 + m11*m2.m12 + m12*m2.m22 + m13*m2.m32,
				m10*m2.m03 + m11*m2.m13 + m12*m2.m23 + m13*m2.m33,

				m20*m2.m00 + m21*m2.m10 + m22*m2.m20 + m23*m2.m30,
				m20*m2.m01 + m21*m2.m11 + m22*m2.m21 + m23*m2.m31,
				m20*m2.m02 + m21*m2.m12 + m22*m2.m22 + m23*m2.m32,
				m20*m2.m03 + m21*m2.m13 + m22*m2.m23 + m23*m2.m33,

				m30*m2.m00 + m31*m2.m10 + m32*m2.m20 + m33*m2.m30,
				m30*m2.m01 + m31*m2.m11 + m32*m2.m21 + m33*m2.m31,
				m30*m2.m02 + m31*m2.m12 + m32*m2.m22 + m33*m2.m32,
				m30*m2.m03 + m31*m2.m13 + m32*m2.m23 + m33*m2.m33);
		}

		//////////////////////////////////////////////////////////////////

		/// Adds to matrices together.
		CMatrix4& operator+=(const CMatrix4& m)
		{
			for(int i = 0; i < 16; ++i)
				(*this)[i] += m[i];
			return *this;
		}

		/// Subtracts one matrix from another.
		CMatrix4& operator-=(const CMatrix4& m)
		{
			for(int i = 0; i < 16; ++i)
				(*this)[i] -= m[i];
			return *this;
		}

		/// Adds to matrices together.
		CMatrix4 operator+(const CMatrix4& m2)
		{
			CMatrix4 m;
			for(int i = 0; i < 16; ++i)
				m[i] = (*this)[i] + m2[i];
			return m;
		}

		/// Subtracts one matrix from another.
		CMatrix4 operator-(const CMatrix4& m2)
		{
			CMatrix4 m;
			for(int i = 0; i < 16; ++i)
				m[i] = (*this)[i] - m2[i];
			return m;
		}

		//////////////////////////////////////////////////////////////////

		/// Set element values
		CMatrix4& set(
			F a00, F a01, F a02, F a03,
			F a10, F a11, F a12, F a13,
			F a20, F a21, F a22, F a23,
			F a30, F a31, F a32, F a33)
		{
			m00 = a00, m01 = a01, m02 = a02, m03 = a03,
			m10 = a10, m11 = a11, m12 = a12, m13 = a13,
			m20 = a20, m21 = a21, m22 = a22, m23 = a23,
			m30 = a30, m31 = a31, m32 = a32, m33 = a33;
			return (*this);
		}

		/// Fill the matrix elements with a specific value
		CMatrix4& fill(const F s)
		{
			F *m = &m00;
			for(int i = 0; i < 16; ++i)
				m[i] = s;
			return (*this);
		}

		/// transpose the matrix (ie. swap rows/columns)
		CMatrix4& transpose()
		{
			std::swap(m10, m01);
			std::swap(m20, m02);
			std::swap(m21, m12);
			std::swap(m30, m30);
			std::swap(m31, m31);
			std::swap(m32, m32);
			return (*this);
		}

		//////////////////////////////////////////////////////////////////

		/// Get the determinant for the matrix.
		F det() const
		{
			const F a = m22*m33 - m23*m32,
				b = m21*m33 - m23*m31,
				c = m21*m32 - m22*m31,
				d = m20*m33 - m23*m30,
				e = m20*m32 - m22*m30,
				f = m20*m31 - m21*m30;
			return ( m00*(m11*a - m12*b + m13*c)
				-m01*(m10*a - m12*d + m13*e)
				+m02*(m10*b - m11*d + m13*f)
				-m03*(m10*c - m11*e + m12*f));
		}

		F detFast() const
		{
			return ( m00*(m11*m22 - m12*m21)
					-m01*(m10*m22 - m12*m20)
					+m02*(m10*m21 - m11*m20));
		}

		/// Get the inverse of the matrix.
		CMatrix4& inverse()
		{
			const F determinant = det();
			if(determinant == F(0))
				throw error::math("CMatrix::inverse() - Invalid determinant!");

			const F a = m12*m23;
			const F b = m13*m22;
			const F c = m13*m21;
			const F d = m11*m23;
			const F e = m12*m21;
			const F f = m11*m22;
			const F g = m03*m22;
			const F h = m02*m23;
			const F i = m03*m21;
			const F j = m01*m23;
			const F k = m02*m21;
			const F l = m01*m22;
			const F M = m02*m13;
			const F n = m03*m12;
			const F o = m03*m11;
			const F p = m01*m13;
			const F q = m02*m11;
			const F r = m01*m12;

			const F s = m13*m20 - m10*m23;
			const F t = m12*m20 - m10*m22;
			const F u = m03*m20 - m00*m23;
			const F v = m02*m20 - m00*m22;

			const F aa = m03*m10 - m00*m13;
			const F ab = m02*m10 - m00*m12;
			const F ac = m11*m20 - m10*m21;
			const F ad = m01*m20 - m00*m21;
			const F ae = m01*m10 - m00*m11;

			const F detmul = (1/determinant);

			m00 = detmul*(m31*(a - b) + m32*(c - d) - m33*(e - f)),
			m01 = detmul*(m31*(g - h) - m32*(i - j) + m33*(k - l)),
			m02 = detmul*(m31*(M - n) + m32*(o - p) - m33*(q - r)),
			m03 = detmul*(m21*(n - M) - m22*(o - p) + m23*(q - r)),

			m10 = detmul*(m30*(b - a) - m32*(s) + m33*(t)),
			m11 = detmul*(m30*(h - g) + m32*(u) - m33*(v)),
			m12 = detmul*(m30*(n - M) - m32*(aa) + m33*(ab)),
			m13 = detmul*(m20*(M - n) + m22*(aa) - m23*(ab)),

			m20 = detmul*(m30*(d - c) + m31*(s) - m33*(ac)),
			m21 = detmul*(m30*(i - j) - m31*(u) + m33*(ad)),
			m22 = detmul*(m30*(p - o) + m31*(aa) - m33*(ae)),
			m23 = detmul*(m20*(o - p) - m21*(aa) + m23*(ae)),

			m30 = detmul*(m30*(e - f) - m31*(t) + m32*(ac)),
			m31 = detmul*(m30*(l - k) + m31*(v) - m32*(ad)),
			m32 = detmul*(m30*(q - r) - m31*(ab) + m32*(ae)),
			m33 = detmul*(m20*(r - q) + m21*(ab) - m22*(ae));

			return (*this);
		}

		/// Get the inverse of a matrix
		CMatrix4& inverseFast()
		{
			const F determinant = detFast();
			if(determinant == F(0))
				throw error::math("CMatrix::inverseFast() - Invalid determinant!");

			const F e = m12*m21;
			const F f = m11*m22;
			const F k = m02*m21;
			const F l = m01*m22;
			const F m = m02*m13;
			const F n = m03*m12;
			const F op= m03*m11 - m01*m13;
			const F q = m02*m11;
			const F r = m01*m12;

			const F t = m12*m20 - m10*m22;
			const F v = m02*m20 - m00*m22;

			const F A = m03*m10 - m00*m13;
			const F B = m02*m10 - m00*m12;
			const F C = m11*m20 - m10*m21;
			const F D = m01*m20 - m00*m21;
			const F E = m01*m10 - m00*m11;

			const F detmul = (1/determinant);

			// Row 1
			m00 = detmul*(f + e);
			m01 = detmul*(k - l);
			m02 = detmul*(r - q);
			m03 = detmul*(m21*(n - m) - m22*(op) + m23*(q - r));
			// Row 2
			m10 = detmul*t;
			m11 = detmul*-v;
			m12 = detmul*B;
			m13 = detmul*(m20*(m - n) + m22*(A) - m23*(B));
			// Row 3
			m20 = detmul*-C;
			m21 = detmul*D;
			m22 = detmul*-E;
			m23 = detmul*(m20*(op) - m21*(A) + m23*(E));
			return (*this);
		}

		/// Get the orientation of the base-vectors
		/** The default orientation is positive (left handed).
		OpenGL uses a right handed system.
		*/
		F orientation() const
		{
			return dot(cross(right(), up()), at());
		}

		void orthoNormalize()
		{
			_CVector3 x = normalize(right());
			_CVector3 y = normalize(up() - dot(x,up())*x);
			_CVector3 z = normalize(at() - dot(x,at())*y - dot(y,at())*y);
			right() = x;
			up() = y;
			at() = z;
		}

		//////////////////////////////////////////////////////////////////

		/// Get the position-part of the matrix (ie. the first 3 values of the last column)
		_CVector3& position()
		{ return column3(3); }

		/// Get the position-part of the matrix (ie. the first 3 values of the last column) (read-only)
		const _CVector3& position() const
		{ return column3(3); }

		/// Get the right-vector (ie. x-axis, the first 3 values of the first column)
		_CVector3& right()
		{ return column3(0); }

		/// Get the right-vector (ie. x-axis, the first 3 values of the first column) (read-only)
		const _CVector3& right() const
		{ return column3(0); }

		/// Get the up-vector (ie. y-axis, the first 3 values of the second column)
		_CVector3& up()
		{ return column3(1); }

		/// Get the up-vector (ie. y-axis, the first 3 values of the second column) (read-only)
		const _CVector3& up() const
		{ return column3(1); }

		/// Get the at-vector (ie. z-axis, the first 3 values of the third column)
		_CVector3& at()
		{ return column3(2); }

		/// Get the at-vector (ie. z-axis, the first 3 values of the third column) (read-only)
		const _CVector3& at() const
		{ return column3(2); }

		//////////////////////////////////////////////////////////////////

		/*
		/// Multiplication by vector specifying w
		_CVector3 mul(const _CVector3& v, F w) const
		{
			return _CVector3(
				m00*v[0] + m01*v[1] + m02*v[2] + m03*w,
				m10*v[0] + m11*v[1] + m12*v[2] + m13*w,
				m20*v[0] + m21*v[1] + m22*v[2] + m23*w);
		}
		*/

		_CVector3 transformPoint(const _CVector3& v) const
		{
			return _CVector3(
				m00*v.x + m01*v.y + m02*v.z + m03,
				m10*v.x + m11*v.y + m12*v.z + m13,
				m20*v.x + m21*v.y + m22*v.z + m23);
		}

		_CVector3 transformVector(const _CVector3& v) const
		{
			return _CVector3(
				m00*v.x + m01*v.y + m02*v.z,
				m10*v.x + m11*v.y + m12*v.z,
				m20*v.x + m21*v.y + m22*v.z);
		}
	};

	template<class F>
	const CMatrix4<F> CMatrix4<F>::IDENTITY(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);

	template<class F>
	const CMatrix4<F> CMatrix4<F>::ZERO(
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0);

	///////////////////////////////////////////////////////////////

	/// Multiplication with scalar.
	template<class F>
	inline CMatrix4<F> operator*(const F s, const CMatrix4<F>& m)
	{ CMatrix4<F> r; for(int i = 0; i < 16; ++i) r[i] = m[i] * s; return r; }

	/// Create a translation matrix
	template<class F>
	inline CMatrix4<F> matrixTranslation(const F x, const F y, const F z)
	{
		return CMatrix4<F>(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1);
	}

	/// Create a translation matrix from a vector
	template<class F>
	inline CMatrix4<F> matrixTranslation(const CVector3<F>& v)
	{
		return CMatrix4<F>(
			1, 0, 0, v.x,
			0, 1, 0, v.y,
			0, 0, 1, v.z,
			0, 0, 0, 1);
	}

	/// Create a scaling matrix
	template<class F>
	inline CMatrix4<F> matrixScaling(const F s)
	{
		return CMatrix4<F>(
			s, 0, 0, 0,
			0, s, 0, 0,
			0, 0, s, 0,
			0, 0, 0, 1);
	}

	/// Create a scaling matrix
	template<class F>
	inline CMatrix4<F> matrixScaling(const F x, const F y, const F z)
	{
		return CMatrix4<F>(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1);
	}

	/// Create a scaling matrix from a vector
	template<class F>
	inline CMatrix4<F> matrixScaling(const CVector3<F>& v)
	{
		return CMatrix4<F>(
			v.x, 0,   0,   0,
			0,   v.y, 0,   0,
			0,   0,   v.z, 0,
			0,   0,   0,   1);
	}

	/// Create a rotation around the x-axis matrix
	template<class F>
	inline CMatrix4<F> matrixRotationX(const F rad)
	{
		CMatrix4<F> m;
		m.m11 = m.m22 = math::cos(rad);
		m.m21 = math::sin(rad);
		m.m12 = -m.m21;
		return m;
	}

	/// Create a rotation around the y-axis matrix
	template<class F>
	inline CMatrix4<F> matrixRotationY(const F rad)
	{
		CMatrix4<F> m;
		m.m00 = m.m22 = math::cos(rad);
		m.m02 = math::sin(rad);
		m.m20 = -m.m02;
		return m;
	}

	/// Create a rotation around the z-axis matrix
	template<class F>
	inline CMatrix4<F> matrixRotationZ(const F rad)
	{
		CMatrix4<F> m;
		m.m00 = m.m11 = math::cos(rad);
		m.m10 = math::sin(rad);
		m.m01 = -m.m10;
		return m;
	}

	/// Get the euler rotation
	template<class F>
	inline CVector3<F> matrixGetEuler(const CMatrix4<F>& m)
	{
		return CVector3<F>(
			math::atan(m.m21/m.m22),
			math::asin(-m.m20),
			math::atan(m.m10/m.m00));
	}

	/// Create a rotation around arbitrary-axis matrix
	template<class F>
	inline CMatrix4<F> matrixRotationAxis(const CVector3<F>& a, F rad)
	{
		const F c = math::cos(rad);
		const F s = math::sin(rad);
		const F t = 1 - c;
		return CMatrix4<F>(
			t*a.x*a.x+c,		t*a.y*a.x-s*a.z,	t*a.z*a.x+s*a.y,	0,
			t*a.x*a.y+s*a.z,	t*a.y*a.y+c,		t*a.z*a.y-s*a.x,	0,
			t*a.x*a.z-s*a.y,	t*a.y*a.z+s*a.x,	t*a.z*a.z+c,		0,
			0,					0,					0,					1);
	}

	/// Create a look-at matrix.
	template<class F>
	inline CMatrix4<F> matrixLookAtRH(const CVector3<F>& eye, const CVector3<F>& ref, const CVector3<F>& up)
	{
		CVector3<F> zaxis = normalize(eye - ref);
		CVector3<F> xaxis = normalize(cross(up, zaxis));
		CVector3<F> yaxis = cross(zaxis, xaxis);

		/*
		return CMatrix4<F>(
			xaxis.x,	xaxis.y,	xaxis.z,	-dot(xaxis, eye),
			yaxis.x,	yaxis.y,	yaxis.z,	-dot(yaxis, eye),
			zaxis.x,	zaxis.y,	zaxis.z,	-dot(zaxis, eye),
			0,			0,			0,			1);
		*/
		return CMatrix4<F>(
			xaxis.x,	yaxis.x,	zaxis.x,	0,
			xaxis.y,	yaxis.y,	zaxis.y,	0,
			xaxis.z,	yaxis.z,	zaxis.z,	0,
			-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1);
	}

	template<class F>
	inline CMatrix4<F> matrixLookAtLH(const CVector3<F>& eye, const CVector3<F>& ref, const CVector3<F>& up)
	{
		CVector3<F> zaxis = normalize(ref - eye);
		CVector3<F> xaxis = normalize(cross(up, zaxis));
		CVector3<F> yaxis = cross(zaxis, xaxis);

		/*
		return CMatrix4<F>(
		xaxis.x,	xaxis.y,	xaxis.z,	-dot(xaxis, eye),
		yaxis.x,	yaxis.y,	yaxis.z,	-dot(yaxis, eye),
		zaxis.x,	zaxis.y,	zaxis.z,	-dot(zaxis, eye),
		0,			0,			0,			1);
		*/
		return CMatrix4<F>(
			xaxis.x,	yaxis.x,	zaxis.x,	0,
			xaxis.y,	yaxis.y,	zaxis.y,	0,
			xaxis.z,	yaxis.z,	zaxis.z,	0,
			-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1);
	}

	/// Get the determinant for the matrix.
	template<class F>
	inline F det(const CMatrix4<F>& m)
	{ return m.det(); }

	/// Get the transpose of the matrix (ie. the rows/columns swapped)
	template<class F>
	inline CMatrix4<F> transpose(const CMatrix4<F>& m)
	{
		return CMatrix4<F>(
			m.m00, m.m10, m.m20, m.m30,
			m.m01, m.m11, m.m21, m.m31,
			m.m02, m.m12, m.m22, m.m32,
			m.m03, m.m13, m.m23, m.m33);
	}

	/// Get the inverse of a matrix
	template<class F>
	inline CMatrix4<F> inverse(const CMatrix4<F>& m)
	{
		const F det = m.det();
		if(det == F(0))
			throw error::math("CMatrix::inverse() - Invalid determinant!");

		const F a = m.m12*m.m23;
		const F b = m.m13*m.m22;
		const F c = m.m13*m.m21;
		const F d = m.m11*m.m23;
		const F e = m.m12*m.m21;
		const F f = m.m11*m.m22;
		const F g = m.m03*m.m22;
		const F h = m.m02*m.m23;
		const F i = m.m03*m.m21;
		const F j = m.m01*m.m23;
		const F k = m.m02*m.m21;
		const F l = m.m01*m.m22;
		const F M = m.m02*m.m13;
		const F n = m.m03*m.m12;
		const F o = m.m03*m.m11;
		const F p = m.m01*m.m13;
		const F q = m.m02*m.m11;
		const F r = m.m01*m.m12;

		const F s = m.m13*m.m20 - m.m10*m.m23;
		const F t = m.m12*m.m20 - m.m10*m.m22;
		const F u = m.m03*m.m20 - m.m00*m.m23;
		const F v = m.m02*m.m20 - m.m00*m.m22;

		const F A = m.m03*m.m10 - m.m00*m.m13;
		const F B = m.m02*m.m10 - m.m00*m.m12;
		const F C = m.m11*m.m20 - m.m10*m.m21;
		const F D = m.m01*m.m20 - m.m00*m.m21;
		const F E = m.m01*m.m10 - m.m00*m.m11;

		const F detmul = (1/det);

		return CMatrix4<F>(
			detmul*(m.m31*(a - b) + m.m32*(c - d) - m.m33*(e - f)),
			detmul*(m.m31*(g - h) - m.m32*(i - j) + m.m33*(k - l)),
			detmul*(m.m31*(M - n) + m.m32*(o - p) - m.m33*(q - r)),
			detmul*(m.m21*(n - M) - m.m22*(o - p) + m.m23*(q - r)),

			detmul*(m.m30*(b - a) - m.m32*(s) + m.m33*(t)),
			detmul*(m.m30*(h - g) + m.m32*(u) - m.m33*(v)),
			detmul*(m.m30*(n - M) - m.m32*(A) + m.m33*(B)),
			detmul*(m.m20*(M - n) + m.m22*(A) - m.m23*(B)),

			detmul*(m.m30*(d - c) + m.m31*(s) - m.m33*(C)),
			detmul*(m.m30*(i - j) - m.m31*(u) + m.m33*(D)),
			detmul*(m.m30*(p - o) + m.m31*(A) - m.m33*(E)),
			detmul*(m.m20*(o - p) - m.m21*(A) + m.m23*(E)),

			detmul*(m.m30*(e - f) - m.m31*(t) + m.m32*(C)),
			detmul*(m.m30*(l - k) + m.m31*(v) - m.m32*(D)),
			detmul*(m.m30*(q - r) - m.m31*(B) + m.m32*(E)),
			detmul*(m.m20*(r - q) + m.m21*(B) - m.m22*(E)));
	}

	/// Get the inverse of a ON-matrix
	template<class F>
	inline CMatrix4<F> inverseFast(const CMatrix4<F>& m)
	{
		return CMatrix4f(
			m.m00, m.m10, m.m20, -m.m00*m.m03 - m.m10*m.m13 - m.m20*m.m23,
			m.m01, m.m11, m.m21, -m.m01*m.m03 - m.m11*m.m13 - m.m21*m.m23,
			m.m02, m.m12, m.m22, -m.m02*m.m03 - m.m12*m.m13 - m.m22*m.m23,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	/// same as inverseFast, but will convert from LH to RH at the same time
	template<class F>
	inline CMatrix4<F> inverseViewFast(const CMatrix4<F>& m)
	{
		return CMatrix4f(
			 m.m00, m.m10, m.m20,-m.m00*m.m03 - m.m10*m.m13 - m.m20*m.m23,
			 m.m01, m.m11, m.m21,-m.m01*m.m03 - m.m11*m.m13 - m.m21*m.m23,
			-m.m02,-m.m12,-m.m22, m.m02*m.m03 + m.m12*m.m13 + m.m22*m.m23,
			  0.0f,  0.0f,  0.0f, 1.0f );
	}

	template<class F>
	inline CMatrix4<F> matrixSwapHandedness(const CMatrix4<F>& m)
	{
		return CMatrix4f(
			m.m00, m.m01,-m.m02, m.m03,
			m.m10, m.m11,-m.m12, m.m13,
			m.m20, m.m21,-m.m22, m.m23,
			m.m30, m.m31,-m.m32, m.m33);
	}

	/*
	/// Get the inverse of a matrix
	template<class F>
	inline CMatrix4<F> inverseFast(const CMatrix4<F>& m)
	{
		const F det = m.detFast();
		if(det == F(0))
			throw error::math("CMatrix::inverseFast() - Invalid determinant!");

		const F e = m.m12*m.m21;
		const F f = m.m11*m.m22;
		const F k = m.m02*m.m21;
		const F l = m.m01*m.m22;
		const F M = m.m02*m.m13;
		const F n = m.m03*m.m12;
		const F OP = m.m03*m.m11 - m.m01*m.m13;
		const F q = m.m02*m.m11;
		const F r = m.m01*m.m12;

		const F t = m.m12*m.m20 - m.m10*m.m22;
		const F v = m.m02*m.m20 - m.m00*m.m22;

		const F A = m.m03*m.m10 - m.m00*m.m13;
		const F B = m.m02*m.m10 - m.m00*m.m12;
		const F C = m.m11*m.m20 - m.m10*m.m21;
		const F D = m.m01*m.m20 - m.m00*m.m21;
		const F E = m.m01*m.m10 - m.m00*m.m11;

		const F detmul = (1/det);

		return CMatrix4<F>(
			// Row 1
			detmul*(f + e), detmul*(k - l), detmul*(r - q), detmul*(m.m21*(n - M) - m.m22*(OP) + m.m23*(q - r)),
			// Row 2
			detmul*t, detmul*-v, detmul*B, detmul*(m.m20*(M - n) + m.m22*(A) - m.m23*(B)),
			// Row 3
			detmul*-C, detmul*D, detmul*-E, detmul*(m.m20*(OP) - m.m21*(A) + m.m23*(E)),
			// Row 4
			0.0f, 0.0f, 0.0f, 1.0f);
	}
	*/
	
	/// Create a rotation matrix
	template<class F>
	inline CMatrix4<F> matrixRotation(const F xrad, const F yrad, const F zrad)
	{
		const F cx = math::cos(xrad), sx = math::sin(xrad);
		const F cy = math::cos(yrad), sy = math::sin(yrad);
		const F cz = math::cos(zrad), sz = math::sin(zrad);
		return CMatrix4<F>(
			cz*cy,	(cz*sy*sx)-(sz*cx),	(sz*sx)+(cz*sy*cx),	0,
			sz*cy,	(cz*cx)+(sz*sy*sx),	(sz*sy*cx)-(cz*sx),	0,
			-sy,	cy*sx,				cy*cx,				0,
			0,		0,					0,					1);
	}

	/// Create a rotation matrix from vector
	template<class F>
	inline CMatrix4<F> matrixRotation(const CVector3<F>& rv)
	{
		return milk::matrixRotation(rv.x, rv.y, rv.z);
	}

	/// Create a transformation matrix from vectors (ie. pos+rotation)
	template<class F>
	inline CMatrix4<F> matrixTransform(const CVector3<F>& pos, const CVector3<F>& rot)
	{
		CMatrix4<F> mat;
		mat = matrixRotation(rot.x, rot.y, rot.z);
		mat.m03 = pos.x;
		mat.m13 = pos.y;
		mat.m23 = pos.z;
		return mat;
	}

	/// Create a transformation matrix (ie. pos+rotation)
	template<class F>
	inline CMatrix4<F> matrixTransform(const F posx, const F posy, const F posz, const F rotx, const F roty, const F rotz)
	{
		CMatrix4<F> mat;
		mat = matrixRotation(rotx, roty, rotz);
		mat.m03 = posx;
		mat.m13 = posy;
		mat.m23 = posz;
		return mat;
	}

	template<class F>
	inline CMatrix4<F> matrixReflection(const CPlane<F>& p)
	{
		const CVector3<F>& n = p.getNormal();
		return CMatrix4<F>(
			CVector3f::XAXIS-2*n.x*n,
			CVector3f::YAXIS-2*n.y*n,
			CVector3f::ZAXIS-2*n.z*n,
			-2*p.getDistance()*n );
	}

	/*
	/// Multiplication by vector3
	template<class F>
	inline CVector3<F> operator*(const CMatrix3<F>& m, const CVector3<F>& v)
	{
		return CVector3<F>(
			m.m00*v.x + m.m10*v.y + m.m20*v.z,
			m.m01*v.x + m.m11*v.y + m.m21*v.z,
			m.m02*v.x + m.m12*v.y + m.m22*v.z);
	}

	/// vector3 multiplied by matrix3
	template<class F>
	inline CVector3<F> operator*(const CVector3<F>& v, const CMatrix3<F>& m)
	{
		return CVector3<F>(
			m.m00*v.x + m.m01*v.y + m.m02*v.z,
			m.m10*v.x + m.m11*v.y + m.m12*v.z,
			m.m20*v.x + m.m21*v.y + m.m22*v.z);
	}
	*/

	/*
	/// Multiplication by vector3 (w=1)
	template<class F>
	inline CVector3<F> operator*(const CMatrix4<F>& m, const CVector3<F>& v)
	{
		return CVector3<F>(
			m.m00*v[0] + m.m01*v[1] + m.m02*v[2] + m.m03,
			m.m10*v[0] + m.m11*v[1] + m.m12*v[2] + m.m13,
			m.m20*v[0] + m.m21*v[1] + m.m22*v[2] + m.m23);
	}
	*/

	/// matrix4 multiplied by vector3 (vector is multiplied as a column-vector)
	template<class F>
	inline CVector4<F> operator*(const CMatrix4<F>& m, const CVector4<F>& v)
	{
		return CVector4<F>(
			m.m00*v.x + m.m01*v.y + m.m02*v.z + m.m03*v.w,
			m.m10*v.x + m.m11*v.y + m.m12*v.z + m.m13*v.w,
			m.m20*v.x + m.m21*v.y + m.m22*v.z + m.m23*v.w,
			m.m30*v.x + m.m31*v.y + m.m32*v.z + m.m33*v.w);
	}

	/// vector4 multiplied by matrix4 (vector is multiplied as a row-vector)
	template<class F>
	inline CVector4<F> operator*(const CVector4<F>& v, const CMatrix4<F>& m)
	{
		return CVector4<F>(
			m.m00*v.x + m.m10*v.y + m.m20*v.z + m.m30*v.w,
			m.m01*v.x + m.m11*v.y + m.m21*v.z + m.m31*v.w,
			m.m02*v.x + m.m12*v.y + m.m22*v.z + m.m32*v.w,
			m.m03*v.x + m.m13*v.y + m.m23*v.z + m.m33*v.w);
	}

	// Typedef Matrix4-float
	typedef CMatrix4<float> CMatrix4f;
	typedef CMatrix4<double> CMatrix4d;
	// explicit initialization...
	template class CMatrix4<float>;
}

#endif
