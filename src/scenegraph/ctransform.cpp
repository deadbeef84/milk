#include "milk/scenegraph/cframe.h"
#include "milk/input.h"
#include "milk/math/cquaternion.h"
#include "milk/glhelper.h"
using namespace milk;

/*
void CSmoothTransform::updateTransform()
{
	CTransform *pParentTransform = getParentByType<CTransform>();
	if(pParentTransform)
	{
		const CMatrix4f& mat = pParentTransform->ltm();
		m_ltm = matrixLerp(0.1f, m_ltm, mat);
		//m_ltm = mat;
	}
}
*/

/*
void CTransform::doTransform()
{
	glMultMatrix(ltm());
	setWinding(this);
}
*/
