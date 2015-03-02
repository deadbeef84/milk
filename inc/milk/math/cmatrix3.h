#ifndef MILK_CMATRIX3_H_
#define MILK_CMATRIX3_H_

#include <algorithm>
#include "milk/error.h"
#include "milk/math/math.h"
#include "milk/math/cvector.h"

namespace milk
{
	/// A 3x3 matrix class
	/**
		This is a 3x3 matrix written for flexibility, reliability and speed.
		It's template based so you can use whatever type you would like.
	*/
	template<class F=float>
	class CMatrix3
	{
	private:
		typedef CVector3<F> _CVector3;
		typedef CVector4<F> _CVector4;

	public:
		typedef F _F;

		/// elements are namned m{row}{column}
		//row1,row2,row3
		F m00, m10, m20,
		  m01, m11, m21,
		  m02, m12, m22;

		/// Access element using operator[]
		F& operator[](int i)
		{ return (&m00)[i]; }

		/// Access element using operator[] (read-only)
		F operator[](int i) const
		{ return (&m00)[i]; }

		/// Access element by specifying row and column
		F& operator()(int row, int column)
		{ return (&m00)[(column*3)+row]; }

		/// Access element by specifying row and column (read-only)
		F operator()(int row, int column) const
		{ return (&m00)[(column*3)+row]; }

		/// Access element by specifying row and column
		F& m(int row, int column)
		{ return (&m00)[(column*3)+row]; }

		/// Access element by specifying row and column (read-only)
		F m(int row, int column) const
		{ return (&m00)[(column*3)+row]; }

		/// Access column
		_CVector3& column(int _column)
		{ return *reinterpret_cast<_CVector3*>(&m00 + (_column*3)); }

		/// Access column (read-only)
		const _CVector3& column(int _column) const
		{ return *reinterpret_cast<const _CVector3*>(&m00 + (_column*3)); }

		/// Access row (read-only)
		_CVector3 row(int _row) const
		{ return _CVector3(m(_row,0), m(_row,1), m(_row,2)); }

		//////////////////////////////////////////////////////////////////

		/// Default constructor, will create an identity matrix.
		CMatrix3()
		:	m00(1), m10(0), m20(0),
			m01(0), m11(1), m21(0),
			m02(0), m12(0), m22(1)
		{ }

		CMatrix3(const _CVector3& r, const _CVector3& u, const _CVector3& a)
		:	m00(r.x), m10(r.y), m20(r.z),
			m01(u.x), m11(u.y), m21(u.z),
			m02(a.x), m12(a.y), m22(a.z)
		{ }

		/// Copy constructor
		template<class V>
		explicit CMatrix3(const V& m)
		:	m00((F)m.m00), m10((F)m.m10), m20((F)m.m20),
			m01((F)m.m01), m11((F)m.m11), m21((F)m.m21),
			m02((F)m.m02), m12((F)m.m12), m22((F)m.m22)
		{ }

		/// create from array
		template<class V>
		explicit CMatrix3(const V* mv)
		{ for(int i = 0; i < 9; ++i) m[i] = F(mv[i]); }

		/// Create from parameters
		CMatrix3(F a00, F a01, F a02,
				 F a10, F a11, F a12,
				 F a20, F a21, F a22)
		:	m00(a00), m10(a10), m20(a20),
			m01(a01), m11(a11), m21(a21),
			m02(a02), m12(a12), m22(a22)
		{ }

		//////////////////////////////////////////////////////////////////

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
		CMatrix3 operator-() const
		{ CMatrix3 m; for(int i = 0; i < 9; ++i) m[i] = -(*this)[i]; return m; }

		/// Does nothing really, just return itself
		const CMatrix3& operator+() const
		{ return *this; }

		//////////////////////////////////////////////////////////////////

		/// Multiply with scalar. Will multiply each element with the specified scalar.
		CMatrix3& operator*=(const F& s)
		{ for(int i = 0; i < 9; ++i) (*this)[i] *= s; return *this; }

		/// Multiplication with scalar.
		CMatrix3 operator*(const F s) const
		{ CMatrix3 r; for(int i = 0; i < 9; ++i) r[i] = (*this)[i] * s; return r; }

		/// Assignment multiplication with matrix.
		CMatrix3& operator*=(const CMatrix3& s)
		{
			CMatrix3 p(*this);

			m00 = p.m00*s.m00 + p.m01*s.m10 + p.m02*s.m20;
			m01 = p.m00*s.m01 + p.m01*s.m11 + p.m02*s.m21;
			m02 = p.m00*s.m02 + p.m01*s.m12 + p.m02*s.m22;

			m10 = p.m10*s.m00 + p.m11*s.m10 + p.m12*s.m20;
			m11 = p.m10*s.m01 + p.m11*s.m11 + p.m12*s.m21;
			m12 = p.m10*s.m02 + p.m11*s.m12 + p.m12*s.m22;

			m20 = p.m20*s.m00 + p.m21*s.m10 + p.m22*s.m20;
			m21 = p.m20*s.m01 + p.m21*s.m11 + p.m22*s.m21;
			m22 = p.m20*s.m02 + p.m21*s.m12 + p.m22*s.m22;

			return (*this);
		}

		/// Multiplication with matrix
		CMatrix3 operator*(const CMatrix3& s)
		{
			CMatrix3 p(*this);
			return CMatrix3(
				p.m00*s.m00 + p.m01*s.m10 + p.m02*s.m20,
				p.m00*s.m01 + p.m01*s.m11 + p.m02*s.m21,
				p.m00*s.m02 + p.m01*s.m12 + p.m02*s.m22,

				p.m10*s.m00 + p.m11*s.m10 + p.m12*s.m20,
				p.m10*s.m01 + p.m11*s.m11 + p.m12*s.m21,
				p.m10*s.m02 + p.m11*s.m12 + p.m12*s.m22,

				p.m20*s.m00 + p.m21*s.m10 + p.m22*s.m20,
				p.m20*s.m01 + p.m21*s.m11 + p.m22*s.m21,
				p.m20*s.m02 + p.m21*s.m12 + p.m22*s.m22);
		}

		//////////////////////////////////////////////////////////////////

		/// Adds to matrices together.
		CMatrix3& operator+=(const CMatrix3& m)
		{
			for(int i = 0; i < 9; ++i)
				(*this)[i] += m[i];
			return *this;
		}

		/// Subtracts one matrix from another.
		CMatrix3& operator-=(const CMatrix3& m)
		{
			for(int i = 0; i < 9; ++i)
				(*this)[i] -= m[i];
			return *this;
		}

		/// Adds to matrices together.
		CMatrix3 operator+(const CMatrix3& m2)
		{
			CMatrix3 m;
			for(int i = 0; i < 9; ++i)
				m[i] = (*this)[i] + m2[i];
			return m;
		}

		/// Subtracts one matrix from another.
		CMatrix3 operator-(const CMatrix3& m2)
		{
			CMatrix3 m;
			for(int i = 0; i < 9; ++i)
				m[i] = (*this)[i] - m2[i];
			return m;
		}

		//////////////////////////////////////////////////////////////////

		/// Set element values
		CMatrix3& set(
			F a00, F a01, F a02,
			F a10, F a11, F a12,
			F a20, F a21, F a22)
		{
			m00 = a00, m01 = a01, m02 = a02,
			m10 = a10, m11 = a11, m12 = a12,
			m20 = a20, m21 = a21, m22 = a22;
			return (*this);
		}

		/// Fill the matrix elements with a specific value
		CMatrix3& fill(const F s)
		{
			F *m = &m00;
			for(int i = 0; i < 9; ++i)
				m[i] = s;
			return (*this);
		}

		/// Set the matrix to the identity matrix
		CMatrix3& identity()
		{
			m01 = m02 = m10 = m12 = m20 = m21 = F(0);
			m00 = m11 = m22 = F(1);
			return (*this);
		}

		/// Check if the matrix is an identity matrix.
		bool isIdentity() const
		{
			for(int i = 0; i < 3; ++i)
				for(int j = 0; j < 3; j++)
					if(i == j)
						if(m(i,j) == F(1))
							continue;
						else
							return false;
					else
						if(m(i,j) == F(0))
							continue;
						else
							return false;
			return true;
		}

		/// transpose the matrix (ie. swap rows/columns)
		CMatrix3& transpose()
		{
			std::swap(m10, m01);
			std::swap(m20, m02);
			std::swap(m21, m12);
			return (*this);
		}

		//////////////////////////////////////////////////////////////////

		/// Get the determinant for the matrix.
		F det() const
		{
			return ( m00*(m11*m22 - m12*m21)
					-m01*(m10*m22 - m12*m20)
					+m02*(m10*m21 - m11*m20));
		}

		/// Get the inverse of the matrix.
		CMatrix3& inverse()
		{
			const F determinant = det();
			if(determinant == F(0))
				throw error::math("CMatrix::inverse() - Invalid determinant!");

			const F detmul = 1/determinant;
			m00 = detmul*(m11*m22-m12*m21),
			m01 = -detmul*(m01*m22-m02*m21),
			m02 = detmul*(m01*m12-m02*m11),
			m10 = -detmul*(m10*m22-m12*m20),
			m11 = detmul*(m00*m22-m20*m02),
			m12 = -detmul*(m00*m12-m02*m10),
			m20 = detmul*(m10*m21-m11*m20),
			m21 = -detmul*(m00*m21-m01*m20),
			m22 = detmul*(m00*m11-m01*m10);
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

		//////////////////////////////////////////////////////////////////

		/// Get the right-vector (ie. x-axis, the first 3 values of the first column)
		_CVector3& right()
		{ return column(0); }

		/// Get the right-vector (ie. x-axis, the first 3 values of the first column) (read-only)
		const _CVector3& right() const
		{ return column(0); }

		/// Get the up-vector (ie. y-axis, the first 3 values of the second column)
		_CVector3& up()
		{ return column(1); }

		/// Get the up-vector (ie. y-axis, the first 3 values of the second column) (read-only)
		const _CVector3& up() const
		{ return column(1); }

		/// Get the at-vector (ie. z-axis, the first 3 values of the third column)
		_CVector3& at()
		{ return column(2); }

		/// Get the at-vector (ie. z-axis, the first 3 values of the third column) (read-only)
		const _CVector3& at() const
		{ return column(2); }
	};

///////////////////////////////////////////////////////////////

	/// Check if the matrix is an identity matrix.
	template<class F>
	inline bool isIdentity(const CMatrix3<F>& m)
	{ return m.isIdentity(); }

	/// Multiplication with scalar.
	template<class F>
	inline CMatrix3<F> operator*(const F s, const CMatrix3<F>& m)
	{ CMatrix3<F> r; for(int i = 0; i < 9; ++i) r[i] = m[i] * s; return r; }

	/// Create a scaling matrix
	template<class F>
	inline CMatrix3<F> matrixScaling3(const F s)
	{
		return CMatrix3<F>(
			s, 0, 0,
			0, s, 0,
			0, 0, s);
	}

	/// Create a scaling matrix
	template<class F>
	inline CMatrix3<F> matrixScaling3(const F x, const F y, const F z)
	{
		return CMatrix3<F>(
			x, 0, 0,
			0, y, 0,
			0, 0, z);
	}

	/// Create a scaling matrix from a vector
	template<class F>
	inline CMatrix3<F> matrixScaling3(const CVector3<F>& v)
	{
		return CMatrix3<F>(
			v.x, 0,   0,
			0,   v.y, 0,
			0,   0,   v.z);
	}

	/// Create a rotation around the x-axis matrix
	template<class F>
	inline CMatrix3<F> matrixRotationX3(const F rad)
	{
		F c = math::cos(rad);
		F s = math::sin(rad);
		return CMatrix3<F>(
			1, 0, 0,
			0, c,-s,
			0, s, c);
	}

	/// Create a rotation around the y-axis matrix
	template<class F>
	inline CMatrix3<F> matrixRotationY3(const F rad)
	{
		F c = math::cos(rad);
		F s = math::sin(rad);
		return CMatrix3<F>(
			 c, 0, s,
			 0, 1, 0,
			-s, 0, c);
	}

	/// Create a rotation around the z-axis matrix
	template<class F>
	inline CMatrix3<F> matrixRotationZ3(const F rad)
	{
		F c = math::cos(rad);
		F s = math::sin(rad);
		return CMatrix3<F>(
			c,-s, +,
			s, c, 0,
			0, 0, 1);
	}

	/// Get the euler rotation
	template<class F>
	inline CVector3<F> matrixGetEuler(const CMatrix3<F>& m)
	{
		return CVector3<F>(
			math::atan(m.m21/m.m22),
			math::asin(-m.m20),
			math::atan(m.m10/m.m00));
	}

	/// Create a rotation around arbitrary-axis matrix
	template<class F>
	inline CMatrix3<F> matrixRotationAxis3(const CVector3<F>& a, F rad)
	{
		const F c = math::cos(rad);
		const F s = math::sin(rad);
		const F t = 1 - c;
		return CMatrix3<F>(
			t*a.x*a.x+c,		t*a.y*a.x-s*a.z,	t*a.z*a.x+s*a.y,
			t*a.x*a.y+s*a.z,	t*a.y*a.y+c,		t*a.z*a.y-s*a.x,
			t*a.x*a.z-s*a.y,	t*a.y*a.z+s*a.x,	t*a.z*a.z+c);
	}

	/// Get the determinant for the matrix.
	template<class F>
	inline F det(const CMatrix3<F>& m)
	{ return m.det(); }

	/// Get the transpose of the matrix (ie. the rows/columns swapped)
	template<class F>
	inline CMatrix3<F> transpose(const CMatrix3<F>& m)
	{
		return CMatrix3<F>(
			m.m00, m.m10, m.m20,
			m.m01, m.m11, m.m21,
			m.m02, m.m12, m.m22);
	}

	/// Get the inverse of a matrix
	template<class F>
	inline CMatrix3<F> inverse(const CMatrix3<F>& m)
	{
		const F determinant = m.det();
		if(determinant == F(0))
			throw error::math("CMatrix::inverse() - Invalid determinant!");

		const F detmul = 1/determinant;
		return CMatrix3<F>(
			  detmul*(m.m11*m.m22-m.m12*m.m21),
			 -detmul*(m.m01*m.m22-m.m02*m.m21),
			  detmul*(m.m01*m.m12-m.m02*m.m11),

			 -detmul*(m.m10*m.m22-m.m12*m.m20),
			  detmul*(m.m00*m.m22-m.m20*m.m02),
		     -detmul*(m.m00*m.m12-m.m02*m.m10),

			  detmul*(m.m10*m.m21-m.m11*m.m20),
			 -detmul*(m.m00*m.m21-m.m01*m.m20),
			  detmul*(m.m00*m.m11-m.m01*m.m10));
	}
	
	/// Create a rotation matrix
	template<class F>
	inline CMatrix3<F> matrixRotation3(const F xrad, const F yrad, const F zrad)
	{
		const F cx = math::cos(xrad), sx = math::sin(xrad);
		const F cy = math::cos(yrad), sy = math::sin(yrad);
		const F cz = math::cos(zrad), sz = math::sin(zrad);
		return CMatrix3<F>(
			cz*cy,	(cz*sy*sx)-(sz*cx),	(sz*sx)+(cz*sy*cx),
			sz*cy,	(cz*cx)+(sz*sy*sx),	(sz*sy*cx)-(cz*sx),
			-sy,	cy*sx,				cy*cx);
	}

	/// Create a rotation matrix from vector
	template<class F>
	inline CMatrix3<F> matrixRotation3(const CVector3<F>& rv)
	{
		return milk::matrixRotation3(rv.x, rv.y, rv.z);
	}

	/// Multiplication by vector3
	template<class F>
	inline CVector3<F> operator*(const CMatrix3<F>& m, const CVector3<F>& v)
	{
		return CVector3<F>(
			m.m00*v.x + m.m01*v.y + m.m02*v.z,
			m.m10*v.x + m.m11*v.y + m.m12*v.z,
			m.m20*v.x + m.m21*v.y + m.m22*v.z);
	}

	/// vector3 multiplied by matrix3
	template<class F>
	inline CVector3<F> operator*(const CVector3<F>& v, const CMatrix3<F>& m)
	{
		return CVector3<F>(
			m.m00*v.x + m.m10*v.y + m.m20*v.z,
			m.m01*v.x + m.m11*v.y + m.m21*v.z,
			m.m02*v.x + m.m12*v.y + m.m22*v.z);
	}

	/*
	/// Multiplication by vector4, ignoring w
	template<class F>
	inline CVector3<F> operator*(const CMatrix3<F>& m, const CVector4<F>& v)
	{
		return CVector3<F>(
			m.m00*v.x + m.m01*v.y + m.m02*v.z,
			m.m10*v.x + m.m11*v.y + m.m12*v.z,
			m.m20*v.x + m.m21*v.y + m.m22*v.z);
	}
	*/

	// Typedef Matrix3-float
	typedef CMatrix3<float> CMatrix3f;
	typedef CMatrix3<double> CMatrix3d;
	// explicit initialization...
	template class CMatrix3<float>;
}

#endif
