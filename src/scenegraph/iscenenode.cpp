#include "milk/scenegraph/iscenenode.h"
#include "milk/scenegraph/cframe.h"
#include "milk/scenegraph/ccamera.h"
#include "milk/scenegraph/cscenemanager.h"
#include "milk/glhelper.h"
using namespace milk;

uint ISceneNode::ms_nodeCount = 0;

/*
CTransform* ISceneNode::getTransformNode()
{
	if(m_pParent)
		return m_pParent->getTransformNode();
	else
		return 0;
}
*/

/*
void ISceneNode::setWinding()
{
	glFrontFace(getWinding()?GL_CCW:GL_CW);
}

bool ISceneNode::getWinding()
{
	float orient = 1;

	orient = ltm().orientation();

	if(m_pSceneManager)
	{
		CCamera *pCamera = m_pSceneManager->getActiveCamera();
		if(pCamera)
			orient *= pCamera->viewMatrix().orientation();
	}
	return orient > 0;
}

void ISceneNode::doTransform()
{
	glMultMatrix(ltm());
	setWinding();
}
*/

void ISceneNode::updateLTM()
{
	if(m_dirty)
	{
		m_ltm = m_pParent ? m_pParent->ltm()*m_matrix : m_matrix;
		m_dirty = false;
	}
}

void ISceneNode::uploadLTM()
{
	m_matrix = m_pParent ? inverseFast(m_pParent->ltm())*m_ltm : m_ltm;
	markDirty();
	m_dirty = false;
}

void ISceneNode::addInternalChild(ISceneNode *pChild)
{
	if(!pChild)
		throw error::scenegraph("Attempting to add NULL-child.");
	if(pChild->m_pParent == this)
		throw error::scenegraph("Attempting to add duplicate child.");

	pChild->makeOrphan();

	m_children.insert(m_children.end(), pChild);
	pChild->m_pParent = this;
	pChild->setSceneManager(m_pSceneManager);
	pChild->onBecomeChild();
	onAddChild(pChild);
}

void ISceneNode::removeInternalChild(ISceneNode *pChild)
{
	if(pChild->m_pParent == this)
	{
		childList::iterator it = std::find(m_children.begin(), m_children.end(), pChild);
		if(it == m_children.end())
			throw error::scenegraph("ISceneNode::removeChild(): Unable to find child."); // FIXME, en riktig SEH-exception kanske?
		onRemoveChild(this);
		pChild->onBecomeOrphan();
		m_children.erase(it);
		pChild->m_pParent = 0;
	}
	else
		throw error::scenegraph("ISceneNode::removeChild(): That child does not belong to this parent.");
}

void ISceneNode::setSceneManager(CSceneManager *pSceneManager)
{
	if(m_pSceneManager != pSceneManager)
	{
		onNewSceneManager(pSceneManager);
		m_pSceneManager = pSceneManager;
		for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
			(*it)->setSceneManager(pSceneManager);
	}
}
