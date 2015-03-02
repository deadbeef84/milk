#include "milk/renderer/irenderpass.h"
#include "milk/renderer.h"
#include "milk/renderer/cappearance.h"
#include "milk/renderer/cgeometry.h"
#include "milk/scenegraph/cscenemanager.h"
using namespace milk;

/*

void CCamera::notifyRender()
{
	if(m_updateInterval > 0.0f)
	{
		if(m_lastUpdate < m_updateInterval)
			return;
		m_lastUpdate = 0.0f;
	}
}

void CCamera::update(float dt)
{
	m_lastUpdate += dt;
	ISceneNodeLinker::update(dt);
}

	// Copy states
	m_clear = camera.m_clear;
	m_clearColor = camera.m_clearColor;
	m_clearDepth = camera.m_clearDepth;
	m_cullFace = camera.m_cullFace;
	m_fog = camera.m_fog;

*/

bool IRenderPass::beginRender()
{
	if(m_pRenderTarget->beginRender())
	{
		// Set viewport
		CRect<int> glViewportRect = m_pRenderTarget->topLeft2BottomLeft(m_viewportWindow);
		glViewport(glViewportRect.getX1(), glViewportRect.getY1(), glViewportRect.getW(), glViewportRect.getH());

		// Clearing
		if(m_clear != NOCLEAR)
		{
			bool needScissor = (m_viewportWindow != m_pRenderTarget->rect());

			// Setup scissor-test
			if(needScissor)
			{
				glPushAttrib(GL_SCISSOR_BIT);
				CRect<int> glViewportRect = m_pRenderTarget->topLeft2BottomLeft(m_viewportWindow);
				glScissor(glViewportRect.getX1(), glViewportRect.getY1(), glViewportRect.getW(), glViewportRect.getH());
				glEnable(GL_SCISSOR_TEST);
			}

			// do the clearing
			clear();

			// reset scissor-test
			if(needScissor)
				glPopAttrib();
		}

		// Setup our camera
		m_pCamera->beginRender();
		return true;
	}
	return false;
}

void IRenderPass::endRender()
{
	m_pRenderTarget->endRender();
}

void IRenderPass::clear()
{
	// do the clearing
	GLbitfield bf = 0;
	if(m_clear&CLEARCOLOR)
	{
		glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
		bf |= GL_COLOR_BUFFER_BIT;
	}
	if(m_clear&CLEARZ)
	{
		glClearDepth(m_clearDepth);
		bf |= GL_DEPTH_BUFFER_BIT;
	}
	if(m_clear&CLEARSTENCIL)
	{
		//glClearStencil();
		bf |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(bf);
}

void IRenderPass::render()
{
	if(beginRender())
	{
		ITexture::reset();

		m_pCamera->getSceneManager()->setupLights();
		m_pCamera->getSceneManager()->setupClipPlanes();

		float camOrient = m_pCamera->viewMatrix().orientation();

		sort_heap(m_renderQueue.begin(), m_renderQueue.end(), sortRender);

		for(RenderQueue::iterator it = m_renderQueue.begin();
			it != m_renderQueue.end(); ++it)
		{
			CGeometry *pGeometry = it->first;
			CPass *pPass = it->second;

			// TODO: pGeometry set glColor... set only if material.trackVertexColor

			// TODO: set CCW/CW
			float orient = pGeometry->mat.orientation();
			PolygonMode::ms_swapWinding = (orient*camOrient>0);
			PolygonMode::setWindingGL(PolygonMode::ms_winding);

			pPass->bind();
			pGeometry->render();
		}

		ITexture::reset();

		endRender();
	}
}

void IRenderPass::setRenderTarget(IRenderTarget *pRenderTarget)
{
	m_pRenderTarget = pRenderTarget;
	if(pRenderTarget)
		setViewport(pRenderTarget->rect());
}
