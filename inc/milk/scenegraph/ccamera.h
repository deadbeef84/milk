#ifndef MILK_CCAMERA_H_
#define MILK_CCAMERA_H_

#include "milk/helper.h"
#include "milk/scenegraph/iscenenode.h"
#include "milk/math/geometry.h"
#include "milk/math/cmatrix4.h"

namespace milk
{
	class CSceneManager;
	class IWindow;
	class IRenderTarget;

	class CCamera : public ISceneNode, public CFrustum<float>
	{
		friend class CSceneManager;
	public:
		enum Projection
		{
			PERSPECTIVE,
			PARALLEL,
		};
		enum ParallelOrientation
		{
			TOPLEFT,
			BOTTOMLEFT
		};

		CCamera()
			: m_projDirty(true), m_viewDirty(true),
			  m_nearClip(0.1f), m_farClip(100.0f),
			  m_view(1.0, 1.0),
			  m_parallelOrient(TOPLEFT)
		{
			setPerspective(90.0f, 4.0f/3.0f);
		}

		virtual ~CCamera()
		{ }

		void beginRender();
		void endRender();

		void updateFrustum(); // TEMP: FIXME: TODO: Remove this function!?
		void setFromCamera(const CCamera& camera);

		Projection getProjection() const
		{ return m_projection; }

		void setPerspective(double fov, double ratio);
		void setParallel(const CRect<float>& parallelWindow, ParallelOrientation porient=TOPLEFT);

		double getFovX();
		double getFovY();
		double getAspectRatio() const;

		const CVector2d getView() const
		{ return m_view; }
		const CRect<double> getViewRect() const
		{ return CRect<double>(-m_view, m_view); }
		const CRect<float>& getParallelWindow() const
		{ return m_parallelWindow; }
		ParallelOrientation getParallelOrientation() const
		{ return m_parallelOrient; }

		void setNearClipPlane(double nearClip)
		{ m_projDirty = true; m_nearClip = nearClip; }
		void setFarClipPlane(double farClip)
		{ m_projDirty = true; m_farClip = farClip; }
		void setClipPlanes(const CVector2d& clip)
		{ m_projDirty = true; m_nearClip = clip.x; m_farClip = clip.y; }
		void setClipPlanes(double nearClip, double farClip)
		{ m_projDirty = true; m_nearClip = nearClip; m_farClip = farClip; }

		double getNearClipPlane() const
		{ return m_nearClip; }
		double getFarClipPlane() const
		{ return m_farClip; }
		CVector2d getClipPlanes() const
		{ return CVector2d(m_nearClip, m_farClip); }

		const CMatrix4f& viewMatrix() const
		{ return m_viewMatrix; }
		const CMatrix4f& projectionMatrix() const
		{ return m_projMatrix; }

		//////////////////////////////////////////////////////////////////////////
	
		/// Space conversions
		/**
			object = ltm
			eye = view
			device = projection
			viewport = ...
		*/
		CVector2f worldToDevice(const CVector3f& world) const;
		CVector2f objectToDevice(const CVector3f& obj, const CMatrix4f& ltm) const;
		CVector2f deviceToViewport(const CVector2f& dev) const;

		/// Mark this frame dirty
		virtual void onBecomeChild()
		{ markDirty(); }

		virtual void markDirty();

	protected:
		void setupPerspective(bool apply=true);
		void setupParallel(bool apply=true);

		CMatrix4f m_projMatrix;
		CMatrix4f m_viewMatrix;
		bool m_projDirty;
		bool m_viewDirty;

		// Projection type
		Projection m_projection;
		// Common
		double m_nearClip;
		double m_farClip;
		// Perspective
		CVector2d m_view;
		// Parallel
		ParallelOrientation m_parallelOrient;
		CRect<float> m_parallelWindow;
	};
}

#endif
