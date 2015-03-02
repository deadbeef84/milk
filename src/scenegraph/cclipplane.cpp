#include "milk/scenegraph/cclipplane.h"
#include "milk/scenegraph/cscenemanager.h"
#include <algorithm>
using namespace milk;

CClipPlane::~CClipPlane()
{
	disable();
}

void CClipPlane::apply()
{
	if(isEnabled())
	{
		glPushMatrix();
		{
			// LTM
			glMultMatrix(ltm());
			double eq[4] = {
				m_plane.equation.x,
				m_plane.equation.y,
				m_plane.equation.z,
				m_plane.equation.w
			};
			glClipPlane(GL_CLIP_PLANE0+m_id, eq);
		}
		glPopMatrix();
	}
}

void CClipPlane::onNewSceneManager(CSceneManager *pSceneManager)
{
	if(m_pSceneManager)
		m_pSceneManager->removeClipPlane(this);
	if(pSceneManager)
		pSceneManager->addClipPlane(this);
}

uint CClipPlane::getMaxClipPlanes()
{
	GLint temp;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &temp);
	return temp;
}

void CClipPlane::enable()
{
	if(!isEnabled())
	{
		m_id = getFreeClipPlane();
		if(m_id != -1)
			glEnable(GL_CLIP_PLANE0+m_id);
	}
}

void CClipPlane::disable()
{
	if(isEnabled())
	{
		glDisable(GL_CLIP_PLANE0 + m_id);
		m_id = -1;
	}
}

bool CClipPlane::isEnabled()
{
	if(m_id >= 0)
		return glIsEnabled(GL_CLIP_PLANE0 + m_id) ? true : false;
	return false;
}

void CClipPlane::disableAll()
{
	uint max_clip_planes = getMaxClipPlanes();
	for(uint i = 0 ; i < max_clip_planes; ++i)
		glDisable(GL_CLIP_PLANE0 + i);
}

int CClipPlane::getFreeClipPlane()
{
	uint max_lights = getMaxClipPlanes();
	for(uint i = 0 ; i < max_lights ; ++i)
		if(!glIsEnabled(GL_CLIP_PLANE0 + i))
			return i;
	return -1;
}
