#ifndef MILK_CTRANSFORM_H_
#define MILK_CTRANSFORM_H_

#include "milk/math/cmatrix4.h"
#include "milk/iobject.h"

namespace milk
{
	class CTransform : public IObject
	{
	public:
		CTransform()
		{ }

		virtual ~CTransform()
		{ }

		CMatrix4f& matrix()
		{ return m_matrix; }

		const CMatrix4f& matrix() const
		{ return m_matrix; }

	protected:
		CMatrix4f m_matrix;
	};

	/*
	class CSmoothTransform : public CTransform
	{
	public:
		CSmoothTransform()
		{ }

		virtual ~CSmoothTransform()
		{ }

		void updateTransform();
	};
	*/
}

#endif
