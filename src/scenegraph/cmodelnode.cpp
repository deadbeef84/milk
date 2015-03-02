#include "milk/scenegraph/cmodelnode.h"
#include "milk/scenegraph/cscenemanager.h"
using namespace std;
using namespace milk;


CModelNode::CModelNode()
: m_pModel(0), m_lastUpdate(0.0f),
  m_bindMaterial(true), m_animate(true), m_drawSkeleton(false), m_drawMesh(true)
{
}

CModelNode::CModelNode(IModel *pModel)
: m_pModel(0), m_lastUpdate(0.0f),
  m_bindMaterial(true), m_animate(true), m_drawSkeleton(false), m_drawMesh(true)
{
	setModel(pModel);
}

CModelNode::~CModelNode()
{
	setModel(0);
}

void CModelNode::setModel(IModel *pModel)
{
	if(m_pModel != pModel)
	{
		if(m_pModel)
		{
			m_pModel->release();

			for(boneList::iterator itc = m_bones.begin(); itc != m_bones.end(); ++itc)
			{
				if((*itc)->getParentNode() == this)
					removeInternalChild(*itc);
				(*itc)->release();
			}
			m_bones.clear();
		}
		m_pModel = pModel;
		if(m_pModel)
		{
			m_pModel->addRef();
			m_pModel->createSkeleton(m_bones);

			for(boneList::iterator itc = m_bones.begin(); itc != m_bones.end(); ++itc)
			{
				(*itc)->addRef();
				if(!(*itc)->getParentNode())
					addInternalChild(*itc);
			}
		}
	}
}

bool CModelNode::hasModel() const
{
	return m_pModel ? true : false;
}

void CModelNode::update(float dt)
{
	advanceAnimation(dt);
	//IRenderable::update(dt);
}

void CModelNode::render()
{
	if(!m_pModel)
		return;

	//if( m_pSceneManager->getActiveCamera()->isInside(CSphere<float>(ltm().position(), 16.0f) ) ) // FIXME: get bounding volume from IModel
	{
		//glPushMatrix();
		{
			//doTransform();

			if(m_drawMesh)
				m_pModel->draw(this, m_bones);

			if(m_drawSkeleton)
				drawSkeleton();
		}
		//glPopMatrix();
	}
}

void CModelNode::drawSkeleton() 
{
	if(m_drawSkeleton)
	{
		glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT|GL_TEXTURE_BIT|GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		glPointSize(5.0f);
		boneList::iterator itc = m_bones.begin();
		for(; itc != m_bones.end(); ++itc)
		{
			CBone& bone = **itc;
			CVector3f v0 = bone.m_modelRelative.transformPoint(CVector3f(0,0,0));
			CVector3f r = bone.m_modelRelative.transformVector(CVector3f(1,0,0));
			CVector3f u = bone.m_modelRelative.transformVector(CVector3f(0,1,0));
			CVector3f a = bone.m_modelRelative.transformVector(CVector3f(0,0,1));

			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_POINTS);
			glVertex3(v0);
			glEnd();

			glBegin(GL_LINES);
			glColor3(CColor3f::RED);
			glVertex3(v0);
			glVertex3(v0 + r*0.1f);
			glColor3(CColor3f::GREEN);
			glVertex3(v0);
			glVertex3(v0 + u*0.1f + CVector3f(0.01f, 0.01f, 0.01f));
			glColor3(CColor3f::BLUE);
			glVertex3(v0);
			glVertex3(v0 + a*0.1f);
			glEnd();

			glColor3f(1.0f, 1.0f, 1.0f);
			CBone *pParentBone = bone.getParentBone();
			if(pParentBone)
			{
				CVector3f v1 = pParentBone->m_modelRelative.transformPoint(CVector3f(0,0,0));
				glBegin(GL_LINES);
				glVertex3(v0);
				glVertex3(v1);
				glEnd();
			}
		}

		glPopAttrib();
	}
}

CBone* CModelNode::getBone(std::string name) const
{
	if(!m_pModel)
		return 0;

	boneList::const_iterator itc = m_bones.begin();
	for(; itc != m_bones.end(); ++itc)
		if( (*itc)->getName() == name)
			return *itc;

	return 0;
}

void CModelNode::restart()
{
}

void CModelNode::advanceAnimation(float dt)
{
	if(!m_pModel || m_bones.empty())
		return;

	if(m_animate)
	{
		// elapsed time since last update
		m_lastUpdate += dt;

		const float updateInterval = 1 / 100.0f;
		if(m_lastUpdate < updateInterval)
			return;

		// Update all bones
		boneList::iterator itc;
		for(itc = m_bones.begin(); itc != m_bones.end(); ++itc)
			(*itc)->updateBone(m_lastUpdate);

		m_lastUpdate = 0.0f;

		// Mark all sub-joints dirty
		markDirty();
	}
}

void CModelNode::setAnimation(string animation, float blendTime)
{
	for(vector<CAnimation>::iterator it = m_pModel->m_animations.begin(); it != m_pModel->m_animations.end(); ++it)
		if(it->m_name == animation)
			return setAnimation(*it, blendTime);
}

void CModelNode::setAnimation(const CAnimation& animation, float blendTime)
{
	for(boneList::iterator itc = m_bones.begin(); itc != m_bones.end(); ++itc)
		(*itc)->setAnimation(animation, blendTime, false);
}

void CModelNode::setBoneAnimation(string bone, string animation, float blendTime)
{
	CBone *pBone = getBone(bone);
	if(pBone)
		for(vector<CAnimation>::iterator it = m_pModel->m_animations.begin(); it != m_pModel->m_animations.end(); ++it)
			if(it->m_name == animation)
				return pBone->setAnimation(*it, blendTime);
}

void CModelNode::setBoneAnimation(string bone, const CAnimation& animation, float blendTime)
{
	CBone *pBone = getBone(bone);
	if(pBone)
		pBone->setAnimation(animation, blendTime);
}
