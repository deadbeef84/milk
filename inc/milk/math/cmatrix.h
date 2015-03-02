#ifndef MILK_CMATRIX_H_
#define MILK_CMATRIX_H_

#include "milk/math/cmatrix3.h"
#include "milk/math/cmatrix4.h"

namespace milk
{
	/// Set the rotation part of a 4x4 matrix to a 3x3 matrix
	template<class F>
	inline CMatrix4<F>& matrixSetRotation(CMatrix4<F>& m4, const CMatrix3<F>& m3)
	{
		m4.m00 = m3.m00;
		m4.m01 = m3.m01;
		m4.m02 = m3.m02;
		m4.m10 = m3.m10;
		m4.m11 = m3.m11;
		m4.m12 = m3.m12;
		m4.m20 = m3.m20;
		m4.m21 = m3.m21;
		m4.m22 = m3.m22;
		return m4;
	}

	/// Loads the rotation part from a 4x4 matrix to a 3x3 matrix
	template<class F>
	inline CMatrix3<F>& matrixSetRotation3(CMatrix3<F>& m3, const CMatrix4<F>& m4)
	{
		m3.m00 = m4.m00;
		m3.m01 = m4.m01;
		m3.m02 = m4.m02;
		m3.m10 = m4.m10;
		m3.m11 = m4.m11;
		m3.m12 = m4.m12;
		m3.m20 = m4.m20;
		m3.m21 = m4.m21;
		m3.m22 = m4.m22;
		return m4;
	}

	/// Returns the rotation part (the top-left 3x3 matrix)
	template<class F>
	inline CMatrix3<F> matrixGetRotation(const CMatrix4<F>& m4)
	{
		return CMatrix3<F>(
			m4.m00, m4.m01, m4.m02,
            m4.m10, m4.m11, m4.m12,
            m4.m20, m4.m21, m4.m22);
	}

	template<class F>
	inline void matrixSetAt(CMatrix4<F>& matrix, const CVector3<F>& at, const CVector3<F>& _up)
	{
		CVector3<F> right = normalize(cross(_up, at));
		CVector3<F> up = normalize(cross(at, right));
		matrix.right() = right;
		matrix.up() = up;
		matrix.at() = at;
	}

	template<class F>
	inline void matrixSetUp(CMatrix4<F>& matrix, const CVector3<F>& up, const CVector3<F>& _at)
	{
		CVector3<F> right = normalize(cross(up, _at));
		CVector3<F> at = normalize(cross(right, up));
		matrix.right() = right;
		matrix.up() = up;
		matrix.at() = at;
	}
}

#endif
