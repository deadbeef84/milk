#include "milk/scenegraph/cscenemanager.h"
#include "milk/glhelper.h"
#include "milk/scenegraph/irenderable.h"
#include "milk/iwindow.h"
#include "milk/scenegraph/cclipplane.h"
#include "milk/scenegraph/ccamera.h"
#include "milk/scenegraph/clight.h"
#include "milk/renderer.h"
#include "milk/renderer/irenderpass.h"
#include "milk/renderer/ctexture.h"
#include "milk/renderer/cmaterial.h"
#include "milk/renderer/cappearance.h"
#include "milk/renderer/cgeometry.h"
using namespace milk;
using namespace std;

#ifndef NDEBUG
vector<CSceneManager*> CSceneManager::ms_sceneManagers;
#endif

CSceneManager::CSceneManager()
: m_fogMode(GL_NONE)
{
	m_pSceneManager = this;
#ifndef NDEBUG
	ms_sceneManagers.push_back(this);
#endif
}

CSceneManager::~CSceneManager()
{
#ifndef NDEBUG
	ms_sceneManagers.erase(remove(ms_sceneManagers.begin(), ms_sceneManagers.begin(), this), ms_sceneManagers.end());
#endif
}

void CSceneManager::render()
{
	/*
	CameraRenderInfo& globalRenderInfo = m_cameraRenderInfo[0];
	m_pCameraRenderInfo = &globalRenderInfo;
	m_pActiveRenderPass = 0;
	m_pActiveCamera = 0;
	*/

	// use standard loop because size() may change
	for(size_t i=0; i<m_renderPasses.size(); ++i)
	{
		m_pActiveRenderPass = m_renderPasses[i];
		m_pActiveCamera = m_pActiveRenderPass->getCamera();

		CameraRenderInfo& cri = m_cameraRenderInfo[m_pActiveCamera];
		m_pCameraRenderInfo = &cri;

		// Do we have geometry for this camera?
		if(!cri.calculated)
		{
			m_pActiveCamera->updateFrustum();
			// recursive render call on all children
			for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
				(*it)->renderRecursive();

			/*
			for(vector<CGeometry*>::iterator it = globalRenderInfo.geometry.begin();
				it != globalRenderInfo.geometry.end(); ++it)
			{
				CGeometry *pGeometry = *it;
				if(1) // scope-test, visibility-test
				{
					cri.geometry.push_back(pGeometry);
				}
			}
			*/
			cri.calculated = true;
		}

		// Reset all appearance active render pass
		for(vector<CGeometry>::iterator it = cri.geometry.begin();
			it != cri.geometry.end(); ++it)
		{
			CAppearance *pAppearance = it->pAppearance;
			if(!pAppearance)
				pAppearance = &CAppearance::getDefault();
			pAppearance->m_pRenderPass = 0;
		}

		// Loop all geometry for the current camera and create
		// the render queue for this pass
		m_pActiveRenderPass->clearRenderQueue();
		for(vector<CGeometry>::iterator it = cri.geometry.begin();
			it != cri.geometry.end(); ++it)
		{
			CGeometry *pGeometry = &*it;

			// prepare appearance
			CAppearance *pAppearance = pGeometry->pAppearance;
			if(!pAppearance)
				pAppearance = &CAppearance::getDefault();
			if(pAppearance->m_pRenderPass == 0)
			{
				pAppearance->prepareRenderPass(this, m_pActiveRenderPass);
				pAppearance->m_pRenderPass = m_pActiveRenderPass;
			}

			pAppearance->prepareGeometry(this, pGeometry);
		}
	}

	// Now, render all passes in descending order
	for(size_t i=m_renderPasses.size(); i>0; --i)
	{
		m_pActiveRenderPass = m_renderPasses[i-1];
		m_pActiveCamera = m_pActiveRenderPass->getCamera();

		CameraRenderInfo& cri = m_cameraRenderInfo[m_pActiveCamera];
		m_pCameraRenderInfo = &cri;

		// Reset all appearance "active render pass"
		for(vector<CGeometry>::iterator it = cri.geometry.begin();
			it != cri.geometry.end(); ++it)
		{
			CAppearance *pAppearance = it->pAppearance;
			if(!pAppearance)
				pAppearance = &CAppearance::getDefault();
			pAppearance->m_pRenderPass = 0;
		}

		// Loop all geometry, and setup each appearance once
		for(vector<CGeometry>::iterator it = cri.geometry.begin();
			it != cri.geometry.end(); ++it)
		{
			CAppearance *pAppearance = it->pAppearance;
			if(!pAppearance)
				pAppearance = &CAppearance::getDefault();
			if(pAppearance->m_pRenderPass == 0)
			{
				pAppearance->setupRenderPass(this, m_pActiveRenderPass);
				pAppearance->m_pRenderPass = m_pActiveRenderPass;
			}
		}

		m_pActiveRenderPass->render();
	}

	// reset camera render lists
	for(cameraMap::iterator it = m_cameraRenderInfo.begin();
		it != m_cameraRenderInfo.end(); ++it)
	{
		it->second.clear();
	}

	m_pActiveRenderPass = 0;
	m_pActiveCamera = 0;

	m_renderPasses.clear();
}

void CSceneManager::render(IRenderPass *pRenderPass)
{
	std::vector<IRenderPass*>::iterator it = std::find(m_renderPasses.begin(), m_renderPasses.end(), pRenderPass);
	if(it == m_renderPasses.end())
		m_renderPasses.push_back(pRenderPass);
}

void CSceneManager::render(const CGeometry& geometry)
{
	m_pCameraRenderInfo->geometry.push_back(geometry);
}

void CSceneManager::setStates()
{
	if(m_fogMode != GL_NONE)
	{
		glFogi(GL_FOG_MODE, m_fogMode);
		glFogfv(GL_FOG_COLOR, m_fogColor);
		if(m_fogMode == GL_LINEAR)
		{
			glFogf(GL_FOG_START, m_fogArg1);
			glFogf(GL_FOG_END, m_fogArg2);
		}
		else
			glFogf(GL_FOG_DENSITY, m_fogArg1);
	}
}

void CSceneManager::setupClipPlanes()
{
	CClipPlane::disableAll();
	for(vector<CClipPlane*>::iterator it = m_clipPlanes.begin(); it != m_clipPlanes.end(); ++it)
	{
		CClipPlane *pClipPlane = *it;
		if(nodesAreLinked(m_pActiveCamera, pClipPlane))
		{
			pClipPlane->enable();
			pClipPlane->apply();
		}
	}
}

void CSceneManager::setupLights()
{
	CLight::disableAll();
	for(vector<CLight*>::iterator it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		CLight *pLight = *it;
		if(nodesAreLinked(m_pActiveCamera, pLight))
		{
			pLight->enable();
			pLight->apply();
		}
	}
}
