#include "milk/scenegraph/cframe.h"
#include "milk/input.h"
#include "milk/glhelper.h"
using namespace milk;

/*
CMatrix4f& CFrameCopy::ltm()
{
	if(m_dirty)
	{
		CTransform *pParentTransform = getParentByType<CTransform>();
		m_ltm = pParentTransform ? pParentTransform->ltm()*m_pFrame->m_matrix : m_pFrame->m_matrix;
		m_dirty = false;
	}
	return m_ltm;
}
*/

/*
void CFrame::render()
{
	if(Input::down(SDLK_MOUSE_R))
	{
		glPushMatrix();
		{
			glLineWidth(2.0f);
			glMultMatrix(ltm());
			glBegin(GL_LINES);
			{
				const float l = 5.0f;
				glColor3(1.0f, 0.0f, 0.0f);
				glVertex3(0.0f, 0.0f, 0.0f); glVertex3(l, 0.0f, 0.0f);
				glColor3(0.0f, 1.0f, 0.0f);
				glVertex3(0.0f, 0.0f, 0.0f); glVertex3(0.0f, l, 0.0f);
				glColor3(0.0f, 0.0f, 1.0f);
				glVertex3(0.0f, 0.0f, 0.0f); glVertex3(0.0f, 0.0f, l);
			}
			glEnd();
			glLineWidth(1.0f);
		}
		glPopMatrix();
		glColor3(1.0f, 1.0f, 1.0f);
	}
}
*/
