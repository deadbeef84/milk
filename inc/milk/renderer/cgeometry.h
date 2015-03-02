#ifndef MILK_CGEOMETRY_H_
#define MILK_CGEOMETRY_H_

#include "milk/math/cmatrix4.h"
#include "milk/renderer/iindexbuffer.h"
#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/cappearance.h"

namespace milk
{
	class CGeometry {
	public:
		CGeometry()
			: pVertexBuffer(0), pIndexBuffer(0),
			renderMode(GL_NONE), start(0), num(0),
			pAppearance(0)
		{ }

		/*
		CGeometry(const CGeometry& rhs)
			: pVertexBuffer(rhs.pVertexBuffer),
			  pIndexBuffer(rhs.pIndexBuffer),
			  renderMode(rhs.renderMode), start(rhs.start), num(rhs.num),
			  pAppearance(rhs.pAppearance)
		{ }
		*/

		CGeometry(IVertexBuffer *pVB, GLenum rm)
			: pVertexBuffer(pVB), pIndexBuffer(0),
			renderMode(rm), start(0), num(pVertexBuffer->numVertices()),
			pAppearance(0)
		{ }

		CGeometry(IVertexBuffer *pVB, IIndexBuffer *pIB, GLenum rm)
			: pVertexBuffer(pVB), pIndexBuffer(pIB),
			renderMode(rm), start(0), num(pIndexBuffer->numIndices()),
			pAppearance(0)
		{ }

		void render()
		{
			glPushMatrix();
			glMultMatrix(mat);

			if(pIndexBuffer)
				pIndexBuffer->draw(pVertexBuffer, renderMode, start, num);
			else
				pVertexBuffer->draw(renderMode, start, num);

			glPopMatrix();
		}

		IVertexBuffer *pVertexBuffer;
		IIndexBuffer *pIndexBuffer;
		GLenum renderMode;
		GLint start, num;

		CMatrix4f mat;
		handle<CAppearance> pAppearance;

		// TODO: Bounding volumes, culling
	};
}

#endif
