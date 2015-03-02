#include "milk/scenegraph/ccamera.h"
#include "milk/iwindow.h"
#include "milk/glhelper.h"
#include "milk/cimage.h"
#include "milk/renderer.h"
#include "milk/renderer/cmaterial.h"
#include "milk/scenegraph/clight.h"
#include "milk/scenegraph/cscenemanager.h"
#include "milk/renderer/irendertarget.h"
#include "milk/includes.h"
#include "milk/input.h" // TEMP
using namespace milk;
using namespace std;

void CCamera::beginRender()
{
	// setup projection
	if(m_projDirty)
	{
		if(m_projection == PERSPECTIVE)
			setupPerspective();
		else
			setupParallel();
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrix(m_projMatrix);
		glMatrixMode(GL_MODELVIEW);
	}

	// Set view-matrix
	if(m_viewDirty)
		m_viewMatrix = inverseViewFast(ltm());
	glLoadMatrix(m_viewMatrix);

	// Recalculate frustum if needed
	if(m_projDirty || m_viewDirty)
	{
		// recalculate frustum
		calculate(m_viewMatrix, m_projMatrix);
		m_projDirty = false;
		m_viewDirty = false;
	}
}

void CCamera::updateFrustum()
{
	// Set view-matrix
	if(m_viewDirty)
		m_viewMatrix = inverseViewFast(ltm());

	// Recalculate frustum if needed
	if(m_projDirty || m_viewDirty)
	{
		// recalculate frustum
		calculate(m_viewMatrix, m_projMatrix);
		m_viewDirty = false;
	}
}

void CCamera::endRender()
{
}

void CCamera::markDirty()
{
	m_viewDirty = true;
	ISceneNode::markDirty();
}

void CCamera::setFromCamera(const CCamera& camera)
{
	// Copy projection
	if(camera.getProjection() == PERSPECTIVE)
		m_view = camera.m_view;
	else if(camera.getProjection() == PARALLEL)
		setParallel(camera.getParallelWindow(), camera.getParallelOrientation());
	setClipPlanes(camera.getClipPlanes());
}

void CCamera::setPerspective(double fov, double ratio)
{
	m_projection = PERSPECTIVE;
	m_projDirty = true;
	m_view.x = math::abs(math::tan(math::toRadians(fov/2.0)));
	m_view.y = m_view.x / ratio;
}

void CCamera::setParallel(const CRect<float>& parallelWindow, ParallelOrientation porient)
{
	m_projection = PARALLEL;
	m_projDirty = true;
	m_parallelWindow = parallelWindow;
	m_parallelOrient = porient;
}

double CCamera::getFovX()
{
	return math::toDegrees(2.0*math::atan2(m_view.x, 1.0));
}

double CCamera::getFovY()
{
	return math::toDegrees(2.0*math::atan2(m_view.y, 1.0));
}

double CCamera::getAspectRatio() const
{
	return m_view.x / m_view.y;
}

void CCamera::setupPerspective(bool apply)
{
	glPushAttrib(GL_TRANSFORM_BIT);

	glMatrixMode(GL_PROJECTION);

	if(!apply)
		glPushMatrix();

	{
		glLoadIdentity();
		gluPerspective(getFovY(), getAspectRatio(), m_nearClip, m_farClip);
		//glFrustum(-m_view.x/m_nearClip, m_view.x/m_farClip, -m_view.y/m_nearClip, m_view.y/m_farClip, m_nearClip, m_farClip);
		glGetMatrix(m_projMatrix, GL_PROJECTION_MATRIX);
	}

	if(!apply)
		glPopMatrix();

	glPopAttrib();
}

void CCamera::setupParallel(bool apply)
{
	glPushAttrib(GL_TRANSFORM_BIT);

	glMatrixMode(GL_PROJECTION);

	if(!apply)
		glPushMatrix();

	{
		glLoadIdentity();
		if(m_parallelOrient == TOPLEFT)
			glOrtho(m_parallelWindow.getX1(), m_parallelWindow.getX2(), m_parallelWindow.getY2(), m_parallelWindow.getY1(), m_nearClip, m_farClip);
		else
			glOrtho(m_parallelWindow.getX1(), m_parallelWindow.getX2(), m_parallelWindow.getY1(), m_parallelWindow.getY2(), m_nearClip, m_farClip);
		glGetMatrix(m_projMatrix, GL_PROJECTION_MATRIX);
	}

	if(!apply)
		glPopMatrix();

	glPopAttrib();
}

CVector2f CCamera::worldToDevice(const CVector3f& world) const
{
	CVector4f d = m_projMatrix * m_viewMatrix * CVector4f(world, 1.0f);
	return CVector2f(d.x / d.w, d.y / d.w);
}

/*
CVector2f CCamera::objectToDevice(const CVector3f& obj) const
{
	CVector4f d = glGetMatrixf(GL_PROJECTION_MATRIX) * glGetMatrixf() * CVector4f(obj, 1.0f);
	return CVector2f(d.x / d.w, d.y / d.w);
}
*/

CVector2f CCamera::objectToDevice(const CVector3f& obj, const CMatrix4f& ltm) const
{
	CVector4f d = m_projMatrix * m_viewMatrix * ltm * CVector4f(obj, 1.0f);
	return CVector2f(d.x / d.w, d.y / d.w);
}

CVector2f CCamera::deviceToViewport(const CVector2f& dev) const
{
	return dev;
	/*
	return CVector2f(
			(1.0f + dev.x) * float(m_viewportWindow.getW()/2) + m_viewportWindow.getX1(),
			(1.0f - dev.y) * float(m_viewportWindow.getH()/2) + m_viewportWindow.getY1()
		);
	*/
}
