#ifndef MILK_IPARAMETRICSURFACE_H_
#define MILK_IPARAMETRICSURFACE_H_

#include "milk/scenegraph/iscenenode.h"
#include "milk/math/math.h"
#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/iindexbuffer.h"
#include "milk/renderer/cmaterial.h"
#include "milk/renderer/cappearance.h"

namespace milk
{
	class IVertexBuffer;
	class IIndexBuffer;

	class IParametricSurface : public ISceneNode
	{
	public:
		IParametricSurface()
		{ }

		virtual ~IParametricSurface()
		{
			safeDelete(m_geometry.pVertexBuffer);
			safeDelete(m_geometry.pIndexBuffer);
		}

		virtual void render()
		{
			m_geometry.mat = ltm() * m_transform;
			m_pSceneManager->render(m_geometry);
		}

		void setAppearance(CAppearance *pAppearance)
		{ m_geometry.pAppearance = pAppearance; }

	protected:
		CMatrix4f m_transform;
		CGeometry m_geometry;
	};

	template<class T>
	class CParametricSurface : public IParametricSurface
	{
	public:
		CParametricSurface(int rx, int ry, const CMatrix4f& transform=CMatrix4f::IDENTITY);

		virtual ~CParametricSurface()
		{ }
	};


	template<class T>
	CParametricSurface<T>::CParametricSurface(int rx, int ry, const CMatrix4f& transform)
	{
		m_transform = transform;

		IVertexBuffer *pVertexBuffer = IVertexBuffer::create(CVertexFormat(3,3,makeTexCoordFormat(2)), (rx+1)*(ry+1));
		pVertexBuffer->lock(WRITE);
		CDataContainer<CVector3f>::iterator vit = pVertexBuffer->getVertices3f().begin();
		CDataContainer<CVector3f>::iterator nit = pVertexBuffer->getNormals3f().begin();
		CDataContainer<CVector2f>::iterator tit = pVertexBuffer->getTexCoords2f(0).begin();
		for(int y = 0; y <= ry; ++y)
		{
			for(int x = 0; x <= rx; ++x)
			{
				float fx = float(x)/float(rx);
				float fy = float(y)/float(ry);
				*(vit++) = T::position(fx,fy);
				*(nit++) = T::normal(fx,fy);
				(tit++)->set(fx,fy);
			}
		}
		pVertexBuffer->unlock();

		IIndexBuffer *pIndexBuffer = IIndexBuffer::create(GL_UNSIGNED_INT, (2*rx+4)*ry - 2);
		pIndexBuffer->lock(WRITE);
		uint *pIndex = pIndexBuffer->getIndicesui();
		for(int y = 0; y < ry; ++y)
		{
			if(y)
			{
				*(pIndex++) = (y-1)*(rx+1) + (rx );
				*(pIndex++) = (y+1)*(rx+1);
			}
			for(int x = 0; x <= rx; ++x)
			{
				*(pIndex++) = (y+1)*(rx+1) + ( x );
				*(pIndex++) = ( y )*(rx+1) + ( x );
			}
		}
		pIndexBuffer->unlock();

		m_geometry = CGeometry(pVertexBuffer, pIndexBuffer, GL_TRIANGLE_STRIP);
	}

	namespace ParametricShapes
	{
		struct Quad
		{
			static CVector3f position(float pu, float pv)
			{ return CVector3f(pu, pv, 0.0f); }
			static CVector3f normal(float, float)
			{ return CVector3f(0.0f, 0.0f, 1.0f); }
		};
		struct Circle
		{
			static CVector3f position(float pu, float pv)
			{
				float phi = 2.0f * math::PI_float * pv;
				return CVector3f(pu * math::cos(phi), pu * math::sin(phi), 0.0f);
			}
			static CVector3f normal(float, float)
			{ return CVector3f(0.0f, 0.0f, 1.0f); }
		};
		struct Tube
		{
			static CVector3f position(float pu, float pv)
			{
				float phi = 2.0f * math::PI_float * pv;
				return CVector3f(math::cos(phi), pu, math::sin(phi));
			}
			static CVector3f normal(float, float pv)
			{
				float phi = 2.0f * math::PI_float * pv;
				return normalize(CVector3f(math::cos(phi), 0.0f, math::sin(phi)));
			}
		};
		struct Sphere
		{
			static CVector3f position(float pu, float pv)
			{
				float phi = 2.0f * math::PI_float * pv;
				float theta = math::PI_float * pu;
				return CVector3f(math::sin(theta) * math::cos(phi), -math::cos(theta), math::sin(theta) * math::sin(phi));
			}
			static CVector3f normal(float pu, float pv)
			{
				return normalize(position(pu,pv));
			}
		};
	}

	typedef CParametricSurface<ParametricShapes::Quad> CParametricQuad;
	typedef CParametricSurface<ParametricShapes::Sphere> CParametricSphere;
	typedef CParametricSurface<ParametricShapes::Circle> CParametricCircle;
	typedef CParametricSurface<ParametricShapes::Tube> CParametricTube;

	class IParametricComposite : public ISceneNode
	{
	public:
		IParametricComposite()
		{
		}

		virtual ~IParametricComposite()
		{
			std::vector<IParametricSurface*>::iterator it = m_surfaces.begin();
			for(; it!= m_surfaces.end(); ++it)
			{
				removeInternalChild(*it);
				safeDelete(*it);
			}
		}

		void addAll()
		{
			std::vector<IParametricSurface*>::iterator it = m_surfaces.begin();
			for(; it!= m_surfaces.end(); ++it)
				addInternalChild(*it);
		}

		void setAppearance(CAppearance *pAppearance)
		{
			std::vector<IParametricSurface*>::iterator it = m_surfaces.begin();
			for(; it!= m_surfaces.end(); ++it)
				(*it)->setAppearance(pAppearance);
		}

	protected:
		std::vector<IParametricSurface*> m_surfaces;
	};

	class CParametricCylinder : public IParametricComposite
	{
	public:
		CParametricCylinder(int rx, int ry, const CMatrix4f& transform=CMatrix4f::IDENTITY)
		{
			m_matrix = transform;
			m_surfaces.push_back(new CParametricTube(rx, ry));
			m_surfaces.push_back(new CParametricCircle(rx, ry, matrixRotationX(math::PI_float/2.0f)));
			m_surfaces.push_back(new CParametricCircle(rx, ry, matrixTranslation(0.0f,1.0f,0.0f)*matrixRotationX(-math::PI_float/2.0f)));
			addAll();
		}

		virtual ~CParametricCylinder()
		{ }
	};
}

#endif