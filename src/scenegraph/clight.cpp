#include "milk/scenegraph/clight.h"
#include "milk/includes.h"
#include "milk/scenegraph/cscenemanager.h"
using namespace milk;

// clight class implementation
///////////////////////////////////////////////

CLight::CLight(LightType type/* =DIRECTIONAL */)
: m_id(-1),
  m_type(type),
  m_cutoff(45.0f), m_exponent(0.0f),
  m_intensity(1.0f),
  m_ambient(0.0f, 0.0f, 0.0f, 1.0f), m_diffuse(1.0f, 1.0f, 1.0f, 1.0f), m_specular(1.0f, 1.0f, 1.0f, 1.0f),
  m_attConstant(1.0f), m_attLinear(0.0f), m_attQuadtratic(0.0f)
{
}

CLight::~CLight()
{
	disable();
}

void CLight::enable()
{
	if(!isEnabled())
	{
		m_id = getFreeLight();
		if(m_id != -1)
			glEnable(GL_LIGHT0 + m_id);
	}
}

void CLight::disable()
{
	if(isEnabled())
	{
		glDisable(GL_LIGHT0 + m_id);
		m_id = -1;
	}
}

bool CLight::isEnabled()
{
	if(m_id >= 0)
		return glIsEnabled(GL_LIGHT0 + m_id) ? true : false;
	return false;
}

uint CLight::getMaxLights()
{
	GLint temp;
	glGetIntegerv(GL_MAX_LIGHTS, &temp);
	return temp;
}

void CLight::disableAll()
{
	static const uint max_lights = getMaxLights();
	for(uint i = 0; i < max_lights; ++i)
		glDisable(GL_LIGHT0 + i);
}

int CLight::getFreeLight()
{
	uint max_lights = getMaxLights();
	for(uint i = 0; i < max_lights; ++i)
		if(!glIsEnabled(GL_LIGHT0 + i))
			return i;
	return -1;
}

void CLight::apply()
{
	if(isEnabled())
	{
		glPushMatrix();
		{
			// LTM
			//doTransform();
			glMultMatrix(ltm());

			// Color [TODO: take intensity into account. ]
			glLightfv(GL_LIGHT0+m_id, GL_AMBIENT, (float*)m_ambient);
			glLightfv(GL_LIGHT0+m_id, GL_DIFFUSE, (float*)m_diffuse);
			glLightfv(GL_LIGHT0+m_id, GL_SPECULAR, (float*)m_specular);

			// Attenuation
			glLightf(GL_LIGHT0+m_id, GL_CONSTANT_ATTENUATION, m_attConstant);
			glLightf(GL_LIGHT0+m_id, GL_LINEAR_ATTENUATION, m_attLinear);
			glLightf(GL_LIGHT0+m_id, GL_QUADRATIC_ATTENUATION, m_attQuadtratic);

			// Set cutoff & exponent
			if(m_type == SPOT)
			{
				glLightf(GL_LIGHT0+m_id, GL_SPOT_CUTOFF, m_cutoff);
				glLightf(GL_LIGHT0+m_id, GL_SPOT_EXPONENT, m_exponent);
			}
			else
			{
				// Set default values for cutoff/exponent
				glLightf(GL_LIGHT0+m_id, GL_SPOT_CUTOFF, 180.0f);
				glLightf(GL_LIGHT0+m_id, GL_SPOT_EXPONENT, 0.0f);
			}

			if(m_type == DIRECTIONAL)
			{
				// Set light direction
				static const float ldir[4] = { 0, 0, -1, 0 };
				glLightfv(GL_LIGHT0+m_id, GL_POSITION, ldir);

				// Unset spot direction
				static const float sdir[3] = { 0, 0, -1 };
				glLightfv(GL_LIGHT0+m_id, GL_SPOT_DIRECTION, sdir);
			}
			else
			{
				// Set position
				static const float lpos[4] = { 0, 0, 0, 1 };
				glLightfv(GL_LIGHT0+m_id, GL_POSITION, lpos);

				// Set spot direction
				static const float sdir[3] = { 0, 0, 1 };
				glLightfv(GL_LIGHT0+m_id, GL_SPOT_DIRECTION, sdir);
			}
		}
		glPopMatrix();
	}
}

void CLight::setGlobalAmbient(const CColor4f& ambient)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const float*)ambient);
}

void CLight::onNewSceneManager(CSceneManager *pSceneManager)
{
	if(m_pSceneManager)
		m_pSceneManager->removeLight(this);
	if(pSceneManager)
		pSceneManager->addLight(this);
}
