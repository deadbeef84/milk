/*#include "milk/cskybox.h"
#include "milk/cimage.h"
using namespace std;
using namespace milk;

void CSkyBox::load(const string& dir, const string& end)
{
	const string names[6] = {"back", "front", "left", "right", "bottom", "top"};
	for (int i=0 ; i<6 ; ++i)
		m_textures[i].reset(new CTexture2D(CImage(dir+names[i]+end), ImageType(RGBA32, false, Quality(Clamp, Clamp))));
}

bool CSkyBox::loaded() const
{
	return m_textures[0];
}

void CSkyBox::unload()
{
	for (int i=0 ; i<6 ; ++i)
		m_textures[i].reset();
}

void CSkyBox::draw() const
{
	const float distance = 1;

	glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);	//disable writing to z-buffer
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);

		m_textures[0]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(+distance,	-distance,	-distance	);
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(+distance,	+distance,	-distance	);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(-distance,	+distance,	-distance	); 
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(-distance,	-distance,	-distance	);
		glEnd();	

		m_textures[1]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(-distance,	-distance,	+distance	);
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(-distance,	+distance,	+distance	);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(+distance,	+distance,	+distance	);
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(+distance,	-distance,	+distance	); 
		glEnd();

		m_textures[2]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(-distance,	+distance, -distance	);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(-distance,	+distance, +distance	);
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(-distance,	-distance,	+distance	); 
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(-distance,	-distance,	-distance	);
		glEnd();

		m_textures[3]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(+distance,	-distance,	-distance	);
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(+distance,	-distance,	+distance	);
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(+distance,	+distance, +distance	);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(+distance,	+distance, -distance	); 
		glEnd();

		m_textures[4]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(-distance,	-distance,	-distance	);
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(-distance,	-distance,	+distance	);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(+distance,	-distance,	+distance	); 
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(+distance,	-distance,	-distance	);
		glEnd();

		m_textures[5]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex3f(+distance,	+distance,	-distance	);
			glTexCoord2f(0.0f, 0.0f);	glVertex3f(+distance,	+distance,	+distance	);
			glTexCoord2f(1.0f, 0.0f);	glVertex3f(-distance,	+distance,	+distance	); 
			glTexCoord2f(1.0f, 1.0f);	glVertex3f(-distance,	+distance,	-distance	);
		glEnd();

	glPopAttrib();
}
*/