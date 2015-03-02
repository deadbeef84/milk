#include "milk/renderer/ctexture.h"
#include "milk/error.h"
#include "milk/iwindow.h"
#include "milk/cimage.h"
#include "milk/renderer.h"
#include "milk/glhelper.h"

using namespace milk;

//////////////////////////////////////////////////////////////////////////

const GLenum ITexture::ms_targetName[ITexture::NUM_TARGET_TYPES] =
{
	GL_TEXTURE_1D,
	GL_TEXTURE_2D,
	GL_TEXTURE_3D,
	GL_TEXTURE_CUBE_MAP
};

GLuint ITexture::ms_targetBinding[8][ITexture::NUM_TARGET_TYPES] = {0};
bool ITexture::ms_targetEnabled[8][ITexture::NUM_TARGET_TYPES] = {0};
int ITexture::ms_activeUnit = 0;

ITexture::~ITexture()
{
	Renderer::checkForErrors();
	if(m_id)
	{
		if(!glIsTexture(m_id))
			throw error::opengl("Attempting to delete a texture id not identified as a texture.");
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}

void ITexture::setBorderColor(const CColor4f& color)
{
	binder bound(*this);
	glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, (const GLfloat*)color);
}

void ITexture::setQuality(const Quality& quality)
{
	m_imgType.quality = quality;

	binder bound(*this);

	// Wrap S
	switch(m_imgType.quality.wrap_s)
	{
	case Repeat:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		break;
	case Clamp:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, IWindow::getClamp());
		break;
	case ClampToBorder:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		break;
	}

	// Wrap t
	switch(m_imgType.quality.wrap_t)
	{
	case Repeat:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case Clamp:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, IWindow::getClamp());
		break;
	case ClampToBorder:
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	}

	// Mipmapped?
	if (m_imgType.mipmap)
	{
		// Mipmap filter?
		if (m_imgType.quality.mipmap_filter == Nearest)
		{
			// Min-filter (nearest-mipmapped)
			if (m_imgType.quality.min_filter == Nearest)
				glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			else
				glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			// Min-filter (linear-mipmapped)
			if (m_imgType.quality.min_filter == Nearest)
				glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			else
				glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
	}
	else
	{
		// Min-filter (non-mipmapped)
		if (m_imgType.quality.min_filter == Nearest)
			glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else
			glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	// Mag-filter
	if (m_imgType.quality.mag_filter == Nearest)
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ITexture::activateUnit(int unit)
{
	if(ms_activeUnit != unit)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + unit);
		ms_activeUnit = unit;
	}
}

void ITexture::bind(int unit, ITexture& texture)
{
	int targetIndex = getTargetIndex(texture.getTarget());

	// enable and bind the new texture
	if(!ms_targetEnabled[unit][targetIndex])
	{
		activateUnit(unit);
		glEnable(texture.getTarget());
		ms_targetEnabled[unit][targetIndex] = true;
	}
	if(ms_targetBinding[unit][targetIndex] != texture.getTextureId())
	{
		activateUnit(unit);
		texture.bind();
		ms_targetBinding[unit][targetIndex] = texture.getTextureId();
	}

	// make sure higher priority texture-targets are disabled
	for(int i=targetIndex+1; i<NUM_TARGET_TYPES; ++i)
	{
		if(ms_targetEnabled[unit][i])
		{
			activateUnit(unit);
			glDisable(ms_targetName[i]);
			ms_targetEnabled[unit][i] = false;
		}
	}
}

void ITexture::disable(int unit)
{
	// make sure all texture-targets are disabled
	for(int i=0; i<NUM_TARGET_TYPES; ++i)
	{
		if(ms_targetEnabled[unit][i])
		{
			activateUnit(unit);
			glDisable(ms_targetName[i]);
			ms_targetEnabled[unit][i] = false;
		}
	}
}

void ITexture::reset()
{
	for(int unit = 7; unit >= 0; --unit)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + unit);

		// make sure all texture-targets are disabled
		for(int i=0; i<NUM_TARGET_TYPES; ++i)
		{
			glDisable(ms_targetName[i]);
			glBindTexture(ms_targetName[i], 0);
			ms_targetBinding[unit][i] = 0;
			ms_targetEnabled[unit][i] = false;
		}
	}
	ms_activeUnit = 0;
}

int ITexture::getTargetIndex(GLenum target)
{
	for(int i=0; i<NUM_TARGET_TYPES; ++i)
		if(ms_targetName[i] == target)
			return i;
	throw error::milk("boo");
}

ITexture* ITexture::create(const std::string& rid)
{
	return new CTexture2D(CImage(rid));
}

int ITexture::toInternalGLFormat(TextureFormat type)
{
	switch(type)
	{
	case DEPTH16:
		return GL_DEPTH_COMPONENT16;
	case DEPTH24:
		return GL_DEPTH_COMPONENT24;
	case DEPTH32:
		return GL_DEPTH_COMPONENT32;

	case A8:
		return GL_ALPHA;

	case RGB16:
		return GL_RGB16;
	case RGBA16:
		return GL_RGBA16;
	case RGB24:
		return GL_RGB8;
	case RGBA32:
		return GL_RGBA8;

	default:
		throw error::milk("Invalid format argument to CTexture2D::toInternalGLFormat()");
	}
}

int ITexture::toGLFormat(TextureFormat type)
{
	switch(type)
	{
	case DEPTH16:
		return GL_DEPTH_COMPONENT;
	case DEPTH24:
		return GL_DEPTH_COMPONENT;
	case DEPTH32:
		return GL_DEPTH_COMPONENT;

	case A8:
		return GL_ALPHA;

	case RGB16:
		return 2;
	case RGBA16:
		return 2;
	case RGB24:
		return GL_RGB;
	case RGBA32:
		return GL_RGBA;

	default:
		throw error::milk("Invalid format argument to CTexture2D::toGLFormat()");
	}
}

GLenum ITexture::getTargetBinding(GLenum target)
{
	switch(target)
	{
	case GL_TEXTURE_1D:
		return GL_TEXTURE_1D_BINDING_EXT;
	case GL_TEXTURE_2D:
		return GL_TEXTURE_2D_BINDING_EXT;
	case GL_TEXTURE_3D:
		return GL_TEXTURE_3D_BINDING_EXT;
	case GL_TEXTURE_CUBE_MAP:
		return GL_TEXTURE_BINDING_CUBE_MAP_EXT;
	}
	return GL_NONE;
}

//////////////////////////////////////////////////////////////////////////

void CTexture2D::private_ctor(const uchar* mem, const CVector2<int>& size,
					   TextureFormat inFormat, const ImageType& imgType)
{
	CHECK_FOR_OPENGL_ERRORS("There were errors prior to this function.");

	if(!m_id)
	{
		glGenTextures(1, &m_id);
		CHECK_FOR_OPENGL_ERRORS("glGenTextures() failed.");
	}

	m_size		= size;
	m_imgType	= imgType;

	binder bound(*this);

	CHECK_FOR_OPENGL_ERRORS("glBindTexture() failed.");

	////////////////////////////////////////////////////

	int format = toInternalGLFormat(m_imgType.type);
	int format2 = toGLFormat(inFormat);

	if(m_imgType.mipmap)
	{
		if(GLEW_SGIS_generate_mipmap)
		{
			glTexParameteri(m_target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexImage2D(m_faceTarget, 0, format, m_size.x, m_size.y, 0, format2, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(mem));
			glTexParameteri(m_target, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
		}
		else
			gluBuild2DMipmaps(m_faceTarget, format, m_size.x, m_size.y, format2, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(mem));
	}
	else
		glTexImage2D(m_faceTarget, 0, format, m_size.x, m_size.y, 0, format2, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(mem));

	CHECK_FOR_OPENGL_ERRORS("gluBuild2DMipmaps() or glTexImage2D() failed.");

	setQuality(getQuality());
	CHECK_FOR_OPENGL_ERRORS("CTexture::setQuality() failed.");
}

CTexture2D::CTexture2D(const CImage& image, const ImageType& imgType)
: ITexture(GL_TEXTURE_2D), m_faceTarget(GL_TEXTURE_2D)
{
	private_ctor(reinterpret_cast<const uchar*>(image.getDataPointer()),
		image.size(), RGBA32, imgType);
}

CTexture2D::CTexture2D(const uchar* mem, const CVector2<int>& size,
						TextureFormat inFormat, const ImageType& imgType)
: ITexture(GL_TEXTURE_2D), m_faceTarget(GL_TEXTURE_2D)
{
	private_ctor(mem, size, inFormat, imgType);
}

CTexture2D::CTexture2D(const CVector2<int>& size, const ImageType& imgType)
: ITexture(GL_TEXTURE_2D), m_faceTarget(GL_TEXTURE_2D)
{
	private_ctor(0, size, imgType.type, imgType);
}

/*
CTexture2D::CTexture2D()
: ITexture(GL_TEXTURE_2D), m_faceTarget(GL_TEXTURE_2D)
{
	glGenTextures(1, &m_id);
	binder bound(*this);
	if(!glIsTexture(m_id))
		throw error::opengl("An error occurred when generating a texture-id.");
}
*/

void CTexture2D::setShadowMap()
{
	if(!GLEW_ARB_shadow)
		throw error::opengl("ARB_shadow not supported");

	binder bound(*this);

	//Enable shadow comparison
	glTexParameteri(m_target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

	//Shadow comparison should be true (ie not in shadow) if r<=texture
	glTexParameteri(m_target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	//Shadow comparison should generate an INTENSITY result
	glTexParameteri(m_target, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
}

//////////////////////////////////////////////////////////////////////////

CCubeMap::CCubeMap()
: ITexture(GL_TEXTURE_CUBE_MAP)
{
	if(!GLEW_EXT_texture_cube_map)
		throw error::opengl("EXT_texture_cube_map not supported.");
}

CCubeMap::~CCubeMap()
{
}
