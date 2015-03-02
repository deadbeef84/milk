#ifndef MILK_IRENDERABLE_H_
#define MILK_IRENDERABLE_H_

#include "milk/scenegraph/iscenenode.h"

namespace milk
{
	/*
	const uint
		SHADOW_NONE = 0,
		SHADOW_CAST = 1,
		SHADOW_RECIEVE = 2;

	class CSceneManager;
	class IRenderable : public ISceneNode
	{
		friend class CSceneManager;
	public:
		IRenderable()
			: m_priority(0)
		{ m_type = TYPE_3D|TYPE_SHADOW_CAST|TYPE_SHADOW_RECIEVE; }

		virtual ~IRenderable()
		{ }

		virtual void render() = 0;
		virtual void norender() { }

		void setPriority(int priority)
		{ m_priority = priority; }
		int getPriority() const
		{ return m_priority; }

	protected:
		virtual void onNewSceneManager(CSceneManager *pSceneManager);

		int m_priority;
	};
	*/
}

#endif
