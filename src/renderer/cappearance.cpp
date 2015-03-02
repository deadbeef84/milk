#include "milk/renderer/cappearance.h"
#include "milk/scenegraph/cscenemanager.h"
using namespace milk;

CAppearance* CAppearance::ms_pDefault = 0;

void CAppearance::prepareGeometry(CSceneManager *pSceneManager, CGeometry *pGeometry) const
{
	IRenderPass *pRenderPass = pSceneManager->getActiveRenderPass();
	uint scope = pRenderPass->getPassScope();
	for(passList::const_iterator it = m_passes.begin();
		it != m_passes.end(); ++it)
	{
		CPass *pPass = *it;
		if(pPass->getPassScope() & scope)
			pRenderPass->addToRenderQueue(pGeometry, pPass);
	}
}
