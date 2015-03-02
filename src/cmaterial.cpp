#include "milk/renderer/cmaterial.h"
#include "milk/renderer.h"
#include "milk/helper.h"
#include "milk/renderer/irenderpass.h"
#include "milk/scenegraph/cscenemanager.h"
#include <algorithm>
using namespace milk;
using namespace std;

//////////////////////////////////////////////////////////////////////////

bool Material::ms_vertexColor = true;

void Material::bind() const
{
	int twoSidedLighting;
	glGetIntegerv(GL_LIGHT_MODEL_TWO_SIDE, &twoSidedLighting);
	GLenum lightTarget = twoSidedLighting?GL_FRONT_AND_BACK:GL_FRONT;

	if(m_lighting)
	{
		glEnable(GL_LIGHTING);

		if(m_vertexColorTracking)
			glEnable(GL_COLOR_MATERIAL);
		else
		{
			glDisable(GL_COLOR_MATERIAL);
			glMaterialfv(lightTarget, GL_AMBIENT, m_ambient);
			glMaterialfv(lightTarget, GL_DIFFUSE, m_diffuse);
		}

		glMaterialfv(lightTarget, GL_EMISSION, m_emissive);
		glMaterialfv(lightTarget, GL_SPECULAR, m_specular);
		glMaterialf(lightTarget, GL_SHININESS, m_shininess);
	}
	else
	{
		glDisable(GL_LIGHTING);
		if(!m_vertexColorTracking)
			glColor4fv(m_diffuse);
	}
	ms_vertexColor = m_vertexColorTracking;
}

//////////////////////////////////////////////////////////////////////////

bool PolygonMode::ms_swapWinding = false;
GLenum PolygonMode::ms_winding = GL_CCW;

void PolygonMode::setWindingGL(GLenum winding)
{
	// Setup winding
	ms_winding = winding;
	if(ms_swapWinding)
		winding = (winding == GL_CCW) ? GL_CW : GL_CCW;
	glFrontFace(winding);
}

void PolygonMode::bind() const
{
	// Setup shading
	glShadeModel(m_shading);

	// Setup culling
	if(m_culling == GL_NONE)
		glDisable(GL_CULL_FACE);
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(m_culling);
	}

	// Setup winding
	setWindingGL(m_winding);

	// Set polygon rasterization mode
	if(m_rasterModeFront == m_rasterModeBack)
		glPolygonMode(GL_FRONT_AND_BACK, m_rasterModeFront);
	else
	{
		glPolygonMode(GL_FRONT, m_rasterModeFront);
		glPolygonMode(GL_BACK, m_rasterModeBack);
	}

	// Set light model
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, m_localCameraLighting?1:0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, m_twoSidedLighting?1:0);
}

//////////////////////////////////////////////////////////////////////////

void CompositingMode::setBlending(Blending blending)
{
	switch(blending)
	{
	case ALPHA_ADD:
		setBlending(GL_SRC_ALPHA, GL_ONE);
		break;
	case ALPHA:
		setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case MODULATE:
		setBlending(GL_DST_COLOR, GL_ZERO);
		break;
	case MODULATE_X2:
		setBlending(GL_DST_COLOR, GL_SRC_COLOR);
		break;
	case REPLACE:
		setBlending(GL_ONE, GL_ZERO);
	}
}

CompositingMode::Blending CompositingMode::getBlending() const
{
	// TODO
	if(m_blendSrc == GL_SRC_ALPHA && m_blendDst == GL_ONE)
		return ALPHA_ADD;
	if(m_blendSrc == GL_SRC_ALPHA && m_blendDst == GL_ONE_MINUS_SRC_ALPHA)
		return ALPHA;
	if(m_blendSrc == GL_DST_COLOR && m_blendDst == GL_ZERO)
		return MODULATE;
	if(m_blendSrc == GL_DST_COLOR && m_blendDst == GL_SRC_COLOR)
		return MODULATE_X2;
	if(m_blendSrc == GL_ONE && m_blendDst == GL_ZERO)
		return REPLACE;
	return OTHER;
}

void CompositingMode::bind() const
{
	// Setup depth-test
	if(m_depthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(m_depthFunc);
	}
	else
		glDisable(GL_DEPTH_TEST);

	// TODO: implement blend-equation
	//glBlendEquation(GL_FUNC_ADD);

	// Setup depth and color writes
	glDepthMask(m_depthWrite);
	glColorMask(m_colorWrite, m_colorWrite, m_colorWrite, m_alphaWrite);

	// Setup alpha testing		
	if(m_alphaTest)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(m_alphaFunc, m_alphaRef);
	}
	else
		glDisable(GL_ALPHA_TEST);

	// Setup blending
	if(m_blendSrc != GL_ONE || m_blendDst != GL_ZERO)
	{
		glBlendFunc(m_blendSrc, m_blendDst);
		glEnable(GL_BLEND);
	}
	else
		glDisable(GL_BLEND);

	// Setup depth offset
	if(m_depthOffsetFactor != 0 || m_depthOffsetUnits != 0)
	{
		glPolygonOffset(m_depthOffsetFactor, m_depthOffsetUnits);
		glEnable(GL_POLYGON_OFFSET_FILL);
	}
	else
		glDisable(GL_POLYGON_OFFSET_FILL);
}

//////////////////////////////////////////////////////////////////////////

void CPass::unloadShader()
{
	if(m_programObject)
	{
		// remove old program if needed.
		if(m_vertexShader)
			m_programObject->detach(*m_vertexShader);
		if(m_fragmentShader)
			m_programObject->detach(*m_fragmentShader);
	}
	m_programObject = 0;
}

void CPass::loadShader(const string& vfilename, const string& ffilename)
{
	unloadShader();

	m_programObject = new CProgramObject();

	if(!vfilename.empty())
	{
		m_vertexShader = vfilename;
		if(m_vertexShader)
			m_programObject->attach(*m_vertexShader);
	}

	if(!ffilename.empty())
	{
		m_fragmentShader = ffilename;
		if(m_fragmentShader)
			m_programObject->attach(*m_fragmentShader);
	}

	m_programObject->link();
}

void CPass::bind()
{
	for(int i=0; i < 8; ++i)
	{
		if(m_textures[i])
			ITexture::bind(i, *m_textures[i]);
		else
			ITexture::disable(i);
	}
	Helper<Material>::bind(m_pMaterial);
	Helper<PolygonMode>::bind(m_pPolygonMode);
	Helper<CompositingMode>::bind(m_pCompositingMode);
	if(m_programObject)
		m_programObject->bind();
	else
		CProgramObject::unbind();
}
