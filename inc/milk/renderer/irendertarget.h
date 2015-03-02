#ifndef MILK_IRENDERTARGET_H_
#define MILK_IRENDERTARGET_H_

#include "milk/math/cvector.h"
#include "milk/math/crect.h"
#include "milk/iobject.h"

namespace milk
{
	class IRenderTarget : public IObject
	{
	public:
		IRenderTarget(int w, int h)
			: m_size(w,h)
		{ }

		IRenderTarget(CVector2<int> size)
			: m_size(size)
		{ }

		virtual ~IRenderTarget()
		{ }

		virtual bool beginRender()=0;
		virtual void endRender()=0;

		template<class F>
		CRect<F> topLeft2BottomLeft(const CRect<F>& r)
		{ return CRect<F>(r.getX1(), m_size.y - r.getY1(), r.getX2(), m_size.y - r.getY2()); }

		template<class F>
		CVector2<F> topLeft2BottomLeft(const CVector2<F>& v)
		{ return CVector2<F>(v.x, m_size.y - v.y); }

		/// get the size of the window
		const CVector2<int>& size() const
		{ return m_size; }

		/// get the size of the window in rectangle format
		CRect<int> rect() const
		{ return CRect<int>(CVector2<int>(0,0), m_size); }

		float getAspectRatio() const
		{ return float(m_size.x) / (m_size.y ? (float)m_size.y : 1.0f); }

	protected:
		CVector2<int> m_size;
	};
}

#endif
