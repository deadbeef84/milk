#include <cstdio>
#include "milk/renderer/imodel.h"
#include "milk/helper.h"
#include "milk/glhelper.h"
#include "milk/math/cquaternion.h"
#include "milk/timer.h"
#include "milk/cimage.h"
//#include "milk/obsolete/milk_cxmlparser.h"
#include "milk/boost.h"
#include "milk/renderer/cmodel_ms3d.h" // FIXME: TEMP?
#include "milk/renderer/cmodel_mmf.h" // FIXME: TEMP?
#include "milk/renderer/cgeometry.h"
#include "milk/scenegraph/cmodelnode.h"
#include "milk/scenegraph/cscenemanager.h"
using namespace milk;
using namespace std;

vector<IModelImporter*> IModel::ms_importers;

//////////////////////////////////////////////////////////////////////////

bool CBone::atEnd() const
{
	if(m_frame == m_animation.m_endFrame)
		return true;
	return false;
}

void CBone::setAnimation(const CAnimation& animation, float blendTime, bool children)
{
	if(blendTime > 0.0f)
	{
		updateBone(0.0f);
		m_blendFrom = matrix();

		m_animation = animation;
		m_frame = math::clamp(m_frame, m_animation.m_startFrame, m_animation.m_endFrame);

		m_blendTime = -1.0f;
		updateBone(0.0f);
		m_totalBlendTime = m_blendTime = blendTime;

		m_blendTo = matrix();
	}
	else
	{
		m_animation = animation;
		m_frame = math::clamp(m_frame, m_animation.m_startFrame, m_animation.m_endFrame);
	}

	if(children)
	{
		for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			CBone *pBone = dynamic_cast<CBone*>(*it);
			if(pBone)
				pBone->setAnimation(animation, blendTime, true);
		}
	}
}

void CBone::updateBone(float dt)
{
	if(m_blendTime > 0.0f)
	{
		// We are blending...
		m_blendTime -= dt;
		float delta = 1.0f - (m_blendTime / m_totalBlendTime);
		delta = math::clamp(delta, 0.0f, 1.0f);
		matrix() = matrixLerp(delta, m_blendFrom, m_blendTo);
	}
	else
	{
		m_frame += dt * m_animation.m_fps;
		if(m_frame > m_animation.m_endFrame)
			m_frame = m_animation.m_loop ? m_animation.m_startFrame : m_animation.m_endFrame;
		matrix() = m_pBoneSource->getBoneMatrix(m_frame);
	}

	// Set final absolute-skeleton-matrix
	CBone *pParentBone = getParentBone();
	if(!pParentBone)
		m_modelRelative = matrix();
	else
		m_modelRelative = pParentBone->m_modelRelative * matrix();

	// set the final transform
	m_transform = m_modelRelative * m_pBoneSource->getWorldInv();
}

//////////////////////////////////////////////////////////////////////////

void IModel::init()
{
	//ms_importers.push_back(new CModelImporterMS3D);
	ms_importers.push_back(new milkModelFile::CModelImporter);
}

void IModel::free()
{
	delete_range(ms_importers.begin(), ms_importers.end());
	ms_importers.clear();
}

IModel* IModel::create(string filename)
{
	for(importerList::iterator it = ms_importers.begin(); it != ms_importers.end(); ++it)
	{
		if((*it)->canImport(filename))
		{
			IModel *pModel = (*it)->load(filename);
			if(pModel)
				return pModel;
		}
	}
	throw error::milk("Failed to load model '"+filename+"'.");
}

//////////////////////////////////////////////////////////////////////////

void IModel::unload()
{
	m_meshes.clear();
	m_materials.clear();
	m_animations.clear();
	delete_range(m_boneSources.begin(), m_boneSources.end());
	m_boneSources.clear();
}

bool IModel::loaded() const
{
	return m_meshes.size() ? true : false;
}

size_t IModel::bufferSize() const
{
	size_t retval = 0;
	for(meshList::const_iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		if(it->m_pVertexBuffer)
		{
			retval += it->m_pVertexBuffer->numVertices() * it->m_pVertexBuffer->format().size();
			if(it->m_pIndexBuffer)
				retval += it->m_pIndexBuffer->numIndices() * it->m_pIndexBuffer->sizeOfFormat();
		}
	}
	return retval;
}

size_t IModel::numMeshes() const
{
	return m_meshes.size();
}

size_t IModel::numTriangles() const
{
	size_t retval = 0;
	for(meshList::const_iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		if(it->m_pVertexBuffer)
		{
			if(it->m_pIndexBuffer)
				retval += (it->m_renderMode==GL_TRIANGLE_STRIP) ? it->m_pIndexBuffer->numIndices()-2 : it->m_pIndexBuffer->numIndices()/3;
			else
				retval += (it->m_renderMode==GL_TRIANGLE_STRIP) ? it->m_pVertexBuffer->numVertices()-2 : it->m_pVertexBuffer->numVertices()/3;
		}
	}
	return retval;
}

size_t IModel::numDegenerateTriangles() const
{
	size_t retval = 0;
	for(meshList::const_iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
		if(it->m_pIndexBuffer && it->m_renderMode==GL_TRIANGLE_STRIP)
			retval += it->m_pIndexBuffer->numDegenerateTriangles();
	return retval;
}

size_t IModel::numVertices() const
{
	size_t retval = 0;
	for(meshList::const_iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
		if(it->m_pVertexBuffer)
			retval += it->m_pVertexBuffer->numVertices();
	return retval;
}

size_t IModel::numIndices() const
{
	size_t retval = 0;
	for(meshList::const_iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
		if(it->m_pIndexBuffer)
			retval += it->m_pIndexBuffer->numIndices();
	return retval;
}

size_t IModel::numMaterials() const
{
	return m_materials.size();
}

size_t IModel::numBones() const
{
	return m_boneSources.size();
}

void IModel::loadAnimations(string filename, string section, string mainsection)
{
	/*
	CXMLParser xml(filename);
	const vector<CXMLElement>& c = xml(mainsection)(section).children;
	for(vector<CXMLElement>::const_iterator it = c.begin(); it != c.end(); it++)
	{
		CAnimation animation;
		animation.m_name = it->name;
		animation.m_startFrame = (*it)("start");
		animation.m_endFrame = (*it)("end");
		if(!(*it).getChildData("fps", animation.m_fps))
			animation.m_fps = 24.0f;
		m_animations.push_back(animation);
	}
	*/
}

const CAnimation& IModel::getAnimation(string name) const
{
	for(animationList::const_iterator it = m_animations.begin(); it != m_animations.end(); it++)
		if(it->m_name == name)
			return *it;
	throw error::milk("IModel::getAnimation - Error, animation not found ("+name+")");
}

CAnimation& IModel::getAnimation(string name)
{
	for(animationList::iterator it = m_animations.begin(); it != m_animations.end(); it++)
		if(it->m_name == name)
			return *it;
	throw error::milk("IModel::getAnimation - Error, animation not found ("+name+")");
}

void IModel::createSkeleton(boneList& bones)
{
	boneSourceList::iterator it = m_boneSources.begin();
	for(; it != m_boneSources.end(); ++it)
	{
		CBone *pBone = new CBone(*it);
		bones.push_back(pBone);
		if((*it)->m_parent != -1)
		{
			BOOST_ASSERT((*it)->m_parent < bones.size());
			bones[(*it)->m_parent]->addChild(pBone);
		}
	}
}

void IModel::skinVertices(CMesh& mesh, boneList& bones)
{
	if(bones.empty())
		return;

	if(mesh.m_hwSkinning)
	{
		if(mesh.m_skinned)
		{
			mesh.m_pVertexBuffer->lock(WRITE);
			vector<CSkinnedVertex>::iterator svit = mesh.m_skinnedVertices.begin();
			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
			for(; svit != mesh.m_skinnedVertices.end(); ++svit, ++vit, ++nit)
			{
				*vit = svit->p;
				*nit = svit->n;
			}
			mesh.m_pVertexBuffer->unlock();
			mesh.m_skinned = false;
		}

		for(boneList::iterator itc = bones.begin(); itc != bones.end(); ++itc)
			(*itc)->getBoneSource()->setSkinMatrix( (*itc)->m_transform );
	}
	else
	{
		/*bool skin = false;
		for(boneList::iterator itc = bones.begin(); itc != bones.end(); ++itc)
		{
			if((*itc)->getBoneSource()->m_skinFrame != (*itc)->m_frame)
			{
				skin = true;
				break;
			}
		}
		*/
		bool skin = true;

		if(skin)
		{
			mesh.m_pVertexBuffer->lock(WRITE);
			vector<CSkinnedVertex>::iterator svit = mesh.m_skinnedVertices.begin();
			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
			for(; svit != mesh.m_skinnedVertices.end(); ++svit, ++vit, ++nit)
			{
				if(!svit->weights.empty())
				{
					CVector3f p;
					CVector3f n;
					for(weightList::iterator wit = svit->weights.begin(); wit != svit->weights.end(); ++wit)
					{
						p += bones[wit->first]->m_transform.transformPoint(svit->p) * wit->second;
						n += bones[wit->first]->m_transform.transformVector(svit->n) * wit->second;
					}
					*vit = p;
					*nit = n;
				}
			}
			mesh.m_pVertexBuffer->unlock();

			for(boneList::iterator itc = bones.begin(); itc != bones.end(); ++itc)
				(*itc)->getBoneSource()->m_skinFrame = (*itc)->m_frame;

			mesh.m_skinned = true;
		}
	}
}

void drawTangentSpace(IVertexBuffer *pVB)
{
	pVB->lock();

	CDataContainer<CVector3f>::iterator vit = pVB->getVertices3f().begin();
	CDataContainer<CVector3f>::iterator tit = pVB->getAttrib3f(0).begin();
	CDataContainer<CVector3f>::iterator bit = pVB->getAttrib3f(1).begin();
	CDataContainer<CVector3f>::iterator nit = pVB->getNormals3f().begin();

	glBegin(GL_LINES);
	for(uint i = 0; i < pVB->numVertices(); ++i, ++vit, ++tit, ++bit, ++nit)
	{
		CVector3f v0 = *vit;
		CVector3f r = *tit;
		CVector3f u = *bit;
		CVector3f a = *nit;

		glColor3(CColor3f::RED);
		glVertex3(v0);
		glVertex3(v0 + r*0.1f);
		glColor3(CColor3f::GREEN);
		glVertex3(v0);
		glVertex3(v0 + u*0.1f);
		glColor3(CColor3f::BLUE);
		glVertex3(v0);
		glVertex3(v0 + a*0.1f);
	}
	glEnd();

	pVB->unlock();
}

void IModel::draw(CModelNode *pModelNode, boneList& bones)
{
	CGeometry geometry;
	for(vector<CMesh>::iterator it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		CMesh& mesh = *it;

		// Get material
		CAppearance *pMaterial = 0;
		if(mesh.m_materialIndex >= 0)
			pMaterial = m_materials[mesh.m_materialIndex];

		// Draw buffers
		if(mesh.m_pVertexBuffer)
		{
			pMaterial->getPass(0).getProgramObject()->bind();
			skinVertices(mesh, bones);
			pMaterial->getPass(0).getProgramObject()->unbind();

			/*
			if(mesh.m_pIndexBuffer)
				mesh.m_pIndexBuffer->draw(mesh.m_pVertexBuffer, mesh.m_renderMode);
			else
				mesh.m_pVertexBuffer->draw(mesh.m_renderMode);
			*/
			if(mesh.m_pIndexBuffer)
				geometry = CGeometry(mesh.m_pVertexBuffer, mesh.m_pIndexBuffer, mesh.m_renderMode);
			else
				geometry = CGeometry(mesh.m_pVertexBuffer, mesh.m_renderMode);

			geometry.mat = pModelNode->ltm();
			geometry.pAppearance = pMaterial;

			pModelNode->getSceneManager()->render(geometry);

		}

		//drawTangentSpace(mesh.m_pVertexBuffer);
	}
}
