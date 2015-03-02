#ifndef MILK_CFRAME_H_
#define MILK_CFRAME_H_

//#include "milk/scenegraph/iscenenode.h"
#include "milk/scenegraph/ctransform.h"
#include "milk/math/cmatrix4.h"

namespace milk
{
	/*
	class CFrame : public CTransform
	{
		friend class CFrameCopy;
	public:
		CFrame()
			: m_dirty(true)
		{ }

		virtual ~CFrame()
		{ }

		virtual void markDirty();

		/// Mark this frame dirty when it becomes a child
		virtual void onBecomeChild()
		{ m_dirty = false; markDirty(); }

		//virtual void render();



	};

	class CFrameCopy : public CFrame
	{
	public:
		class CFrameCopy_internal : public ISceneNode
		{
		public:
			CFrameCopy_internal(CFrameCopy *pFrame)
				: m_pFrame(pFrame)
			{ }

			virtual void markDirty()
			{ m_pFrame->markDirty(); }

		protected:
			CFrameCopy *m_pFrame;
		};

		CFrameCopy(CFrame *pFrame)
			: m_pFrame(pFrame)
		{ m_pFrameInt = new CFrameCopy_internal(this); m_pFrameInt->addRef(); m_pFrame->addChild(m_pFrameInt); }

		virtual ~CFrameCopy()
		{ m_pFrameInt->release(); }

		virtual CMatrix4f& matrix()
		{ return m_pFrame->matrix(); }

		virtual const CMatrix4f& matrix() const
		{ return m_pFrame->matrix(); }

		virtual CMatrix4f& ltm();

	protected:
		CFrame *m_pFrame;
		CFrameCopy_internal *m_pFrameInt;
	};
	*/
}

#endif
