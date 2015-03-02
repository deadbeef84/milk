#ifndef MILK_CCLIPPLANE_H_
#define MILK_CCLIPPLANE_H_

#include "milk/scenegraph/iscenenode.h"
#include "milk/math/geometry.h"

namespace milk
{
	class CClipPlane : public ISceneNode
	{
		friend class CSceneManager;
	public:
		CClipPlane()
			: m_id(-1)
		{ }
		CClipPlane(const CPlane<float>& plane)
			: m_id(-1), m_plane(plane)
		{ }
		virtual ~CClipPlane();

		CPlane<float> getPlane() const
		{ return m_plane; }

		void setPlane(const CPlane<float>& plane)
		{ m_plane = plane; }

		/// Enable clip-plane
		void enable();

		/// Disable clip-plane
		void disable();

		/// Checks whether this clip-plane is enabled or not.
		bool isEnabled();

		void apply();

		/// Get the maximum numbers of active lights.
		static uint getMaxClipPlanes();

		/// Get a free light (opengl identifier i, GL_CLIP_PLANEi), -1 if none are free.
		static int getFreeClipPlane();

		/// Disables all opengl lights
		static void disableAll();

	protected:
		virtual void onNewSceneManager(CSceneManager *pSceneManager);

		int m_id;
		CPlane<float> m_plane;
	};
}

#endif
